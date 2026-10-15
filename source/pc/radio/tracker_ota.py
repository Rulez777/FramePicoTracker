# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""FW189 OTA wire builders and stop-and-wait sender; no hardware I/O.

Command 1 / subcommand 0x25 -> table at 0x16558 -> 0x88cc -> 0xfbcc.
Only application ACKs advance the sender. A transport ACK is insufficient.
No automatic reconnect/resume: a lost peer requires a new reviewed session.
"""
from dataclasses import dataclass
import hashlib
import math
import time

from .swift_light_sync import pack, record
from .swift_records import decode_records
from .tracker_image_audit import crc8_maxim

BLOCK = 24


def start_body(image: bytes, version: int = 189) -> bytes:
    # The FW189 CRC routine always hashes the first full 24-byte block.
    if not BLOCK <= len(image) <= 0xffffff or len(image) // BLOCK + 1 > 0xffff:
        raise ValueError('Image exceeds FW189 size/block fields')
    if not 1 <= version <= 0xffff:
        raise ValueError('Invalid OTA version')
    return b'\x25\x01' + len(image).to_bytes(4, 'big') + bytes([crc8_maxim(image)]) + version.to_bytes(2, 'big')


def data_body(image: bytes, index: int) -> bytes:
    # FW189 calculates floor(size/24)+1, even for exact multiples. The extra
    # zero-length sentinel block is stored but excluded from the final CRC.
    count = len(image) // BLOCK + 1
    if not 1 <= index <= count:
        raise ValueError('Invalid OTA block index')
    data = image[(index - 1) * BLOCK:index * BLOCK]
    return b'\x25\x02' + index.to_bytes(2, 'big') + bytes([len(data)]) + data.ljust(BLOCK, b'\xff')


def finish_body() -> bytes:
    return b'\x25\x03'


@dataclass(frozen=True)
class OtaAck:
    operation: int
    success: bool
    index: int | None = None


def decode_ack(body: bytes) -> OtaAck:
    if len(body) == 3 and body[:2] in (b'\x25\x01', b'\x25\x03') and body[2] in (0, 1):
        return OtaAck(body[1], bool(body[2]))
    if (len(body) in (5, 6) and body[:2] == b'\x25\x02' and body[4] in (0, 1)
            and (len(body) == 5 or body[5] == 0xaa)):
        return OtaAck(2, bool(body[4]), int.from_bytes(body[2:4], 'big'))
    raise ValueError('Not a complete FW189 OTA application ACK')


class OtaSession:
    def __init__(self, image: bytes, expected_sha256: str, *, sequence: int = 0,
                 now=time.monotonic, retry_s=.25, data_timeout_s=10.,
                 setup_timeout_s=30., overall_timeout_s=1800., initial_generation=0,
                 response_poll=False, capture_ack=False):
        if hashlib.sha256(image).hexdigest() != expected_sha256:
            raise ValueError('OTA image hash mismatch')
        self.start_packet = start_body(image)
        limits=(retry_s,data_timeout_s,setup_timeout_s,overall_timeout_s)
        if (type(sequence) is not int or not 0 <= sequence < 64
                or any(type(v) not in (int,float) or not math.isfinite(v) or v<=0 for v in limits)):
            raise ValueError('Invalid OTA sender limits')
        if type(initial_generation) is not int or not 0 <= initial_generation <= 0xffffffff or initial_generation & 1:
            raise ValueError('OTA generation must follow the even peer mailbox generation')
        self.image = bytes(image); self.sequence = sequence; self.now = now
        self.image_sha256 = expected_sha256
        self.retry_s = retry_s; self.data_timeout_s = data_timeout_s
        self.setup_timeout_s = setup_timeout_s; self.overall_timeout_s = overall_timeout_s
        self.state = 'ready'; self.index = 0; self.acked_blocks = 0
        self.generation = initial_generation; self.attempts = 0; self.retries = 0
        self.rejected = 0; self.error = None; self.s1 = 2
        self.started = None; self.operation_started = None; self.last_attempt = None
        self.next_state = None; self.attempt_tags = set()
        self.published_tags = set()
        self.empty_prestart_reply_observed = False
        if type(response_poll) is not bool:raise ValueError('Invalid response-poll policy')
        self.response_poll = response_poll
        if type(capture_ack) is not bool or capture_ack and not response_poll:
            raise ValueError('Capture ACK requires response polls')
        self.capture_ack = capture_ack
        self.data_request = True
        self.response_polls = 0

    def _next_generation(self):
        # MCU returns only the low 16 bits. Never reuse an earlier tag in this
        # connection, including pre-OTA peer generations. Exhaustion fails closed.
        if self.generation & 0xffff >= 0xfffe:
            self.abort('OTA 16-bit transmission tag space exhausted; automatic wrap forbidden')
            return False
        self.generation += 2
        return True

    @property
    def blocks(self):
        return len(self.image) // BLOCK + 1

    @property
    def terminal(self):
        return self.state in ('finish_verified', 'failed')

    def begin(self):
        if self.state != 'ready':
            raise RuntimeError('OTA sessions cannot restart or resume implicitly')
        # start + finish + every block, each followed by a drain operation.
        # Reject even the no-retry transfer before submitting a destructive start
        # if it cannot fit the remaining unique low-16-bit tags.
        minimum_tags = (3 if self.response_poll else 2) * self.blocks + 5
        if (0xfffe - (self.generation & 0xffff)) // 2 < minimum_tags:
            self.abort('Insufficient unique OTA transmission tags before start; automatic wrap forbidden')
            return
        self.started = self.now();self.s1 = 3;self._operation('prepare_ack')

    def _operation(self, state):
        self.state = state; self.operation_started = self.now(); self.last_attempt = self.now()
        self.attempts = 1
        self.published_tags = set()
        self.data_request = True
        if not self._next_generation():return
        self.attempt_tags = {self.generation & 0xffff}

    def abort(self, reason):
        self.state = 'failed'; self.error = str(reason)

    def tick(self):
        if self.terminal or self.state == 'ready':return
        stamp = self.now()
        limit = self.data_timeout_s if self.state in ('data', 'drain_ack') else self.setup_timeout_s
        if stamp - self.started >= self.overall_timeout_s or stamp - self.operation_started >= limit:
            self.abort(f'OTA {self.state} block {self.index}: application ACK deadline expired');return
        if stamp - self.last_attempt >= self.retry_s:
            self.last_attempt = stamp; self.attempts += 1; self.retries += 1
            # Same command sequence/body: do not submit another erase/write
            # while the FW189 application is still processing this operation.
            # The mailbox is polled continuously by the MCU. This unchanged
            # operation keeps its tag; a new operation alone consumes a tag.
            # The default policy's pre-start budget covers arbitrary retries.
            # Response-poll content switches spend additional tags and retain
            # the same fail-closed exhaustion check.
            if self.response_poll and self.state == 'data' and not self.data_request:
                self._data_publication(True)

    def _data_publication(self, request):
        # Different mailbox content requires a fresh seqlock generation. Keep
        # earlier tags eligible within this operation only: the native worker
        # may queue its ACK after the request's immediate telemetry reply.
        if not self._next_generation():return
        self.attempt_tags.add(self.generation & 0xffff)
        self.data_request = request
        if not request:self.response_polls += 1

    def mailbox(self):
        if self.state == 'ready':raise RuntimeError('Begin the sender explicitly')
        if not self.terminal:self.published_tags.add(self.generation & 0xffff)
        if self.terminal or self.state in ('prepare_ack', 'drain_ack'):return self.generation, self.s1, b'\0'
        if self.response_poll and self.state == 'data' and not self.data_request:
            return self.generation, 2, b'\0'  # Read notifications without retiring them.
        body = (self.start_packet if self.state == 'start' else
                data_body(self.image, self.index) if self.state == 'data' else finish_body())
        return self.generation, self.s1, pack([record(self.sequence, 1, body)])

    def receive(self, payload: bytes, *, crc_valid: bool, s1: int, tx_generation: int = 0):
        if self.terminal or self.state == 'ready':return False
        self.tick()
        if self.terminal:return False
        if crc_valid is not True or s1 not in (2, 3):self.rejected += 1;return False
        if (type(tx_generation) is not int or tx_generation not in self.published_tags
                or tx_generation not in self.attempt_tags):
            self.rejected += 1;return False
        try:commands = decode_records(payload)
        except ValueError:self.rejected += 1;return False
        if self.state == 'prepare_ack':
            # Clear reliable notifications before issuing a destructive start.
            # A full empty working reply is required: another queue/telemetry
            # record may otherwise hide an old OTA record behind the RF budget.
            # This proves only the visible queue boundary, not absence of a
            # previously running asynchronous OTA task.
            if payload != b'\0':self.rejected += 1;return False
            self.empty_prestart_reply_observed = True
            self.s1 = 2;self._operation('start');return True
        if self.state == 'drain_ack':
            # Send ACK-only polls until a real reply proves one was transmitted.
            # Repeating a data packet with S1 bit0 set would acknowledge that
            # block's newly generated application ACK before the host saw it.
            # A repeated application ACK is not proof the queue was drained.
            # Stay on ACK-only polls until the reply is exactly empty; telemetry can hide another ACK.
            if payload != b'\0':
                self.rejected += 1;return False
            self.s1 = 2
            if self.next_state == 'finish_verified':self.state = 'finish_verified'
            else:self._operation(self.next_state)
            return True
        expected = {'start': 1, 'data': 2, 'finish': 3}[self.state]
        for command in commands:
            if command.command != 1:continue
            try:ack = decode_ack(command.body)
            except ValueError:continue
            if ack.operation != expected or (expected == 2 and ack.index != self.index):
                self.rejected += 1;continue
            if not ack.success:
                self.abort(f'OTA negative application ACK: operation {expected}, block {self.index}');return False
            self.s1 = 3  # Drain this record with separate ACK-only polls.
            self.sequence = (self.sequence + 1) & 63
            if expected == 1:self.index = 1;self.next_state = 'data'
            elif expected == 2:
                self.acked_blocks += 1
                if self.index == self.blocks:self.next_state = 'finish'
                else:self.index += 1;self.next_state = 'data'
            else:self.next_state = 'finish_verified'
            # Indexed ACKs cannot satisfy another block. With retained-only
            # MCU ACK ownership, old reliable replies retire independently;
            # telemetry need not become empty between indexed operations.
            if expected == 2 and self.capture_ack:
                self.s1 = 2;self._operation(self.next_state)
            else:self._operation('drain_ack')
            # A single RF reply may contain repeated old ACKs. Never consume
            # two operations from one received payload.
            return True
        if (self.response_poll and self.state == 'data' and self.data_request
                and tx_generation == (self.generation & 0xffff)):
            # A checksum-valid reply from a published request tag is evidence
            # of a completed request slot, not application acceptance. The
            # 33-byte request leaves only 32 reply bytes in native 0x5a50;
            # a 1-byte poll restores the 54-byte response budget. S1 stays 2.
            self._data_publication(False)
        return False

    def status(self):
        return dict(state=self.state, block=self.index, total_blocks=self.blocks,
                    acked_blocks=self.acked_blocks, retries=self.retries, rejected=self.rejected,
                    error=self.error, image_sha256=self.image_sha256,
                    empty_prestart_reply_observed=self.empty_prestart_reply_observed,
                    native_ota_idle_verified=False,
                    command_sequence=self.sequence,tx_generation=self.generation,
                    response_poll=self.response_poll,data_request=self.data_request,
                    capture_ack=self.capture_ack,
                    response_polls=self.response_polls,
                    boot_verified=False, cadence_verified=False)
