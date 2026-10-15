# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Local application policy. Runtime bundles must be explicitly admitted.

Discovery, mode proof and firmware identity are independent. Never infer stock
firmware from a timeout, or tracking from service activation.
"""
import json,time,subprocess
try:
    from .frame_profile_owner import ProfileOwner
except ImportError:
    from frame_profile_owner import ProfileOwner

class AppControl:
    def __init__(self,root,run=subprocess.run):self.root=root;self.run=run

    def catalog(self):
        try:return json.loads((self.root/'app-profiles.json').read_text())
        except (OSError,ValueError):return {}

    def trackers(self,state):
        fresh=state.get('running') is True and isinstance(state.get('updated_unix'),(int,float)) and 0<=time.time()-state['updated_unix']<2
        result=[]
        for peer in state.get('trackers',[]):
            if not fresh or peer.get('connected') is not True:continue
            mode=peer.get('radio_mode') if peer.get('radio_mode_verified') is True else None
            supported=peer.get('custom_mode_supported')
            # An explicit fresh capability answer is required for stock classification.
            stock=supported is False and peer.get('firmware_identity_verified') is True
            tracked=peer.get('pose_valid') is True and mode=='frame'
            result.append(dict(id_le=peer.get('id_le'),serial=peer.get('serial'),
                mode=1 if mode=='frame' else 0 if mode=='stock' else None,
                mode_verified=mode in ('frame','stock'),
                status='Tracked' if tracked else 'RF connected; awaiting verified 6DoF',
                pose_valid=tracked,
                update_required=stock,update_available=False,
                mode_status='Custom firmware update required' if stock else 'Checking mode' if mode is None else 'Game mode' if mode=='frame' else 'Service mode'))
        return result

    def status(self,state):
        profiles={}
        for key in ('game','service'):
            try:ProfileOwner(self.root,self.run).validate(key);profiles[key]={'available':True,'reason':''}
            except (ValueError,RuntimeError,OSError,TypeError,subprocess.TimeoutExpired) as error:profiles[key]={'available':False,'reason':str(error)}
        trackers=self.trackers(state)
        return dict(profiles=profiles,
            trackers=trackers,tracking_available=any(peer['pose_valid'] for peer in trackers),
            desired_game_mode=1,update_mode=0)

    def select(self,profile):
        return ProfileOwner(self.root,self.run).select(profile)
    def stop(self):
        return ProfileOwner(self.root,self.run).stop()
    def update(self,chip,state):
        peers=[p for p in self.trackers(state) if p['id_le']==chip]
        if len(peers)!=1 or not peers[0]['update_required']:raise ValueError('Fresh stock firmware capability proof required')
        # A sealed updater must bind target, image and mode0 preflight itself.
        raise ValueError('OTA application owner integration pending; no firmware transfer started')
