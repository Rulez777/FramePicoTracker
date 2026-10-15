# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Cross-process Roy ownership, shared by loaders and recovery tools.

The advisory lock lives outside the deployment directory so two different
checkouts cannot reserve the same MCU concurrently. It never kills an owner.
"""
import os
from pathlib import Path


class RoyOwnership:
    def __init__(self, path=None):
        # Root and the ordinary Frame account must share the same lock too.
        # Privilege changes cannot create another independent MCU owner.
        frame_home=Path('/home/steamos')
        owner_home=frame_home if os.name=='posix' and frame_home.is_dir() else Path.home()
        self.path = Path(path or owner_home / '.local/state/frame-pico/roy.lock')
        self.file = None

    def __enter__(self):
        import fcntl
        self.path.parent.mkdir(parents=True, exist_ok=True)
        self.file = self.path.open('a+')
        try:
            fcntl.flock(self.file.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            self.file.close()
            self.file = None
            raise RuntimeError('Roy is owned by another loader or recovery process') from None
        self.file.seek(0)
        self.file.truncate()
        self.file.write(str(os.getpid()))
        self.file.flush()
        return self

    def __exit__(self, *args):
        import fcntl
        if self.file is not None:
            fcntl.flock(self.file.fileno(), fcntl.LOCK_UN)
            self.file.close()
            self.file = None
