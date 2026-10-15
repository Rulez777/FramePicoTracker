# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Read-only Linux diagnostic endpoint for the Frame research workspace."""
import argparse
from datetime import datetime, timezone
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
from pathlib import Path
import time
import subprocess
import threading
import uuid
import re
import tarfile
import shutil
from urllib.parse import urlparse
try:
    from .frame_app_control import AppControl
    from .frame_profile_status import live_status,current_stream
except ImportError:
    from frame_app_control import AppControl
    from frame_profile_status import live_status,current_stream

ROOT = Path('/home/steamos/pico-ram-work')
START = time.monotonic()
DATASETS = Path('/home/steamos/.local/share/Steam/config/cv/xrservice/datasets')
LOCK = threading.Lock()
RADIO_LOCK = threading.Lock()
JOBS = {}
ACTIVE = None
VRCMD = '/opt/steamvr/bin/linuxarm64/vrcmd'


def camera_command(start):
    command = 'start_tracking_recording 1 0 0' if start else 'stop_tracking_recording'
    result = subprocess.run([VRCMD,'--cmd','0',command],capture_output=True,text=True,timeout=5)
    if result.returncode != 0 or 'response: ok' not in result.stdout:
        raise RuntimeError('Camera recording command failed: '+result.stdout+result.stderr)
    return result.stdout.strip()
PAGE = b'''<!doctype html><meta charset="utf-8"><title>FramePicoTracker</title>
<body style="font:18px system-ui;background:#101820;color:#eef;padding:30px;max-width:900px">
<h1>FramePicoTracker</h1><p>Diagnostic capture: cameras, radio packets and Frame poses. Screen and audio recording disabled.</p>
<button id="capture">Record diagnostics (10 seconds)</button> <button id="refresh">Refresh status</button>
<p id="result"></p><pre id="status" style="white-space:pre-wrap;font-size:13px"></pre>
<script>
async function status(){document.getElementById('status').textContent=JSON.stringify(await(await fetch('/status')).json(),null,2)}
document.getElementById('refresh').onclick=status;
document.getElementById('capture').onclick=async()=>{
const button=document.getElementById('capture');button.disabled=true;
try{let r=await fetch('/capture',{method:'POST'});let job=await r.json();if(!r.ok)throw Error(job.error);
document.getElementById('result').textContent='Recording '+job.id;
while(true){await new Promise(r=>setTimeout(r,1000));let j=await(await fetch('/capture/'+job.id)).json();
if(j.state==='complete'){let a=document.createElement('a');a.href='/capture/'+job.id+'/bundle';a.textContent='Download diagnostic bundle';document.getElementById('result').replaceChildren(a);break}
if(j.state==='failed')throw Error(JSON.stringify(j));}await status();
}catch(e){document.getElementById('result').textContent=e.message}finally{button.disabled=false}}
status();</script>'''


def dataset_inventory():
    result = []
    for directory in sorted(DATASETS.glob('*.dataset'), key=lambda p:p.stat().st_mtime, reverse=True)[:5]:
        files = [{ 'name':p.name, 'bytes':p.stat().st_size, 'mtime':p.stat().st_mtime}
                 for p in directory.iterdir() if p.is_file()]
        result.append({'name':directory.name, 'files':files,
                       'scope':'Recorded dataset, not a live camera feed'})
    return result


def capture_worker(identifier):
    """Record an existing station stream; never open a second SWD owner."""
    global ACTIVE
    prefix=ROOT/('api-'+identifier)
    started=time.time()
    try:
        with LOCK: JOBS[identifier]['state']='running'
        if not radio_status().get('running'):
            raise RuntimeError('Station is not running; diagnostics cannot start radio probes')
        _,log=current_stream(ROOT)
        if not log.exists():
            raise RuntimeError('Station record stream is unavailable')
        received=0
        with log.open('rb') as source, prefix.with_suffix('.records.jsonl').open('wb') as output:
            source.seek(0,2)
            initial=source.tell()
            deadline=time.monotonic()+10
            while time.monotonic()<deadline:
                if not radio_status().get('running'):
                    raise RuntimeError('Station stopped or became stale during capture')
                block=source.read(65536)
                if block:
                    received+=len(block)
                    if received>8*1024*1024:
                        raise RuntimeError('Diagnostic byte budget exceeded')
                    output.write(block)
                else:
                    # Rotation must not replay the saved history as live.
                    if log.stat().st_ino!=__import__('os').fstat(source.fileno()).st_ino:
                        raise RuntimeError('Station log rotated; repeat capture from current stream')
                    time.sleep(.02)
        manifest={'id':identifier,'started_unix':started,'finished_unix':time.time(),
                  'radio_bytes':received,'history_bytes_skipped':initial,
                  'source':'existing station stream','opens_swd':False,
                  'controls_camera_recording':False,'tracker_poses_available':False}
        prefix.with_suffix('.manifest.json').write_text(json.dumps(manifest,indent=2))
        with tarfile.open(prefix.with_suffix('.tar.gz'),'w:gz') as archive:
            for suffix in ('.records.jsonl','.manifest.json'):
                path=prefix.with_suffix(suffix)
                archive.add(path,arcname=path.name)
        with LOCK: JOBS[identifier].update(state='complete',manifest=manifest)
    except Exception as error:
        with LOCK: JOBS[identifier].update(state='failed',error=str(error))
    finally:
        with LOCK: ACTIVE=None


def start_capture():
    global ACTIVE
    with LOCK:
        if ACTIVE is not None:
            return None
        identifier = uuid.uuid4().hex
        ACTIVE = identifier
        JOBS[identifier] = {'id':identifier,'state':'queued','kind':'existing_station_stream'}
        # Retain a bounded in-memory history; saved artifacts remain on disk.
        for key in list(JOBS)[:-20]:
            if key != identifier: JOBS.pop(key,None)
    threading.Thread(target=capture_worker,args=(identifier,),daemon=True).start()
    return identifier


def radio_status():
    return live_status(ROOT)


def status():
    reports = []
    active_probes = []
    for path in sorted(ROOT.glob('ir-tx*.json'), key=lambda p:p.stat().st_mtime, reverse=True):
        if path.name.endswith('.recovery.json'):
            continue
        try:
            data = json.loads(path.read_text())
            if not data.get('active_ir_tx_probe'):
                continue
            keys = ('tx_diagnostics','slots_granted','session_closed_verified',
                    'ram_restored_verified','mpu_restored','flash_written')
            active_probes.append({'source':path.name,
                'age_seconds':time.time()-path.stat().st_mtime,
                'evidence_kind':'Completed TX probe, not proof of command delivery',
                'report':{key:data.get(key) for key in keys}})
            if len(active_probes) == 3:
                break
        except (OSError, ValueError):
            continue
    for path in sorted(ROOT.glob('*.summary.json'), key=lambda p:p.stat().st_mtime, reverse=True)[:5]:
        try:
            reports.append({'name':path.name,'mtime':path.stat().st_mtime,
                            'age_seconds':time.time()-path.stat().st_mtime,
                            'evidence_kind':'Completed capture; not live device presence',
                            'report':json.loads(path.read_text())})
        except (OSError, ValueError):
            reports.append({'name':path.name,'error':'Report incomplete or unreadable'})
    scan = ROOT/'tracker-camera101-scan0.json'
    camera = None
    if scan.exists():
        data = json.loads(scan.read_text())
        counts = [len(r['blobs']) for r in data['frames']]
        camera = {'source':scan.name,'frames':len(counts),'min_blobs':min(counts,default=0),
                  'max_blobs':max(counts,default=0),'identity_verified':False}
    return {'utc':datetime.now(timezone.utc).isoformat(),'uptime_seconds':time.monotonic()-START,
            'radio_service':radio_status(),'application':AppControl(ROOT).status(radio_status()),'mode':'local_frame_tracker_api','recent_radio_runs':reports,'recent_active_probes':active_probes,'camera_scan':camera,
            'active_capture':ACTIVE,'datasets':dataset_inventory(),
            'live_camera_available':False,'tracker_poses_available':False}


class Handler(BaseHTTPRequestHandler):
    def json_response(self, value, code=200):
        body=json.dumps(value).encode()
        self.send_response(code)
        self.send_header('Content-Type','application/json')
        self.send_header('Content-Length',str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_POST(self):
        # Local SSH tunnel only; reject browser-origin requests and non-empty bodies.
        origin = self.headers.get('Origin')
        host = self.headers.get('Host','')
        local_host = re.fullmatch(r'(127\.0\.0\.1|localhost):[0-9]+',host)
        app_route=self.path in ('/profiles/game','/profiles/service') or re.fullmatch(r'/trackers/[0-9a-f]{8}/update',self.path)
        if (self.path not in ('/capture','/radio/start','/radio/stop') and not app_route) or not local_host or (origin and origin != 'http://'+host) or self.headers.get('Transfer-Encoding'):
            self.send_error(403); return
        if self.headers.get('Content-Length','0') != '0':
            self.send_error(400); return
        if app_route:
            if ACTIVE or not RADIO_LOCK.acquire(blocking=False):
                self.json_response({'error':'Radio operation or capture already in progress'},409);return
            try:
                control=AppControl(ROOT)
                value=control.select(self.path.rsplit('/',1)[1]) if self.path.startswith('/profiles/') else control.update(self.path.split('/')[2],radio_status())
                self.json_response(value,202)
            except (ValueError,RuntimeError,OSError,subprocess.TimeoutExpired) as error:
                self.json_response({'error':str(error)},409)
            finally:RADIO_LOCK.release()
            return
        if self.path.startswith('/radio/'):
            if ACTIVE:
                self.json_response({'error':'Wait for capture cleanup'},409);return
            action=self.path.rsplit('/',1)[1]
            if not RADIO_LOCK.acquire(blocking=False):
                self.json_response({'error':'Radio operation already in progress'},409);return
            try:
                control=AppControl(ROOT)
                value=control.select('game') if action=='start' else control.stop()
                self.json_response(value,202)
            except (ValueError,RuntimeError,OSError) as error:
                self.json_response({'error':str(error)},409)
            except subprocess.TimeoutExpired:
                self.json_response({'error':'Service operation timed out; check status before retrying'},504)
            finally: RADIO_LOCK.release()
            return
        if not radio_status().get('running'):
            self.json_response({'error':'Station is not running; diagnostics do not start RF probes'},409);return
        identifier = start_capture()
        self.json_response({'id':identifier} if identifier else {'error':'Capture already running'},
                           202 if identifier else 409)

    def do_GET(self):
        path = urlparse(self.path).path
        if path == '/':
            self.send_response(200)
            self.send_header('Content-Type','text/html; charset=utf-8')
            self.send_header('Content-Length',str(len(PAGE)))
            self.end_headers(); self.wfile.write(PAGE); return
        dataset_match = re.fullmatch(r'/dataset/([A-Za-z0-9_.-]+\.dataset)/([A-Za-z0-9_.-]+)',path)
        if dataset_match:
            directory,filename = dataset_match.groups()
            root = DATASETS.resolve()
            file = (DATASETS/directory/filename).resolve()
            if root not in file.parents or not file.is_file(): self.send_error(404); return
            # Serve only dataset files, never arbitrary headset paths.
            size = file.stat().st_size
            self.send_response(200)
            self.send_header('Content-Type','application/octet-stream')
            self.send_header('Content-Length',str(size))
            self.end_headers()
            with file.open('rb') as source:
                remaining = size
                while remaining:
                    block = source.read(min(65536,remaining))
                    if not block: break
                    self.wfile.write(block); remaining -= len(block)
            return
        match = re.fullmatch(r'/capture/([0-9a-f]{32})(/bundle)?',path)
        if match:
            identifier,bundle = match.groups()
            with LOCK: job = dict(JOBS.get(identifier,{}))
            if not job: self.send_error(404); return
            if not bundle: self.json_response(job); return
            archive = ROOT/('api-'+identifier+'.tar.gz')
            if job['state'] not in ('complete','failed') or not archive.is_file():
                self.send_error(409); return
            self.send_response(200)
            self.send_header('Content-Type','application/gzip')
            self.send_header('Content-Length',str(archive.stat().st_size))
            self.end_headers()
            with archive.open('rb') as source:
                while block := source.read(65536): self.wfile.write(block)
            return
        if path != '/status': self.send_error(404); return
        try:
            body=json.dumps(status()).encode()
        except (OSError,ValueError) as error:
            self.send_error(503,str(error))
            return
        self.send_response(200)
        self.send_header('Content-Type','application/json')
        self.send_header('Content-Length',str(len(body)))
        self.end_headers()
        self.wfile.write(body)


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port',type=int,default=18741)
    args=parser.parse_args()
    ThreadingHTTPServer(('127.0.0.1',args.port),Handler).serve_forever()
