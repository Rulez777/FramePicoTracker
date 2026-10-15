# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Exclusive systemd profile transitions with sealed units and actual cleanup.

No discovery/OTA inference. Each installed unit must emit its systemd
INVOCATION_ID in its final MCU restoration report.
"""
import hashlib,json,re,subprocess,threading,os
from pathlib import Path

BARRIERS=('session_closed_verified','ram_restored_verified','mpu_restored','trace_controls_restored_verified')
UNITS={'game':'frame-pico-game.service','service':'frame-pico-service.service'}
MODULE_SHA256={'game':'d8328717a5f6b757fd002ad6622e4fa5f07c45a1de935f6631267792deb286a3',
              'service':'44f4e52b07bbba4c459256c910228d440e25ff0d288238e76047fd21000c4c7c'}

class ProfileOwner:
    def __init__(self,root,run=subprocess.run):
        self.root=Path(root);self.run=run;self.lock=threading.Lock()

    def command(self,*args):
        result=self.run(['systemctl','--user',*args],capture_output=True,text=True,timeout=30)
        if result.returncode:raise RuntimeError(result.stderr.strip() or 'Radio service operation failed')
        return result.stdout

    def unit(self,name):
        raw=self.command('show',name,'--property=ActiveState,InvocationID,MainPID,FragmentPath')
        return dict(line.split('=',1) for line in raw.splitlines() if '=' in line)

    def validate(self,profile):
        if profile not in UNITS:raise ValueError('Unknown radio profile')
        try:manifest=json.loads((self.root/'app-profiles.json').read_text())
        except (OSError,ValueError) as error:raise ValueError('Installed profile catalog unavailable') from error
        item=manifest.get(profile,{})
        if item.get('admitted') is not True:raise ValueError(item.get('reason','Profile not admitted'))
        if item.get('mode')!=(1 if profile=='game' else 0):raise ValueError('Wrong applied-mode requirement')
        if item.get('unit')!=UNITS[profile]:raise ValueError('Unexpected radio unit')
        files=item.get('files')
        if not isinstance(files,dict) or not files:raise ValueError('Missing artifact seals')
        for path,sha in files.items():
            if not Path(path).is_absolute() or not re.fullmatch('[0-9a-f]{64}',sha):raise ValueError('Invalid artifact seal')
            if hashlib.sha256(Path(path).read_bytes()).hexdigest()!=sha:raise ValueError('Radio artifact seal mismatch: '+path)
        unit=self.unit(UNITS[profile])
        if unit.get('FragmentPath') not in files:raise ValueError('Installed radio unit is not sealed')
        # The service unit must run a reviewed owner which implements current
        # HMD/controller/target admission, leased mode proof and restoration.
        if item.get('owner_contract')!='fresh-admission-mode-lease-cleanup-v1':raise ValueError('Missing runtime owner contract')
        config_path=item.get('station_config')
        if config_path not in files:raise ValueError('Station mode configuration is not sealed')
        config=json.loads(Path(config_path).read_text())
        if config.get('tracker_desired_mode')!=('frame' if profile=='game' else 'stock'):raise ValueError('Runtime does not maintain requested tracker mode')
        if not config.get('tracker_dual_mode_ids') or config.get('tracker_mode_query_schema')!=2:raise ValueError('Fresh applied-mode queries required')
        if profile=='service' and config.get('optics_enabled') is not False:raise ValueError('Service mode cannot publish optical poses')
        command=item.get('command')
        if not isinstance(command,list) or not command or any(not isinstance(arg,str) for arg in command):raise ValueError('Missing reviewed runtime command')
        if command[0] not in files:raise ValueError('Runtime executable is not sealed')
        for argument in command[1:]:
            if argument.endswith('.py') and argument not in files:raise ValueError('Runtime entry script is not sealed')
        if '--tracking-watch' in command:
            if command.count('--tracking-watch')!=1 or command.index('--tracking-watch')+1>=len(command):raise ValueError('Invalid tracking watcher argument')
            watcher=command[command.index('--tracking-watch')+1]
            if watcher not in files or not os.access(watcher,os.X_OK):raise ValueError('Runtime tracking watcher is not executable or sealed')
        if command.count('--live-state')!=1 or command.index('--live-state')+1>=len(command) or command[command.index('--live-state')+1]!='{attempt}/radio-live.json':raise ValueError('Current owner live stream required')
        if command.count('--station-config')!=1 or command[command.index('--station-config')+1]!=config_path:raise ValueError('Runtime command uses a different mode configuration')
        if command.count('--module')!=1 or command.index('--module')+1>=len(command):raise ValueError('Explicit radio module required')
        module=command[command.index('--module')+1]
        if files.get(module)!=MODULE_SHA256[profile]:raise ValueError('Radio module differs from retained game/service profile')
        return item

    def saved_owner(self):
        path=self.root/'app-owner-last.json'
        if not path.exists():return None
        value=json.loads(path.read_text())
        if value.get('profile') not in UNITS or not re.fullmatch('[0-9a-f]{32}',value.get('invocation','')):raise RuntimeError('Saved radio ownership malformed')
        return value

    def restored(self,profile,invocation):
        if not re.fullmatch('[0-9a-f]{32}',invocation or ''):raise RuntimeError('Previous owner invocation unknown')
        report=json.loads((self.root/'app-cleanup'/profile/(invocation+'.json')).read_text())
        if report.get('owner_invocation_id')!=invocation or report.get('profile')!=profile or not all(report.get(k) is True for k in BARRIERS):
            raise RuntimeError('Previous owner MCU restoration not verified')

    def select(self,profile):
        if not self.lock.acquire(blocking=False):raise RuntimeError('Profile transition in progress')
        try:
            self.validate(profile) # Complete admission BEFORE stopping another owner.
            legacy=self.unit('frame-pico-radio.service')
            if legacy.get('ActiveState') not in ('inactive','failed'):raise RuntimeError('Legacy radio owner must be closed and restored first')
            states={current:self.unit(name) for current,name in UNITS.items()}
            if sum(state.get('ActiveState')=='active' for state in states.values())>1:raise RuntimeError('Multiple radio owners active; restoration required')
            saved=self.saved_owner()
            if saved and not (states[saved['profile']].get('ActiveState')=='active' and states[saved['profile']].get('InvocationID')==saved['invocation']):
                self.restored(saved['profile'],saved['invocation'])
            for current,name in UNITS.items():
                state=states[current]
                if state.get('ActiveState') not in ('inactive','failed','active'):raise RuntimeError('Radio owner still transitioning')
                if current==profile and state.get('ActiveState')=='active':
                    return {'requested_profile':profile,'state':'already_running','tracking_verified':False}
                invocation=state.get('InvocationID')
                if state.get('ActiveState')=='active':
                    self.command('stop',name)
                    stopped=self.unit(name)
                    if stopped.get('ActiveState')!='inactive' or stopped.get('MainPID')!='0':raise RuntimeError('Previous radio process still owns hardware')
                    self.restored(current,invocation)
                elif invocation:self.restored(current,invocation)
            self.validate(profile) # Recheck seals after cleanup.
            self.command('start',UNITS[profile])
            return {'requested_profile':profile,'state':'start_requested','desired_mode':1 if profile=='game' else 0,'tracking_verified':False}
        finally:self.lock.release()

    def stop(self):
        if not self.lock.acquire(blocking=False):raise RuntimeError('Profile transition in progress')
        try:
            for profile,name in UNITS.items():
                state=self.unit(name)
                if state.get('ActiveState') in ('active','activating','deactivating'):
                    invocation=state.get('InvocationID')
                    self.command('stop',name)
                    after=self.unit(name)
                    if after.get('ActiveState')!='inactive' or after.get('MainPID')!='0':raise RuntimeError('Radio owner remains alive')
                    self.restored(profile,invocation)
            saved=self.saved_owner()
            if saved:self.restored(saved['profile'],saved['invocation'])
            legacy=self.unit('frame-pico-radio.service')
            if legacy.get('ActiveState') not in ('inactive','failed'):
                self.command('stop','frame-pico-radio.service')
                raise RuntimeError('Legacy radio stop requested; its restoration requires separate verification')
            return {'state':'stopped','owned_profiles_restored':True,'tracking_verified':False}
        finally:self.lock.release()
