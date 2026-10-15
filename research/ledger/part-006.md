# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00401
Source `D0194`, original line180, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

Audit launch initially targeted preserved candidate07-grid SHA3957a3da...69e9.
During the read-only review, lead review added atomic state snapshot and fixed
same-mode/different-N staging. Final reviewed image above supersedes that exact
intermediate. Grok must receive a delta follow-up and verify final source/image;
an opinion on the intermediate alone is not final-image approval. Intermediates
and failing initial tests remain available. Evidence-manifest.json records
source hashes and verified frozen stock/disassembly provenance.

## K00402
Source `D0194`, original line188, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

## Independent review and disposition

## K00403
Source `D0194`, original line190, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

First review saved verbatim in `FW189_REMOTE_MODE44_GROK_FIRST_2026-10-08.md`.
Verdict: NOT OTA-ready. Lead verified native addresses and implemented:

## K00404
Source `D0194`, original line193, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

| Finding | Final disposition |
|---|---|
| Critical:62e4 called from Disabled creates unsolicited80us TX | Removed call; TIMER0 stays stopped until genuine native ADDRESS/Rx rearm; compare-path test added |
| High:pending N reissued command16 resets countdown | Pending!=0 re-query only; no repeated setter/countdown reset |
| High:binary/test drift during first review | Exact final image pinned above; new final JUnit, source manifest; delta review requested |
| Medium:retained native N pending survives startup/off | Clear8B mode and5B native N record on both boundaries, byte stores for unaligned f21 |
| Missing bootloader/recovery/new hash authorization | Unchanged unknowns; no OTA whitelist expansion or physical write |

## K00405
Source `D0194`, original line201, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

Lead also masks before halt ONLY for a pending transition and checks halt return0,
keeping the usual RX boundary unmasked when no transition is pending. First
reviewer's RTC/TIMER2 names were reversed in one paragraph;4000A000 is TIMER2,
4000B000 is RTC0. Core TX/countdown findings independently confirmed.
Final delta review outcome must be saved before any promotion.

## K00406
Source `D0194`, original line207, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

### Lead integration observation while final delta review runs

## K00407
Source `D0194`, original line209, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

Native8efc/8f00 (command19) sets20000fe1=1. This is the autonomous-transmit
permission, not evidence that command9 pairing completed. Host queries/sets mode
BEFORE its normal configuration reaches command19. Fixture Remote had forced
fe1=1, masking this integration condition. Withfe1=0, candidate6c702e51 stages
then cancels mode without applying; evidence initial-command19-gate.json.
Second Grok delta saved verbatim in FW189_REMOTE_MODE44_GROK_DELTA_2026-10-08.md:
TX/countdown defects closed on that exact intermediate, no additional code
blocker identified by reviewer; the lead observation above was not addressed.
Lead then removed only the false fe1 gate. It does NOT enable autonomous TX.
Final candidate40941b34...56d5 leaves fe1 unchanged at0 or1, and keeps TIMER0
stopped. Test initial connection executes actual8f0c/6100 and all three6df6
countdown/commit passes through query/set/query BEFORE command19, then proves
Frame mode/N13 whilefe1 remains0. tests-real-countdown.xml:1 passed;
tests-initial-connection-final.xml:38 passed before that extra integration test.
Full245-test final campaign and source/assembly/binary manifest match verified.
Final focused Grok delta requested; release/hardware gates remain closed.

## K00408
Source `D0194`, original line226, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

## Final audit outcome

## K00409
Source `D0194`, original line228, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

Claude Opus5.5 third-party audit (original reference: `FW189_REMOTE_MODE44_CLAUDE_AUDIT_2026-10-08.md`):
no code blocker; 245 tests reproduced; low findings A1/A2 to resolve or record before OTA.

## K00410
Source `D0194`, original line231, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

Final Grok4.7medium opinion (original reference: `FW189_REMOTE_MODE44_GROK_FINAL_2026-10-08.md`)
confirms no further demonstrable code blocker in exact40941b34...56d5 image.
It specifically verifies mode-before19 leaves autonomous permission unchanged
and does not start TIMER0. First review (original reference: `FW189_REMOTE_MODE44_GROK_FIRST_2026-10-08.md`)
and second review (original reference: `FW189_REMOTE_MODE44_GROK_DELTA_2026-10-08.md`) preserved.
Lead verified fixes and source/assembly/binary hashes;245 final tests pass.

## K00411
Source `D0194`, original line238, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

Work requested here is complete as an audited OFFLINE firmware/host candidate.
Installed tracker remains05. No new OTA, deployed automatic-mode config or RF
measurement. Physical transition, coexistence, bootloader and recovery remain
unknown; approved_for_ota=false and exact05 hardware exception unchanged.
Do not interpret the review or full emulated transfer as physical OTA approval.

## K00412
Source `D0194`, original line244, SHA256 `523b7303e4c14851824e16c20b5bc02a5235e94c1eb9d969d247455b8af853f8`. Status: historical/unadjudicated narrative; English source prose.

Frame was used only as clang/ld.lld build host; new builder copied, radio runtime
was not deployed/restarted. No new tracker OTA, Frame flash, RF experiment or
controller measurement in44. Installed tracker still05; actual present mode
unknown without fresh hardware response. Final independent findings must be
saved alongside and resolved before considering exact-image OTA authorization.

## K00413
Source `D0195`, original line1, SHA256 `1ca32645a98078b0eb6f0a332e0db5ddf187174253fea23df68f34154b415cdb`. Status: historical/unadjudicated narrative; English source prose.

# Revision07 independent audit disposition

## K00414
Source `D0195`, original line3, SHA256 `1ca32645a98078b0eb6f0a332e0db5ddf187174253fea23df68f34154b415cdb`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. Offline documentation and source-comment follow-up only.

## K00415
Source `D0195`, original line5, SHA256 `1ca32645a98078b0eb6f0a332e0db5ddf187174253fea23df68f34154b415cdb`. Status: historical/unadjudicated narrative; English source prose.

Source: independent Claude audit (original reference: `FW189_REMOTE_MODE44_CLAUDE_AUDIT_2026-10-08.md`).
The original audit and historical Grok findings remain preserved. This is a
disposition record, not a new independent review or hardware result.

## K00416
Source `D0195`, original line9, SHA256 `1ca32645a98078b0eb6f0a332e0db5ddf187174253fea23df68f34154b415cdb`. Status: historical/unadjudicated narrative; English source prose.

## Decisions

## K00417
Source `D0195`, original line11, SHA256 `1ca32645a98078b0eb6f0a332e0db5ddf187174253fea23df68f34154b415cdb`. Status: historical/unadjudicated narrative; English source prose.

- **A1 OPEN, known limitation.** Preserve the independently audited revision07
  binary rather than silently changing its protocol or disconnect hooks.
  Schema2's last byte describes native N pending, not remote staging at
  0x20005151/52. Native64ac does not clear remote staging. A queued transition
  can survive disconnect and commit after a successful query on reconnect.
  Unchanged desired policy often makes that transition agree with the host,
  but that is not proof that no transition remains. The 10-second refresh can
  detect a later mismatch while communication works; it neither prevents it
  nor guarantees a maximum outage. A future firmware revision should expose
  staging atomically or cancel it in the actual disconnect path, preserving
  the selected mode and native teardown semantics. No such fix is claimed here.
- **A2 retained integration requirement.** After an applied mode transition,
  restart native configuration with command16, even when N already matches.
  Remote commit does not execute native6e0c..6e1a/f25 one-shot report8a71.
  Post-switch command16 supplies the native path. Added a source comment at
  the configuration restart; existing remote-mode test checks command16 next.
  Do not confuse this required post-switch command with prohibited resends
  while the native N countdown is still pending.
- **A3 documentation corrected.** STATE==0 after successful57ec is defensive.
  Do not claim an independently demonstrated active-radio deferral branch.
  Waiting for EVENTS_DISABLED under PRIMASK requires no other interrupt, but
  an actual peripheral stall would prevent interrupt preemption. Hardware
  behavior remains unknown; offline execution cannot rule out that stall.
- **A4 hardware unknown.** TIMER0 remains stopped until a matching host packet
  produces native ADDRESS/Rx rearm. No autonomous safety transmission is armed.
  Common-grid calculations are a preflight condition, not measured evidence
  that RF will reacquire after switching. Test this before any hardware promotion.

## K00418
Source `D0195`, original line39, SHA256 `1ca32645a98078b0eb6f0a332e0db5ddf187174253fea23df68f34154b415cdb`. Status: historical/unadjudicated narrative; English source prose.

## Preserved artifact and approval

## K00419
Source `D0195`, original line41, SHA256 `1ca32645a98078b0eb6f0a332e0db5ddf187174253fea23df68f34154b415cdb`. Status: historical/unadjudicated narrative; English source prose.

`outputs/remote44/candidate07-initial-connection/fw189-dual-mode.bin`:
97,832 bytes, SHA256
`40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5`.

## K00420
Source `D0195`, original line45, SHA256 `1ca32645a98078b0eb6f0a332e0db5ddf187174253fea23df68f34154b415cdb`. Status: historical/unadjudicated narrative; English source prose.

No executable behavior or firmware binary changed in this disposition.
The previous 245-test result remains historical; the focused remote-mode suite
was rerun for this follow-up: **39 passed in 6.24 seconds**, using
`.venv/Scripts/python.exe -m pytest -q -p no:faulthandler pc/tests/test_remote_mode44.py`.
The binary SHA256 above was independently recomputed and remains unchanged.
The historical `outputs/remote44/evidence-manifest.json` remains preserved;
its host-source hash describes the pre-disposition source (before comment-only
changes), not the current commented file. No RF, deployment, restart, or OTA is performed.
`approved_for_ota=false` remains. Installed revision05 is not replaced.

## K00421
Source `D0197`, original line1, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

# Grok 4.7 medium second review

## K00422
Source `D0197`, original line3, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

Verbatim response on preserved candidate6c702e51...2eb6 below. The lead's
command19 integration gate observation remains separate; this response does
not address that newly recorded finding. Subsequent image needs its own delta.

## K00423
Source `D0197`, original line7, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

**Not OTA-ready, and not hardware-safe.** On exact image `outputs/remote44/candidate07-audit-final/fw189-dual-mode.bin` (97,848 bytes, manifest SHA256 `6c702e51c22f7521ae5a00b13550557a32baf72e19822cb6df6f26b092992eb6`) the two confirmed code defects are fixed. I do not see a new demonstrable code bug that brings the unsolicited TX or the command-16 countdown reset back.

## K00424
Source `D0197`, original line9, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

`tests-audited-regression.xml` is complete: 243 passed, 0 failures, 2026-10-08 22:43. `tests-audit-final.xml` is 36 passed on this image. `approved_for_ota` remains false. Installed firmware is still `395ce84b…`.

## K00425
Source `D0197`, original line11, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

## Correction

## K00426
Source `D0197`, original line13, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

`0x4000A000` is TIMER2. `0x4000B000` is RTC0. In `0x641c`, `20000f02==0` reads TIMER2 PRESCALER at `0x4000A510` and schedules TIMER2. Nonzero `20000f02` clears RTC0 and writes RTC0 `CC[0]` at `0x4000B540`. `0x6fd8` is the RTC0 `EVENTS_COMPARE[0]` entry into `0x6ab8`. `0x6ff4` is the other compare entry. IRQ 10 (TIMER2, priority `0x40` at `0x6630`) and IRQ 11 (RTC0, priority `0x40` at `0x6a9a`) are the same priority. TIMER0 is `0x40008000`, IRQ 8, priority `0x60`. The extra-TX finding did not depend on those names: `0x5e9c` is reached only from TIMER0 `0x6250` and `0x62bc`.

## K00427
Source `D0197`, original line15, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

## Disposition of the first findings

## K00428
Source `D0197`, original line17, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

| Finding | On this image |
|---|---|
| `0x62e4` from Disabled starts an 80 µs TX | Fixed in `profile_rx_boundary`. No branch to `0x62e4`. After a successful halt and `RADIO.STATE==0` it writes TIMER0 `TASKS_SHUTDOWN` (`0x40008010`), `TASKS_CLEAR` (`0x4000800C`), clears `EVENTS_COMPARE[0]` and `[1]`, and clears NVIC pending bit 8. The timer is not started. |
| Pending N resends command 16 and reloads countdown 2 | Fixed in `PeerEngine._accept_mode`. `b[25]!=0` only re-queries. Command 16 is sent only when pending is already 0 and live N differs. Its transport ACK still only starts another query. |
| Test/image drift | `test_remote_mode44.py` loads `candidate07-audit-final`. Manifest SHA matches the requested image. |
| Retained `20000f21` across startup and full-off | `profile_boot_entry` and `profile_power_off` store zero across all 8 bytes at `0x20005150` and byte-clear `0x20000f21` through `0x20000f25`. The hold counter clear at `0x20001ff2` remains. |
| Bootloader, recovery, new-hash authorization | Unchanged. Still no flash basis. |

## K00429
Source `D0197`, original line25, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

## What the pending path now does

## K00430
Source `D0197`, original line27, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

A nonzero pending byte masks IRQ before `0x57ec`. `0x852c` / `0x7fd4` only toggle GPIO (`0x50000508` / `0x5000050C`); they do not wait for an interrupt, so the mask does not deadlock the `EVENTS_DISABLED` spin. Return value `0x101` (software state at `0x200008c8` not 4 or 5) leaves pending in place and does not stop TIMER0. Return 0 with `STATE!=0`, or with `20000f6a!=0`, also defers. OTA, radio-disabled, or disconnected cancels pending. The commit of mode, live N, and `f24`, and the clear of command-16 countdown, happen only after return 0 and `STATE==0`, still under that mask. With no pending byte, `0x57ec` keeps the incoming `PRIMASK`.

## K00431
Source `D0197`, original line29, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

The next `0x62e4` is still only `0x52dc` → `0x532c`, and only when `EVENTS_ADDRESS` is set and `STATE==3`. `0x626c` then does not call `0x5e9c`. `test_no_extra_tx_until_legitimate_radio_address_rearm` runs that sequence on this image: shutdown observed, `0x5e9c` not entered, then ADDRESS/Rx writes `CC[0]=12000` and compare1 while Rx still does not enter `0x5e9c`. `test_native_receive_boundary_and_radio_halt_then_new_rx_delay` runs real `0x57ec` with an emulated DISABLED event and checks that `0x40008000` and `CC[1]` stay 0. Unicorn does not raise the IRQ by itself; the compare test calls `0x626c` directly. That is offline branch evidence.

## K00432
Source `D0197`, original line31, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

Schema 2 snapshots mode, N, and `20000f22` under `PRIMASK` and restores it before `0x9368`. Same mode with a different N, or with native N pending, stages instead of taking status 0. Streaming still requires `phase_verified`, which command 10 sets and every switch clears.

## K00433
Source `D0197`, original line33, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

## What is not a new code defect

## K00434
Source `D0197`, original line35, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

Stopping TIMER0 means `0x5e9c` cannot run until a later ADDRESS match in Rx. Those are its only two callers. The following `0x6cac` / `0x6d22` / `0x641c` delay is the native receive-complete scheduler. The emulator then shows ADDRESS rearm works. A physical gap until that ADDRESS is unmeasured, not an extra TX inserted by this hook.

## K00435
Source `D0197`, original line37, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

If `0x57ec` returns nonzero, TIMER0 is left running on purpose. Stopping it there could hit a radio that is still in Rx. That leftover compare is the pre-existing timer, and this path does not call `0x62e4`.

## K00436
Source `D0197`, original line39, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

TIMER2 and RTC0 can both enter `0x6ab8`, at equal priority, and the hook does not disarm the inactive one. That overlap is older than this patch. It does not arm the 80 µs compare.

## K00437
Source `D0197`, original line41, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

The no-pending receive-complete path still calls `0x57ec` with interrupts as it found them. That is stock `0x6c98` behavior.

## K00438
Source `D0197`, original line43, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

A queue-rejected command 16 can still advance the host sequence on the transport ACK. The following query sees the real N and pending byte. Pending nonzero does not send another command 16, so the M=2 countdown is not reloaded.

## K00439
Source `D0197`, original line45, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

## Verdict

## K00440
Source `D0197`, original line47, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

Code disposition: the first review’s TX and countdown defects are closed on `6c702e51…`. I do not have a further code blocker in this delta.

## K00441
Source `D0197`, original line49, SHA256 `0b37093a4b089e5cdc34669dfd6d57a964b0912c02e1bb8f6574f1863a70aabd`. Status: historical/unadjudicated narrative; English source prose.

Release disposition: do not flash it. Bootloader and recovery are still unknown, this hash is not the installed image, and no physical transition, cadence, or controller measurement exists.

## K00442
Source `D0198`, original line1, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

# Grok 4.7 medium final independent delta

## K00443
Source `D0198`, original line3, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

Read-only Cursor bridge, English response saved verbatim below. Exact final
image40941b34...56d5,97832B. No hardware, OTA or deployment performed by reviewer.
Lead clarification: native init/teardown can CLEAR fe1; the statement below that
it is written only by command19 means the identified SET-to1 path. This final
review finds no further demonstrable code blocker; it is not hardware validation
or authority to flash. First and second reviews are preserved separately.

## K00444
Source `D0198`, original line10, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

**No new code blocker.** Exact image `outputs/remote44/candidate07-initial-connection/fw189-dual-mode.bin`, 97,832 bytes, SHA256 `40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5`, stays `approved_for_ota: false`. Dropping the `20000fe1` check lets the initial mode commit finish, and it does not grant autonomous transmit. Hardware, bootloader, and recovery remain unknown. This is not an OTA approval.

## K00445
Source `D0198`, original line12, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

`0x4000A000` is TIMER2. `0x4000B000` is RTC0. TIMER0 is `0x40008000`. `20000fe1` is the byte at `20000fd0+0x11`, written only by native command 19 at `0x8efc`/`0x8f00`.

## K00446
Source `D0198`, original line14, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

## What changed

## K00447
Source `D0198`, original line16, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

`profile_rx_boundary` no longer reads or writes `20000fe1`. The builder hash in `evidence-manifest.json` matches the image manifest (`968d80ba…`). The initial-connection assembly has no `20000fd0` reference. Commit still requires `0x57ec` return 0, `RADIO.STATE==0`, `20000e10` enabled, `20000f6a==0`, and OTA `20002240` idle. The setter still requires the four chip bytes at `20000efe`. The hook still only shuts TIMER0 down and clears its compares and NVIC bit 8. It does not write `TASKS_START` and does not call `0x62e4`.

## K00448
Source `D0198`, original line18, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

`outputs/remote44/initial-command19-gate.json` records the old image `6c702e51…` with `fe1=0`: pending `000200` became `000000`. That was the cancel. This image commits instead and leaves the permission byte as it found it.

## K00449
Source `D0198`, original line20, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

## Why this does not start TX before command 19

## K00450
Source `D0198`, original line22, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

`0x5e9c` is still reached only from TIMER0 `0x6250` and `0x62bc`. Both functions that start that timer refuse when `20000fe1` is 0:

## K00451
Source `D0198`, original line24, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

- `0x62e4` at `0x62f6` skips the arm and the `CC[1]=80` write.
- `0x61f4` at `0x6206` returns before `TASKS_START` and before `0x5e9c`.

## K00452
Source `D0198`, original line27, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

Command 19 sets the flag and then goes to `0xaca4`. It does not start TIMER0. Until some later ADDRESS match in Rx finds the flag already 1, `0x532c` will not arm the timer. A compare that is already running can still call `0x5e9c` without reading `fe1`; this hook stops TIMER0 and does not set the flag, so a zero flag stays zero.

## K00453
Source `D0198`, original line29, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

## Host order

## K00454
Source `D0198`, original line31, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

Mode is still handled in `_confirmed` before the configuration list `(17, 25, 19)`. Command 16 for the current profile runs first. `test_initial_connection_real_command16_countdown_then_mode_before19` runs real `0x8f0c`/`0x6100`, then all three `0x6df6` passes: countdown 2, then 1, then commit of stock N `0x09`, with the pending queries not reloading command 16. The setter and the following query then show Frame and live N `0x0d`, while `20000fe1` is still 0. `test_mode_selectable_before_command19_without_enabling_autonomous_tx` does the same commit for permission 0 and 1 and checks the byte is unchanged and TIMER0 start stays 0. The fixture no longer forces `fe1=1`.

## K00455
Source `D0198`, original line33, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

`tests-final-exact.xml` is complete: 245 passed, 0 failures, 2026-10-08 22:51. `tests-real-countdown.xml` is the one integration test. `tests-initial-connection-final.xml` is 38 passed from before that test was added.

## K00456
Source `D0198`, original line37, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

Code: this delta closes the initial-connection cancel. I do not see a demonstrable code bug that turns command 19’s permission on early or that rearms TIMER0.

## K00457
Source `D0198`, original line39, SHA256 `5b68992f331b6533e6c43dfc19ebd63b12dcca5773984e3f586ba8f60983d5ea`. Status: historical/unadjudicated narrative; English source prose.

Release: still not flashable. No bootloader, no recovery, and no physical proof that the next ADDRESS rearm and command 19 behave on the tracker.

## K00458
Source `D0199`, original line1, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

# Grok 4.7 medium independent first review

## K00459
Source `D0199`, original line3, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

Verbatim response below. Read-only Cursor bridge; no hardware or OTA.
Review initially targeted preserved candidate07-grid SHA3957a3da...69e9.
Source changed during review as disclosed in the audit entry; this response is
not approval of later images. Reviewer swaps RTC/TIMER2 labels in one paragraph:
4000A000 is TIMER2,4000B000 RTC0, as native SDK/tests show. Lead checks findings
against native code rather than treating every reviewer assertion as fact.

## K00460
Source `D0199`, original line10, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

**OTA-ready: no.** The grid candidate `outputs/remote44/candidate07-grid` (manifest SHA256 `3957a3da605d9f3613fcb2a19c75c528e293437c9264448eda2c9a5e798669e9`, 97,736 bytes, `approved_for_ota: false`) must not be flashed. The mode/N commit itself is coherent, but the receive-complete hook arms the native TX timer in the one state where that timer starts a transmission, and the host can keep that N from ever committing.

## K00461
Source `D0199`, original line12, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

## Critical

## K00462
Source `D0199`, original line14, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

**1. `profile_rx_boundary` calls `0x62e4` after the radio is already Disabled, which arms an 80 µs compare that starts TX.**

## K00463
Source `D0199`, original line16, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

Native `0x6c98` is only `bl 0x57ec` inside `0x6ab8` when `20000ef5==2`. That state is set at `0x65f4` and the callback is entered from TIMER2 `0x6fe8` or the RTC compare dispatcher `0x6ffa`. `0x57ec` disables the radio only when `200008c8` is 4 or 5: it clears RADIO SHORTS, clears all RADIO interrupts, writes `TASKS_DISABLE` (`0x40001010`), and spins until `EVENTS_DISABLED` (`0x40001110`). On any other software state it returns `0x101` and leaves the radio alone. The hook ignores that return value.

## K00464
Source `D0199`, original line18, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

The hook then masks IRQs and requires `RADIO.STATE` (`0x40001550`) == 0, `20000e10` enabled, `20000fe1` connected, `20000f6a==0`, and OTA byte `20002240==0`. On that path it shuts TIMER0 down, then calls `0x62e4`. `0x62e4` is not a neutral period reload. Its only native caller is `0x532c`, and only when `EVENTS_ADDRESS` is set and `RADIO.STATE==3` (Rx). It writes `CC[0]` (`0x40008540`) from the mode selectors (`0x630a`/`0x630e`: Frame 12000/3000, Stock 18910/4727), sets SHORTS bit 8 (`COMPARE0_CLEAR`), sets `CC[1]=0x50` (80 µs at the TIMER0 prescaler 4 / 1 µs tick programmed at `0x6188`), sets INTEN bits 16 and 17, and starts the timer.

## K00465
Source `D0199`, original line20, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

TIMER0 IRQ `0x626c` consumes `EVENTS_COMPARE[1]` and branches to `0x5e9c` unless `STATE==3` or `STATE` is 9..12. `0x5e9c` calls `0x57ec` again, marks software state 5, and writes `TASKS_TXEN` (`0x40001000`). After this hook, state is 0, so the suppress condition is false. `0x5e9c` runs.

## K00466
Source `D0199`, original line22, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

Static NVIC priorities: TIMER0 IPR[8] = `0x60` (priority 3) at `0x61ac`; TIMER2 IPR[10] = `0x40` and RTC0 IPR[11] = `0x40` (priority 2) at `0x6630` and `0x6a9a`. Priority 3 does not preempt the timer callback, so this is not an in-function preemption. The compare is still pending when `0x6ab8` returns, and it then transmits. `COMPARE0` later takes `0x61f4`, which also tail-calls `0x5e9c` with no `STATE==3` check. Clearing NVIC pending bit 8 (`0xE000E280`, mask `0x100`) drops a stale TIMER0 request and does not cover TIMER2 IRQ 10, RTC0 IRQ 11, or PPI. The PPI channel programmed at `0x6148` is ADDRESS → TIMER0 `TASKS_STOP`, so it is not the TX source. The TX source is the software IRQ.

## K00467
Source `D0199`, original line24, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

The following native `0x6cac` / `0x6d22` / `0x641c` path is a different scheduler (RTC `0x4000A000` when `20000f02==0`, else TIMER2 `0x4000B000`). `20000f02` is clock-domain selection. The hook leaves it alone and does not disarm the inactive domain. An already-armed compare on the other timer is not cleared. That ordering is not established.

## K00468
Source `D0199`, original line26, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

Narrow fix: under the same PRIMASK section, commit mode and N, clear `20000f21` bytes 1 and 2, shut TIMER0 down, and clear COMPARE0/1 and NVIC bit 8. Do not call `0x62e4`, do not write `CC[1]=80`, and do not set INTEN bit 17. The next legitimate rearm remains `0x532c` while `STATE==3`, and the selectors already read `20005150` at use time. Stopping TIMER0, which native `0x6c98` left running, drops one timer-driven TX until that ADDRESS rearm. Restarting it here is worse.

## K00469
Source `D0199`, original line28, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

## High

## K00470
Source `D0199`, original line30, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

**2. Command 16 resend resets the countdown that the query is waiting on.**

## K00471
Source `D0199`, original line32, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

`0x8f0c` loads body `[N, M]` and `0x6100` stores `20000f24=N`, `20000f23=M`, `20000f22=1`. It does not write live `20000f21`. `0x6df6` commits `f24` into `f21` only when pending is 1 and the countdown byte is already 0. Any nonzero countdown is decremented and the commit is skipped. Host `decimation_m` is 2, so the first two `0x6df6` passes only count down; the third pass commits.

## K00472
Source `D0199`, original line34, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

`PeerEngine._accept_mode` treats pending != 0 as failure and sends command 16 again. `test_current_grid_requires_actual_n_pending_zero_before_set` locks that in: a reply with pending 1 produces another command 16. Each new `0x6100` stores countdown 2 again. A query that arrives before the third `0x6df6` therefore prevents N from committing. Mode-set and mode-query do not advance sequence on a bare transport ACK while `mode_nonce` is set. Command 16 is different: `mode_preparing` clears the nonce first, `ack_tag` accepts the transport ACK, and the next query is issued. That is the retained queue-admission behavior applied to N.

## K00473
Source `D0199`, original line36, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

Narrow fix in `_accept_mode`: if `b[25]!=0`, re-query only. Send command 16 only when pending is 0 and live N differs. Do not infer success from the command 16 ACK.

## K00474
Source `D0199`, original line38, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

**3. The 30-test grid run does not exercise this image’s IRQ behavior, and the test file no longer loads it.**

## K00475
Source `D0199`, original line40, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

`outputs/remote44/tests-grid.xml` is 30 passes of `test_remote_mode44` only (2026-10-08 22:28, 6.1 s). It has no `test_atomic_n_replaces_old_native_pending_request`, no `test_native_receive_boundary_and_radio_halt_then_new_rx_delay`, and no `test_current_grid_requires_actual_n_pending_zero_before_set`. The current test module reads `outputs/remote44/candidate07-snapshot`, whose manifest is a different image: 97,768 bytes, SHA256 `7c21f8bb57de3ea952c794963f58dac61e7cbe28f9943f6e54d0b796e36ee036`, assembly SHA256 `53c2799e…`. The grid assembly SHA256 is `bce3d04f…`. `tests-final.xml` (236) is an earlier intermediate and is not evidence for `3957a3da…`.

## K00476
Source `D0199`, original line42, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

`Remote._hook` returns immediately at `0x57ec`. The one test that models `EVENTS_DISABLED` calls `0x6c98` as a subroutine and expects `CC[0]==12000`, which is the harmful `0x62e4` store. Unicorn in M-class mode executes `PRIMASK`; it does not deliver TIMER0. The boundary test’s delay value 7810 is not in the grid junit.

## K00477
Source `D0199`, original line44, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

## Medium

## K00478
Source `D0199`, original line46, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

**4. Startup and full-off do not clear the retained command-16 record.**

## K00479
Source `D0199`, original line48, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

`profile_boot_entry` writes `20005150` and `20005151` to 0 and clears the hold counter `20001ff2`. `profile_power_off` does the same after `0x5fe0`. Neither clears `20005152` (staged N) nor `20000f21..20000f25`. Iteration 43 showed `20001ff2` surviving repeated `0xc3ec` entry because that path is not C startup. The same RAM class can still hold a command-16 countdown, and the next `0x6df6` can commit it after the mode bytes were cleared. Clear all eight `mode_flag` bytes and the five `20000f21` bytes at both sites. Reason-0 and counts 100..149 are otherwise intact in `profile_boot_hold` / `profile_boot_release`; other reset reasons still cannot select Frame. Gesture timing remains hardware-unknown.

## K00480
Source `D0199`, original line50, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

**5. There is still no bootloader, recovery, or exact-image gate for this hash.**
