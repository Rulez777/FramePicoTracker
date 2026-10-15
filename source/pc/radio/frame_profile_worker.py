# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Systemd child supervisor. Restores before allowing another radio owner.

The immutable profile bundle supplies a reviewed runtime command; this worker
does not replace its HMD/controller/mode/OTA admission checks.
"""
import argparse,json,os,re,signal,subprocess,time
import sys
from pathlib import Path
try:
    from .frame_profile_owner import ProfileOwner,BARRIERS
except ImportError:
    from frame_profile_owner import ProfileOwner,BARRIERS

def execute(root,profile,*,owner_factory=ProfileOwner):
    import fcntl
    root=Path(root).resolve()
    invocation=os.environ.get('INVOCATION_ID','')
    if not re.fullmatch('[0-9a-f]{32}',invocation):raise ValueError('Fresh systemd invocation required')
    with (root/'app-radio-owner.lock').open('a') as lock:
        fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
        owner=owner_factory(root)
        item=owner.validate(profile)
        saved=owner.saved_owner()
        if saved:owner.restored(saved['profile'],saved['invocation'])
        attempt=root/'app-attempts'/profile/invocation
        attempt.mkdir(parents=True,exist_ok=False)
        template=item.get('command')
        if not isinstance(template,list) or not template or any(not isinstance(arg,str) or '\0' in arg for arg in template):raise ValueError('Invalid runtime command')
        command=[arg.replace('{attempt}',str(attempt)) for arg in template]
        if not Path(command[0]).is_absolute() or command[0] not in item['files']:raise ValueError('Interpreter or executable not sealed')
        # All command/script runtime inputs must be listed by the reviewed
        # manifest. Caller cannot submit command overrides through the API.
        report_path=attempt/'radio-run.json'
        if command.count('--out')!=1 or command.index('--out')+1>=len(command) or Path(command[command.index('--out')+1]).resolve()!=report_path:raise ValueError('Per-invocation MCU report required')
        if command.count('--live-state')!=1 or command.index('--live-state')+1>=len(command) or Path(command[command.index('--live-state')+1]).resolve()!=attempt/'radio-live.json':raise ValueError('Per-invocation live stream required')
        if '--run' not in command:raise ValueError('Expected reviewed radio runtime')
        if any('ota' in arg.lower() for arg in command):raise ValueError('Profile selection cannot start firmware transfer')
        saved_path=root/'app-owner-last.json'
        temporary=saved_path.with_suffix('.new')
        with temporary.open('w') as output:
            json.dump({'profile':profile,'invocation':invocation},output);output.flush();os.fsync(output.fileno())
        temporary.replace(saved_path)
        cancelled=False
        def stop(signum,frame):
            nonlocal cancelled
            cancelled=True
        previous={s:signal.signal(s,stop) for s in (signal.SIGINT,signal.SIGTERM)}
        child=None
        try:
            if cancelled:raise RuntimeError('Cancelled before radio start')
            with (attempt/'stdout.log').open('x') as log:
                parent_guard=None
                if sys.platform=='linux':
                    import ctypes
                    libc=ctypes.CDLL(None,use_errno=True)
                    parent_pid=os.getpid()
                    def parent_guard():
                        # Parent death requests the loader's normal restoration;
                        # never kill an SRAM owner. Check the fork/prctl race too.
                        if libc.prctl(1,int(signal.SIGINT),0,0,0)!=0 or os.getppid()!=parent_pid:os._exit(125)
                child=subprocess.Popen(command,cwd=item['working_directory'],stdout=log,stderr=subprocess.STDOUT,preexec_fn=parent_guard)
                (attempt/'child.json').write_text(json.dumps({'pid':child.pid,'owner_invocation_id':invocation,'command':command}))
                while child.poll() is None and not cancelled:time.sleep(.05)
                if child.poll() is None:
                    child.send_signal(signal.SIGINT)
                    # No SIGKILL of a process owning SRAM. Stay the exclusive
                    # owner while restoration is unresolved.
                    while child.poll() is None:time.sleep(.05)
                report=json.loads(report_path.read_text())
                if not all(report.get(k) is True for k in BARRIERS):raise RuntimeError('Actual MCU cleanup incomplete')
                folder=root/'app-cleanup'/profile;folder.mkdir(parents=True,exist_ok=True)
                final={k:report[k] for k in BARRIERS}
                final.update(owner_invocation_id=invocation,profile=profile,child_pid=child.pid,child_exit_code=child.returncode,report_path=str(report_path))
                with (folder/(invocation+'.json')).open('x') as output:json.dump(final,output)
                return child.returncode
        finally:
            # Unexpected supervisor failure also retains ownership until the
            # child exits. Its absence of a final report blocks future profiles.
            if child is not None and child.poll() is None:
                child.send_signal(signal.SIGINT)
                while child.poll() is None:time.sleep(.05)
            for sig,handler in previous.items():signal.signal(sig,handler)

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root',required=True)
    parser.add_argument('--profile',choices=('game','service'),required=True)
    args=parser.parse_args()
    raise SystemExit(execute(args.root,args.profile))
