# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Bounded official tracking recording; restore the camera-recording setting."""
import json,subprocess,time,shutil
from pathlib import Path
root=Path('/home/steamos/pico-ram-work')
tool='/opt/steamvr/bin/linuxarm64/vrcmd'
folder=Path('/home/steamos/.local/share/Steam/config/cv/xrservice/datasets')
report={'before':sorted(p.name for p in folder.iterdir()) if folder.exists() else []}
if shutil.disk_usage(root).free<500*1024*1024:raise RuntimeError('Insufficient recording space')
old=None
try:
    result=subprocess.run([str(root/'frame_camera_recording_setting'),'on'],check=True,capture_output=True,text=True,timeout=8)
    change=json.loads(result.stdout);old=change['old'];report['setting_change']=change
    if change['error'] or not change['actual']:raise RuntimeError('Camera recording setting failed')
    started=subprocess.run([tool,'--startlighthouserecording'],capture_output=True,text=True,timeout=8)
    report['start']={'returncode':started.returncode,'stdout':started.stdout,'stderr':started.stderr}
    if started.returncode:raise RuntimeError('Tracking recording command failed')
    time.sleep(3)
finally:
    stopped=subprocess.run([tool,'--stoplighthouserecording'],capture_output=True,text=True,timeout=8)
    report['stop']={'returncode':stopped.returncode,'stdout':stopped.stdout,'stderr':stopped.stderr}
    if old is not None:
        restored=subprocess.run([str(root/'frame_camera_recording_setting'),'on' if old else 'off'],capture_output=True,text=True,timeout=8)
        report['setting_restore']={'returncode':restored.returncode,'stdout':restored.stdout,'stderr':restored.stderr}
    report['after']=sorted(p.name for p in folder.iterdir()) if folder.exists() else []
    (root/'frame-dataset-probe.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report))
