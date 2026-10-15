# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Closed-session phase acquisition for the sealed short-only RAM candidate.

The caller owns the loader's checkpoint and final restoration. This helper
neither closes/opens a session nor changes CODE, peer state or mode proof.
"""
import hashlib,struct,time
try:
    from .roy_ram_loader import DATA,CODE,MPSL,SHELL_HOOK,session_is_active,invoke_native
except ImportError:
    from roy_ram_loader import DATA,CODE,MPSL,SHELL_HOOK,session_is_active,invoke_native

CODE_SHA='a498034510958d7544aefa35887ccb526991c33fc6b89a0e32adc2441807932b'
CODE_BYTES=1773
ENTRY=0x2001151d

def require_closed(debug):
    if (session_is_active(debug.read(MPSL,48)) or debug.read(DATA+84,1)!=b'\xff'
            or debug.read(SHELL_HOOK,4)!=bytes(4)):
        raise RuntimeError('Closed MPSL session and released native hook required')
    write,read=struct.unpack('<II',debug.read(DATA+112,8))
    if write!=read:raise RuntimeError('Drain and preserve RX records before phase scratch reuse')

def common_plan(rows):
    accepted=[r for r in rows if r[0]==r[2]]
    if len(accepted)<1000:raise ValueError('Insufficient zero-bracket phase observations')
    ref=accepted[0][0];hist=[]
    for part in range(6):
        total=[0]*60;busy=[0]*60
        for before,state,after,base in accepted[len(accepted)*part//6:len(accepted)*(part+1)//6]:
            phase=(((before-ref)&0xffffff)*1e6/32768)%3000
            i=min(59,int(phase/50));total[i]+=1;busy[i]+=int(state!=0)
        hist.append({'total':total,'busy':busy})
    quiet=[all(h['total'][i]>=3 and h['busy'][i]/h['total'][i]<=.05 for h in hist) for i in range(60)]
    best=run=start=beststart=0
    for i in range(120):
        if quiet[i%60]:
            if not run:start=i
            run+=1
            if min(run,60)>best:best=min(run,60);beststart=start
        else:run=0
    width=best*50;budget=width-2e6/32768-200
    if budget<330:raise ValueError(f'Fresh phase RX budget {budget:.3f}us does not admit330us')
    # Split the RTC allowance across both sides, then retain100us lead.
    target=(beststart%60*50+1e6/32768+100)%3000
    return {'reference_tick':ref,'target_tick':int(round(ref+target*32768/1e6))&0xffffff,
            'target_phase_us':target,'raw_width_us':width,'rx_budget_us':budget,
            'accepted':len(accepted),'raw':len(rows),'histograms':hist,
            'scope':'Offline STATE admission only; no guaranteed exclusive reservation'}

def collect_closed(debug,guard,*,seconds=6,budget_check=lambda:None):
    require_closed(debug)
    if hashlib.sha256(debug.read(CODE,CODE_BYTES)).hexdigest()!=CODE_SHA:
        raise RuntimeError('Exact phase90d CODE required')
    peers=debug.read(DATA+1504,96);mailboxes=debug.read(DATA+1664,192)
    rows=[];calls=[];until=time.monotonic()+seconds
    while time.monotonic()<until:
        budget_check();guard.check(require_now=True);require_closed(debug)
        # Clear completion marker; never rewrite a peer's last-reply time.
        debug.write(DATA+1600,bytes(4))
        invoke_native(debug,ENTRY,3,disable_mpu=True,reserved_mpu=True)
        deadline=time.monotonic()+.3
        while debug.read(DATA+1600,4)!=struct.pack('<I',0x52455431) or debug.read(SHELL_HOOK,4)!=bytes(4):
            if time.monotonic()>=deadline:raise RuntimeError('Observer did not release native ownership; retain checkpoint')
            guard.check();time.sleep(.005)
        require_closed(debug)
        n=struct.unpack('<I',debug.read(DATA+1900,4))[0]
        if not 1<=n<=64:raise RuntimeError('Invalid or refused closed-session observer')
        block=debug.read(DATA+256,n*16)
        rows.extend(struct.unpack_from('<4I',block,i*16) for i in range(n));calls.append(n)
        if debug.read(DATA+1504,96)!=peers or debug.read(DATA+1664,192)!=mailboxes:
            raise RuntimeError('Observer changed peer or mailbox state')
    budget_check();guard.check(require_now=True)
    return {'rows':rows,'calls':calls,'plan':common_plan(rows)}
