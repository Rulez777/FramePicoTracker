# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Phase-lock station slots into the quiet window of Roy's stock BLE schedule.

Hardware 2026-10-06: Roy keeps two BLE-2M controller connections on a 3000.0us
grid (RTC/LFCLK domain). Controller A is on air ~500..1200us, B ~2200..3100us;
both are quiet ~1300..2200us. A HIGH-priority timeslot that overlaps their
events froze controller poses for 200..400ms. MPSL timeslots and BLE anchors
share the LF clock, so once a slot sits in the quiet window it stays there; a
slow host loop corrects residual drift with one-shot distance adjustments.
"""
import struct
import numpy as np

RADIO=0x40001000;RTC1=0x40011000
RTC_HZ=32768;RTC_WRAP=1<<24
PERIOD_S=3000e-6
BINS=30

def quiet_width_us(slot_us,margin_us,lead_us,post_return_us=None):
    """Required quiet run; early return is experimental and opt-in."""
    if post_return_us is None:return slot_us+200
    if (not 0<margin_us<slot_us or lead_us<100 or post_return_us<100):
        raise ValueError('Early return needs a bounded deadline and >=100us quiet padding')
    return lead_us+(slot_us-margin_us)+post_return_us


def sample(debug):
    """One (rtc_tick, busy, base0) sample of the stock radio; read-only.

    All reads go in ONE OpenOCD command: separate host round trips took 5-8ms
    under station CPU load, longer than the 3ms BLE period, and smeared the
    fold flat. In one TCL list the reads are back-to-back on the target.
    """
    text=debug.command(f'list [read_memory {RTC1+0x504:#x} 32 1] [read_memory {RADIO+0x550:#x} 32 1] '
                       f'[read_memory {RADIO+0x51c:#x} 32 1] [read_memory {RTC1+0x504:#x} 32 1]')
    values=[int(v,0) for v in text.replace('{','').replace('}','').split()]
    if len(values)!=4:raise RuntimeError('Unexpected OpenOCD radio sample reply')
    before,state,base0,after=values
    middle=before+(((after-before)%RTC_WRAP)/2)
    return int(middle)%RTC_WRAP,state!=0,base0


def quiet_window(samples,*,period_s=PERIOD_S,min_samples=600,max_busy_fraction=None,bins=BINS):
    """Return (reference_tick, quiet_center_phase_s, width_s, profile) or raise.

    Phases are taken relative to the first sample's tick so the 24-bit RTC wrap
    (512s) never splits the fold.
    """
    if not isinstance(bins,int) or not 4<=bins<=120:raise ValueError('Invalid BLE histogram bin count')
    if len(samples)<min_samples:raise ValueError('Too few radio samples for BLE phase')
    ticks=np.array([s[0] for s in samples],dtype=float);busy=np.array([s[1] for s in samples])
    reference=ticks[0]
    elapsed=((ticks-reference)%RTC_WRAP)/RTC_HZ
    phase=(elapsed%period_s)/period_s
    total,_=np.histogram(phase,bins=bins,range=(0,1));hit,_=np.histogram(phase[busy],bins=bins,range=(0,1))
    if total.min()<5:raise ValueError('Radio samples do not cover the BLE period')
    profile=hit/total
    # Relative threshold: a third of the way from the quietest to the busiest bin.
    low,high=profile.min(),profile.max()
    quiet=profile<=low+.35*(high-low) if high-low>=.2 else np.zeros(BINS,dtype=bool)
    if max_busy_fraction is not None:
        if not 0<=max_busy_fraction<=1:raise ValueError('Invalid BLE busy fraction limit')
        quiet &= profile<=max_busy_fraction
    # Longest circular run of quiet bins.
    best=(0,0);run=0;start=0
    for i in range(2*bins):
        if quiet[i%bins]:
            if run==0:start=i
            run+=1
            if run>best[0]:best=(min(run,bins),start)
        else:run=0
    length,start=best
    if length/bins*period_s<400e-6-1e-12:raise ValueError(f'No quiet window in the BLE schedule: {profile.round(2).tolist()}')
    center=((start+length/2)%bins)/bins*period_s
    return int(reference),float(center),length/bins*period_s,profile.round(3).tolist()


class PhaseLock:
    """Host side of the slot phase loop. MCU stores RTC1 at each slot start;
    the host writes a one-shot signed distance adjustment (us) when off target."""
    def __init__(self,reference_tick,center_s,slot_s,*,width_s=None,lead_s=100e-6,deadband_s=60e-6,max_step_s=400e-6,period_s=PERIOD_S):
        self.reference=reference_tick;self.period=period_s;self.deadband=deadband_s;self.max_step=max_step_s
        # Slot start target: the slot [start,start+slot] centred in the window.
        # With a known width, start just after the previous event ends: the
        # next BLE event reserves radio time before it goes on air (hardware
        # 2026-10-06: centred 700us slots still froze a controller).
        start=center_s-slot_s/2 if width_s is None else center_s-width_s/2+lead_s
        self.target=start%period_s
        self.last_tick=None;self.errors=[];self.adjustments=0

    def target_tick(self):
        """24-bit RTC1 tick of one target slot start (MCU repeats it every period)."""
        return int(round(self.reference+self.target*RTC_HZ))%RTC_WRAP

    def error_s(self,slot_start_tick):
        elapsed=((slot_start_tick-self.reference)%RTC_WRAP)/RTC_HZ
        error=(elapsed-self.target)%self.period
        return error-self.period if error>self.period/2 else error

    def update(self,slot_start_tick,pending_adjust):
        """Return a signed adjustment in us to write, or None."""
        if slot_start_tick==self.last_tick or pending_adjust:return None
        self.last_tick=slot_start_tick
        error=self.error_s(slot_start_tick);self.errors=(self.errors+[error])[-20:]
        if abs(error)<=self.deadband:return None
        step=-max(-self.max_step,min(self.max_step,error))
        self.adjustments+=1
        return int(round(step*1e6)) or None

    def status(self):
        recent=self.errors[-10:]
        return {'target_phase_us':round(self.target*1e6),'last_error_us':None if not recent else round(recent[-1]*1e6),
                'max_recent_error_us':None if not recent else round(max(abs(e) for e in recent)*1e6),
                'adjustments':self.adjustments}
