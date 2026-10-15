# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Read only the current owner's live stream, never a previous attempt."""
import json,time,re
from pathlib import Path
try:
    from .frame_profile_owner import ProfileOwner,UNITS
except ImportError:
    from frame_profile_owner import ProfileOwner,UNITS

def current_stream(root,*,owner=None):
    root=Path(root)
    owner=owner or ProfileOwner(root)
    saved=owner.saved_owner()
    states={profile:owner.unit(unit) for profile,unit in UNITS.items()}
    active=[profile for profile,state in states.items() if state.get('ActiveState')=='active']
    if any(state.get('ActiveState') not in ('active','inactive','failed') for state in states.values()):
        raise RuntimeError('Radio profile is transitioning')
    if len(active)>1:raise RuntimeError('Multiple radio owners active')
    if saved or active:
        if not saved or active!=[saved['profile']]:raise RuntimeError('Current radio owner is inactive or unbound')
        state=states[saved['profile']]
        if state.get('InvocationID')!=saved['invocation'] or not re.fullmatch('[1-9][0-9]*',state.get('MainPID','')):
            raise RuntimeError('Current radio invocation does not match its owner')
        attempt=root/'app-attempts'/saved['profile']/saved['invocation']
        return attempt/'radio-live.json',attempt/'radio-live-records.jsonl'
    legacy=owner.unit('frame-pico-radio.service')
    if legacy.get('ActiveState')!='active':raise RuntimeError('Radio service is inactive')
    return root/'radio-live.json',root/'radio-live-records.jsonl'

def live_status(root,*,owner=None,now=time.time):
    try:
        path,_=current_stream(root,owner=owner)
        state=json.loads(path.read_text())
        stamp=state.get('updated_unix')
        if (state.get('running') is not True or type(stamp) not in (int,float)
                or not 0<=now()-stamp<2):raise RuntimeError('Radio stream is stale or stopped')
        return state
    except (OSError,ValueError,KeyError,TypeError,RuntimeError) as error:
        return {'running':False,'trackers':[],'poses_valid':False,'error':str(error)}
