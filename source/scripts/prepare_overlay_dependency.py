# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
REVISION = "d1e8c830b00928ab94a44e1d5323613af95263c6"
upstream = ROOT / ".deps" / "frame-mic-tuner"
if not upstream.exists():
    upstream.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(["git", "clone", "https://github.com/sasaken1102r/frame-mic-tuner", str(upstream)], check=True)
subprocess.run(["git", "-C", str(upstream), "checkout", "--detach", REVISION], check=True)
vendor = ROOT / "pc" / "frame_app" / "vendor"
vendor.mkdir(parents=True, exist_ok=True)
for name in ("vr_overlay.cpp", "vr_overlay.h", "vk_texture.cpp", "vk_texture.h"):
    shutil.copy2(upstream / "src" / name, vendor / name)
shutil.copy2(upstream / "LICENSE", vendor / "LICENSE")
p = vendor / "vr_overlay.cpp"
text = p.read_text(encoding="utf-8")
import re
text, key_count = re.subn(r'(kDashboardKey\s*=\s*)"[^"]*"', r'\1"pico.frame-tracker"', text)
text, name_count = re.subn(r'(kDashboardName\s*=\s*)"[^"]*"', r'\1"PICO"', text)
if key_count != 1 or name_count != 1:
    raise RuntimeError("Upstream branding layout changed; review before building")
p.write_text(text, encoding="utf-8")
print("Prepared pinned overlay dependency; upstream license retained")
