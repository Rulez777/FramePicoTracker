# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K04961
Source `D0172`, original line167, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 5. Minor: abort before begin leaves `waiting_target`

## K04962
Source `D0172`, original line171, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Lines 86–87 create a result with state `waiting_target` even before the session. Lines 122–124: if within 120 s the target has not reached `WAIT_SYNC` or `STREAMING`, `poll` throws `RuntimeError` and does not overwrite result. `_write_status` with `self.ota is None` returns immediately. Repeating the same job is forbidden because result already exists.

## K04963
Source `D0172`, original line173, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Minimal test: deadline expiration without peer; result must receive error state; retry with the same path must be rejected explicitly.

## K04964
Source `D0172`, original line175, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Radio

## K04965
Source `D0172`, original line177, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The claimed effect "these 30 bytes put responses on Valve's 3000 µs grid" does not follow from the bytes.

## K04966
Source `D0172`, original line179, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Even ideal 3000 µs on this RTC would be 98.304 ticks. The path `0x641c` can only handle integer ticks after division by 30.5. The candidate at N=13 programs 1338 ticks instead of stable 1333. Autonomous TX when N≠0 becomes 393 ticks, not 392. The 10980 µs branch is not moved by the patch. Therefore, matching the period with the 42 ms polling is not an RX phase.

## K04967
Source `D0172`, original line181, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A separate counterexample of the original document remains valid as a scheme, already after quantization: station slots of 0/6/12 ms within a cycle of 42 ms and autonomous TX about 12 ms from its own poll can coincide with the poll of another peer. This is a calculation based on the code of two TX branches `0x61f4` and `0x62e4`, not an over-the-air measurement. Both branches when N≠0 write a large constant into TIMER0 CC `0x540`.

## K04968
Source `D0172`, original line183, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The airtime estimate `38+4*L` at 2 Mbps is marked in the document as provisional. This analysis did not re-measure it. On nRF52833, the value MODE=4 in the specification is BLE LR 125 kbps, whereas working Roy captures were with MODE=4 and an observed end of response around 227..528 µs. For long IMU responses, this meshes poorly with 125 kbps. The contradiction between the register and the measurement is not resolved here.

## K04969
Source `D0172`, original line185, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Activation and recovery

## K04970
Source `D0172`, original line187, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

There is no analyzed bootloader in the materials. Finish ACK confirms the comparison of staging CRC with the byte from start, followed by the RAM flag. Metadata in `0x2f000` and reset are written by a separate `0xfa98`. The presence of the stock file and offline `reverse()` is not a method to recover the device from a bad boot. Activation/recovery verdict: insufficient data. This matches the original document's own boundary.

## K04971
Source `D0172`, original line189, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The unit of argument `10000` (`0x2710`) in `0x7294` and `0xfcc2` has not been determined. Both calls enter `0x13244` via `0x7140` / `0xf7a8`, not `0x641c`. Comparison with host timeout of 10 s therefore remains an assumption. During OTA, host writes value `40*64000000` (`pc/radio/ram/roy_ram_loader.py`) into `DATA+1612`, which is 40 s of DWT silence before stage reset. This number by itself is not tied to the `10000` unit on the tracker.

## K04972
Source `D0172`, original line191, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What is missing for the next conclusion

## K04973
Source `D0172`, original line193, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Live byte `0x20000f02`: RTC path with divider 30.5 or tail of `0x3ae0`.
- Live byte `0x20000f69` and its writers: fixed 10980 µs or formula with N.
- Bootloader: reading `0x2f000`, image verification, power loss, application launch condition.
- Time unit of `0x13244`, where constant 10000 goes in OTA.
- Complete semantics of queue `0x9368`: whether an unremoved ACK survives until the next session.

## K04974
Source `D0172`, original line199, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

79 tests from saved JUnit were not rerun. Their boundary (comparison of `r0` before `0x641c`) does not cover RTC ticks.

## K04975
Source `D0173`, original line1, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Verification of Gemini report on FW189 GRID3000

## K04976
Source `D0173`, original line3, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-07. User report:
`docs/FW189_GRID3000_INDEPENDENT_REVIEW_REPORT.md`.
SHA256 of verified report:
`728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`.

## K04977
Source `D0173`, original line8, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Conclusion:** the report contains useful remarks, but it cannot be accepted verbatim.
Binary patch encoding is confirmed; D-1 is reproduced. The verdict
"OTA sender is correct" is too strong, while "radio coexistence is disproven"
does not follow from the presented schedule without proof of phase, gating, and airtime.
Hardware launch is still premature: activation/recovery have not been verified.

## K04978
Source `D0173`, original line14, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Verification was performed offline. SSH/SWD/RF/OTA were not launched. Firmware binaries
and production Python/C sources were not modified; original Gemini report preserved.

## K04979
Source `D0173`, original line17, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. Adjusted verdicts

## K04980
Source `D0173`, original line19, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Subject | Verdict of this review |
|---|---|
| Exact binary edits, encoding, length, reverse | Confirmed in narrow byte scope |
| Completeness/physical correctness of new timer constants | Insufficient data on full event schedule |
| OTA wire format/SPI layout/CRC | Confirmed in previous offline scope; this is not a full sender audit |
| OTA sender/live integration | Not approved: provenance/epoch and failure persistence require fix/extended review |
| RF coexistence | Not proven; Gemini's specific "inevitable" refutation is not proven either |
| Hardware boot/activation/recovery | Insufficient data; current candidate is not ready for launch |

## K04981
Source `D0173`, original line28, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Actually changed 30 bytes at 10 sites; manifest reverse matched stock.
Candidate SHA remains
`6a82a89206a609fdce92a3de764882ce53f79f0360b925bebe1ce7853cde6a0d`.

## K04982
Source `D0173`, original line32, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. Verification of report findings

## K04983
Source `D0173`, original line34, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### B-1. Autonomous TX collisions — risk is valid, inevitability is not proven

## K04984
Source `D0173`, original line36, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Confirmed: patch indeed sets CC0=12000 when N≠0 in two branches of
TIMER0; autonomous TX is not eliminated by this patch itself. For 3 peers, the risk of
collision with others' polls must be modeled.

## K04985
Source `D0173`, original line40, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

However, an inevitable collision cannot be derived from `12000 mod 6000 = 0`. Real
anchors of both radio events and their durations are required. In the report, station slot
start, RADIO ADDRESS, and autonomous TIMER0 origin are equated without proof.

## K04986
Source `D0173`, original line46, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- VA0x5310..0x532c responds to RADIO ADDRESS in RX state and calls 0x62e4.
- Gates are present in 0x62e4: connection flags, byte0x20000f6a, and OTA state
  0x20002240. Not every N≠0 turns on the timer by itself.
- In 0x6148, the PPI endpoint receives RADIO ADDRESS0x40001104, and target is obtained by
  arithmetic `0x40001104+0x6f00=0x40008004`, which is the TIMER0 STOP task.
  This is a significant part of the control flow that is absent in Gemini's simple schedule.
- After compare, path 0x61f4 can transition to 0x5e9c; there RX is disabled,
  payload is formed, and TX begins if the length is non-zero. Empty queue and
  flags affect the actual appearance of the packet on the air.

## K04987
Source `D0173`, original line56, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The gate branch was checked in ARM emulation: N13, byte0x20000f6a=1,
CC0 previously 12345. Call 0x62e4 left CC0=12345 — did not set 12000.
This does not prove the absence of TX in the normal connected state, but shows
that the model "N13 always means TX every 12 ms" is incomplete.

## K04988
Source `D0173`, original line61, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A separate logical error of the report: empty PICO slots of 18/24/30/36 ms are not
entire intervals reserved for Valve. BLE events occur on their own
grid of 3 ms and inside each such interval. Falling into a truly quiet
phase **does not mean** a direct intersection with a controller event; the impact must be measured
or derived from a complete reservation/airtime model.

## K04989
Source `D0173`, original line67, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Correction:** "significant risk of autonomous TX collisions; patch has not proven
a coexistence solution." Not "fact of inevitable interference with controllers."

## K04990
Source `D0173`, original line70, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Gemini's recommendation of NOP CC0 store is not approved:** write in 0x6222/0x6318
selects the value for both N cases. Without proper conditional control flow,
NOP can remove the needed N0 branch as well, leave stale CC0, interrupts, and timer
start/SHORTS. An audit of the entire 0x61f4/0x62e4/PPI lifecycle is needed, rather than a single write.

## K04991
Source `D0173`, original line75, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### B-2. Budget of 400 µs is a confirmed issue, timestamps interpreted imprecisely

## K04992
Source `D0173`, original line77, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Indeed `D(1892)=600−200=400`. The log contains observations of 227–528 µs
and controller degradation when RF allocation increases. Therefore, patch periods
does not prove that the reply fits into the budget.

## K04993
Source `D0173`, original line81, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

However, the recorded time is a **software callback observation**, not a hardware
RADIO END timestamp. `roy_ram_station_v4.c:64` calls TIMER capture inside
`capture()`, and the loader explicitly marks this as callback time, not RF-arrival time.
From observation of 528 µs, one cannot strictly deduce hardware END of 528 µs without a separate
tag and upper bound callback latency. And previous measurements are not a measurement
of the new firmware candidate.

## K04994
Source `D0173`, original line88, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

An additional check of the current source revealed a separate gap:

## K04995
Source `D0173`, original line95, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The first RX compare is not clamped to D(1892). If TX ends at 350 µs,
the first check is scheduled for 450 µs, although boundary is 400. In callback, capture
is possible before compare tick with boundary (`:172`, then `:178`). This is a conditional
source-level counterexample to the claim of a hard guaranteed 400 µs
return, rather than a new hardware result. Subsequent ticks are clamped (`:179`),
the first is not. Real MPSL preemption/latency still needs to be taken into account.

## K04996
Source `D0173`, original line102, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Correction:** budget is an unresolved blocker for coexistence readiness.
Actual END/return timings and elimination of the first overdue compare are needed; measured 528 µs
cannot be declared as the exact RF duration without provenance.

## K04997
Source `D0173`, original line106, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### B-3. N9 instead of N13 — configuration risk confirmed, "complete loss" not proven

## K04998
Source `D0173`, original line108, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`station_v4_runtime.py:167..176` uses per-ID override, and if absent,
takes common N. FW version 189 is intentionally not changed. Therefore, an incorrect map
can indeed assign N9 to patched tracker.

## K04999
Source `D0173`, original line112, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

But `int(9*3000+1810.5)=28810` is an argument of a single timer branch, not a proven
absolute opening from poll. Closing strictly at 30 ms and the absolute impossibility
of a response at 42 ms are not derived from the full scheduler. Native miss/search/reconnect
branches are not considered; the previous model of such a window already did not match the hardware.

## K05000
Source `D0173`, original line117, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A software marker in firmware is not the only possible mechanism. An explicit
per-ID map already exists. A proper transaction state update after verified
boot, a rollback/reconnect workflow, and the absence of an automatic assumption
for version189 are needed. Adding a new version may, in turn, require
changes to the host whitelist/IMU decoder; do not impose it without a specific reason.

## K05001
Source `D0173`, original line123, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Correction:** wrong-source-cadence/config ordering is a real problem for
the next integration. Complete loss of communication based on a single formula has not been established.

## K05002
Source `D0173`, original line126, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### D-1. waiting_target after pre-OTA error — confirmed by reproducer

## K05003
Source `D0173`, original line128, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Reproduced with fake Debug and temporary registry/job:

## K05004
Source `D0173`, original line130, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Constructor creates result=`waiting_target`.
2. connection_deadline expires; `poll()` throws
   `OTA target configuration deadline expired`.
3. Standard `publish(...,False)` is called.
4. result is still `waiting_target`, without an error cause.
5. `load_job()` fails due to the existing result.

## K05005
Source `D0173`, original line137, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Fake SWD writes in this check: 0. This is confirmation of a host bug without a device.
Prohibiting replay is useful in itself; the error is that the terminal failure is not saved
and there is no clear workflow for creating a new job after state verification.

## K05006
Source `D0173`, original line141, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Accept the comment.** The fix must cover timeout, unsupported product,
guard/loader setup failure, and cancellation before creating OtaSession, while preserving the prohibition
of uncontrolled repeated OTA.

## K05007
Source `D0173`, original line145, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### D-2. Synchronous I/O — structural risk, claimed latency not confirmed

## K05008
Source `D0173`, original line147, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The host indeed writes status synchronously, and not just once per
block: `_write_status()` is called in active `poll()` and `publish()` as well.
Events are written on transitions. The loader updates threshold=`slots+1000`
after runtime poll; 1000×6 ms ≈ 6 s, without taking blocked slots into account.

## K05009
Source `D0173`, original line152, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The presence of synchronous I/O is confirmed, but the specific latency > 1 s and eMMC type
are not measured in this report. A stall of 1 s by itself does not exhaust the 6 s budget; the total
delay must be compared with remaining watchdog slots and fresh clock evidence.

## K05010
Source `D0173`, original line156, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Batch/rate-limit/asynchronous writing of progress is reasonable, but terminal errors and
finish evidence must be saved separately. The simple advice to "write every 100
blocks" does not replace lifecycle/durability design.

## K05011
Source `D0173`, original line160, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### C-1. Activation/recovery — limitation confirmed, unbrickable not proven

## K05012
Source `D0173`, original line162, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The necessary analysis of the tracker bootloader is not here. Hardware boot, signature
policy, power-loss behavior, and rescue indeed remain unknown.
In this state, experimental OTA hardware readiness cannot be approved.

## K05013
Source `D0173`, original line166, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

However, the absence of USB does not prove unrecoverability. The debug/rescue mechanism
is also unknown, rather than disproven. The bootloader 0..0x1000/"upper region" addresses
must be confirmed against the actual MCU layout; SWD pinout/full relevant backup may
help, but their necessity/sufficiency are not automatically derived from the app base.

## K05014
Source `D0173`, original line171, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Correction:** "recovery is not confirmed, there is a risk of losing the working application".
Not "in case of HardFault, a guaranteed unrecoverable brick".

## K05015
Source `D0173`, original line174, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. What the report missed: OTA provenance/epoch

## K05016
Source `D0173`, original line176, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`tracker_ota.py:71` starts generation from 0, first start tag 2, drain tag 4.
A preceding normal PeerEngine could have already used these same numeric tags.
A complete OTA epoch/freshness boundary is not visible in this path.

## K05017
Source `D0173`, original line180, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Offline check of core:

## K05018
Source `D0173`, original line182, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Created OtaSession and begin(); active start generation2.
2. Sent positive start ACK with unrelated tx_generation0x1234.
3. ACK accepted: for start/data/finish tx_generation match is not checked.
4. Received drain generation4.
5. Fed no-op reply with numeric tag4, treated in the model as an old cached record.
6. Core accepts it and transitions to data, although probe did not perform a new RF no-op.

## K05019
Source `D0173`, original line189, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**What is proven:** acceptance rule does not distinguish record epoch and accepts
uncorrelated application ACK. **What is not proven:** specific occurrence
of such a delayed record in the real ring/queue under current poll order.
Fault-injection/model tests on actual ring ordering, MCU slots and queue are needed.
This scope cannot be replaced with Gemini's formulation "drain guarantees".

## K05020
Source `D0173`, original line195, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Select generation namespace after normal engine, do not repeat recent 16bit
tags, establish fresh record boundary, bind ACK to operation/session and
source provenance. For queued retransmitted ACK, correlation must take into account
the real native protocol, rather than blindly requiring current generation on any reply.
Before hardware OTA, this gap must receive a well-founded resolution.

## K05021
Source `D0173`, original line201, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 4. Reproducible evidence

## K05022
Source `D0173`, original line203, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Script: `outputs/check_gemini_fw189_review.py` — read-only firmware,
  temporary files and fake Debug; no device I/O.
- Result: `outputs/tracker-grid3000-20261007/gemini-review-countercheck.json`.
- Verified 30bytes/10sites/reverse/hash.
- Reproduced D1, core ACK acceptance and native TX gate; for each probe in JSON
  scope/limitation is explicitly recorded.
- Verified 24 files against frozen hashes of original validation MD; no changes.
- Both original MD revisions and production sources are preserved.

## K05023
Source `D0173`, original line212, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 5. Practical next step

## K05024
Source `D0173`, original line214, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Do not rewrite binary patch based on Gemini recommendations automatically.
First close host provenance and pre-OTA error persistence; separately analyze
TIMER0/PPI/queue event schedule and first RX deadline. Then activation/recovery
evidence. After that, hardware readiness of a single target can be reviewed.

## K05025
Source `D0173`, original line219, SHA256 `1099d50a1e4149d04ba9059348f691776956cb2114ee9e8cccfa4a4ecbd277fc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

User pause is maintained. This check is not a signal to start OTA.

## K05026
Source `D0174`, original line1, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# FW189 GRID3000 — iteration 02, 2026-10-07

## K05027
Source `D0174`, original line3, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

User goal: obtain release firmware today. Status of this iteration: RC1 for offline review; hardware release is not confirmed. Previous iteration report saved in `FW189_OFFLINE_REVISION_2026-10-07.md`. SSH/RF/OTA/services pause is maintained.

## K05028
Source `D0174`, original line5, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Changes

## K05029
Source `D0174`, original line7, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Real ACK queue and dispatcher

## K05030
Source `D0174`, original line9, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

New `pc/tests/fw189_queue_emulator.py` executes ARM:

## K05031
Source `D0174`, original line11, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- 0x901c — command table;
- 0x9270 — creation of three queues;
- 0x9368 → 0x92a8 → 0x922c — OTA ACK, its own sequence and insertion;
- 0x94fc — sequence filtering and dispatch of incoming working records;
- 0x95c4/0x9590 — response packing and list of transmitted reliable records;
- 0x96f8/0x969c/0x924c — S1 ACK processing and deletion of records;
- native OTA dispatcher/worker and verification of SPI staging from previous model.

## K05032
Source `D0174`, original line19, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Substituted allocator/free, IRQ critical sections, deferred callback scheduling, and SPI operations. RF/DMA arrival, RTOS scheduling, and real erase time are not modeled. Payload with verified working checksum is fed to dispatcher input; hardware CRC is provided by external calling code.

## K05033
Source `D0174`, original line21, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Confirmed: old reliable ACK is repeated at S1=2, deleted at S1=3; its sequence is a separate tracker counter. Repeating start with the previous command sequence after ACK loss does not trigger a second erase.

## K05034
Source `D0174`, original line23, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### New prepare_ack phase

## K05035
Source `D0174`, original line25, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

OtaSession starts with ACK-only (`S1=3`, payload `00`), not with erase. Start is permitted only after an exactly empty valid working payload `00` from the published generation of this phase. Any record or padding/terminator instead of a real empty payload leaves the preparation phase in place. Timeout does not send start.

## K05036
Source `D0174`, original line27, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For a start ACK pre-queued in the native queue: the first response does not advance the sender, the next ACK-only retire removes the entry, then an empty response permits a new start. One additional operation is accounted for in the minimum tag reserve: `2×blocks+5`.

## K05037
Source `D0174`, original line29, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The status separately shows `empty_prestart_reply_observed`; `native_ota_idle_verified` remains false. These concepts cannot be equated.

## K05038
Source `D0174`, original line31, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### The remaining race is now reproduced via native code

## K05039
Source `D0174`, original line33, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The counterexample has become stronger than the previous artificial host input:

## K05040
Source `D0174`, original line35, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. The native dispatcher accepts the old start for the image of 24 bytes. The OTA worker has not yet executed, there is no ACK yet.
2. A new host session for 25 bytes sees an empty queue and passes prepare_ack.
3. A new start with a new sequence is rejected by the native handler because the old OTA is still active.
4. The old worker completes erase and places its start ACK into the real queue.
5. The sender receives it under the current poll tag and accepts it. Native metadata still describes 24 bytes, the host intends to transmit 25.
