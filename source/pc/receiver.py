# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""TCP Receiver for Pico Motion Tracker Wire Protocol v1.

Listens on TCP 127.0.0.1:39500 for incoming connections forwarded via
`adb reverse tcp:39500 tcp:39500`.
"""

from __future__ import annotations

import logging
import socket
from typing import Callable, Optional

from protocol import (
    HEADER_SIZE,
    TYPE_FRAME,
    TYPE_HELLO,
    FramePayload,
    HelloPayload,
    unpack_frame,
    unpack_header,
    unpack_hello,
)

logger = logging.getLogger(__name__)

DEFAULT_TCP_HOST = "127.0.0.1"
DEFAULT_TCP_PORT = 39500


def _recv_exact(sock: socket.socket, num_bytes: int) -> Optional[bytes]:
    """Read exactly num_bytes from socket, or return None if disconnected."""
    buf = bytearray()
    while len(buf) < num_bytes:
        chunk = sock.recv(num_bytes - len(buf))
        if not chunk:
            return None
        buf.extend(chunk)
    return bytes(buf)


class StreamReceiver:
    """TCP Server receiving Wire Protocol v1 messages from Pico headset."""

    def __init__(
        self,
        host: str = DEFAULT_TCP_HOST,
        port: int = DEFAULT_TCP_PORT,
        on_hello: Optional[Callable[[HelloPayload], None]] = None,
        on_frame: Optional[Callable[[FramePayload], None]] = None,
    ) -> None:
        self.host = host
        self.port = port
        self.on_hello = on_hello
        self.on_frame = on_frame
        self._running = False
        self._server_sock: Optional[socket.socket] = None

    def start(self) -> None:
        """Run blocking server loop."""
        self._running = True
        self._server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._server_sock.bind((self.host, self.port))
        self._server_sock.listen(1)
        logger.info("TCP Receiver listening on %s:%d", self.host, self.port)

        while self._running:
            try:
                logger.info("Waiting for connection from Pico app (via ADB reverse)...")
                conn, addr = self._server_sock.accept()
                conn.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                logger.info("Connected to %s", addr)
                self._handle_client(conn)
            except OSError as e:
                if not self._running:
                    break
                logger.warning("Socket error in accept: %s", e)

    def stop(self) -> None:
        """Stop server."""
        self._running = False
        if self._server_sock:
            try:
                self._server_sock.close()
            except Exception:
                pass

    def _handle_client(self, conn: socket.socket) -> None:
        with conn:
            while self._running:
                # 1. Read header
                hdr_bytes = _recv_exact(conn, HEADER_SIZE)
                if hdr_bytes is None:
                    logger.info("Client disconnected")
                    break

                try:
                    hdr = unpack_header(hdr_bytes)
                except ValueError as e:
                    logger.error("Protocol error unpacking header: %s", e)
                    break

                # 2. Read payload
                payload_bytes = _recv_exact(conn, hdr.length)
                if payload_bytes is None:
                    logger.warning("Client disconnected before payload finished")
                    break

                # 3. Parse and dispatch
                try:
                    if hdr.msg_type == TYPE_HELLO:
                        hello = unpack_hello(payload_bytes)
                        logger.info("Received Hello: %s", hello)
                        if self.on_hello:
                            self.on_hello(hello)
                    elif hdr.msg_type == TYPE_FRAME:
                        frame = unpack_frame(payload_bytes)
                        if self.on_frame:
                            self.on_frame(frame)
                except Exception as e:
                    logger.error("Error processing message type %d: %s", hdr.msg_type, e)
