# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K03921
Source `D0612`, original line26, SHA256 `2938bb5e38288aeeb82f5ee25fcccbdf39775e90f713c03432d18846e333825c`. Status: historical/unadjudicated narrative; English source prose.

**Sequence 1 is not justified as a send.** It is the only value `0x94fc` will accept after a sequence-0 record has been accepted, including when `0x15d4` fails. The live halfword is `0x2000102E` (`0x20000fd0+0x34+2*21`), bits 6–11. Phase 172 never read it. The fresh-model test accepts sequence 0 and rejects sequence 1. If the published record never reached `0x94fc`, sequence 1 is ignored. If `0x7c44` already ran, sequence 1 repeats the same PPI-then-START order under the same 6 ms radio.

## K03922
Source `D0612`, original line28, SHA256 `2938bb5e38288aeeb82f5ee25fcccbdf39775e90f713c03432d18846e333825c`. Status: historical/unadjudicated narrative; English source prose.

## What an existing reply can prove

## K03923
Source `D0612`, original line30, SHA256 `2938bb5e38288aeeb82f5ee25fcccbdf39775e90f713c03432d18846e333825c`. Status: historical/unadjudicated narrative; English source prose.

Nothing in the installed revision 09 proves command-21 execution or TIMER1 state. `FE04` byte 23 is `FA7` itself. `FE03` bytes for `0x2000066c` and `0x2000067c` are not the `0x15d4` mailbox at `0x20001a58`. Command 1 subcommand `0x02` indexes 0..12 read fixed calibration, not `CC[2]` or that halfword. A transport ACK is not `0x8d40`.

## K03924
Source `D0612`, original line32, SHA256 `2938bb5e38288aeeb82f5ee25fcccbdf39775e90f713c03432d18846e333825c`. Status: historical/unadjudicated narrative; English source prose.

## One next intervention

## K03925
Source `D0612`, original line34, SHA256 `2938bb5e38288aeeb82f5ee25fcccbdf39775e90f713c03432d18846e333825c`. Status: historical/unadjudicated narrative; English source prose.

Add one read-only diagnostic reply, reviewed offline, and do not flash it until the existing pre-OTA pause. The reply only loads:

## K03926
Source `D0612`, original line36, SHA256 `2938bb5e38288aeeb82f5ee25fcccbdf39775e90f713c03432d18846e333825c`. Status: historical/unadjudicated narrative; English source prose.

- bits 6–11 of `0x2000102E`
- TIMER1 `MODE` `0x40009504`, `BITMODE` `0x40009508`, `PRESCALER` `0x40009510`, `CC[2]` `0x40009548`, `EVENTS_COMPARE[2]` `0x40009148`
- the PPI channel byte at `0x20000f82` and that channel’s EEP/TEP

## K03927
Source `D0612`, original line40, SHA256 `2938bb5e38288aeeb82f5ee25fcccbdf39775e90f713c03432d18846e333825c`. Status: historical/unadjudicated narrative; English source prose.

It must not write tasks, clear events, or change CHEN. Read it once after a fresh command-21 publication on a later, already-authorized image:

## K03928
Source `D0612`, original line42, SHA256 `2938bb5e38288aeeb82f5ee25fcccbdf39775e90f713c03432d18846e333825c`. Status: historical/unadjudicated narrative; English source prose.

- sequence still 0 and the `0x7b50` register pattern absent: the record never passed `0x94fc`
- sequence 1 and the timer still at reset: enqueue or worker missed `0x7b50`
- sequence 1, init pattern present, `CC[2]` no longer 500000, compare event 0: capture moved the compare
- init pattern present, `CC[2]` still 500000, compare event 1, `FA7` 0: the event was raised and `0x7760` did not store the gate

## K03929
Source `D0612`, original line47, SHA256 `2938bb5e38288aeeb82f5ee25fcccbdf39775e90f713c03432d18846e333825c`. Status: historical/unadjudicated narrative; English source prose.

Do not send command 9, sequence 1, or another unchanged command 21 on the installed image.

## K03930
Source `D0613`, original line1, SHA256 `8bc80ad4031a18cf51d673c6c74e254b843e74373ae2280644052b1cc8f1cb34`. Status: historical/unadjudicated narrative; English source prose.

No. One command 10 with body `00000000` after fresh mode 1 and sensor 17 is not a sensor-timer start, and it is not justified before command 19.

## K03931
Source `D0613`, original line3, SHA256 `8bc80ad4031a18cf51d673c6c74e254b843e74373ae2280644052b1cc8f1cb34`. Status: historical/unadjudicated narrative; English source prose.

`0x40001104` is `RADIO.EVENTS_ADDRESS`. `EVENTS_END` is `0x4000110C`. `0x767c` and `0x79d2` both use the ADDRESS address. Phase 172 never published command 10. Its `phase_applied` mark is the Roy BLE phase. `actions.jsonl` contains command 21 only.

## K03932
Source `D0613`, original line5, SHA256 `8bc80ad4031a18cf51d673c6c74e254b843e74373ae2280644052b1cc8f1cb34`. Status: historical/unadjudicated narrative; English source prose.

Command 10 is `0x8df0`. `0x59cc` must return the byte at `0x20000e08` as 0, or `0x79b8` is skipped. `0x79c8` then requires PPI channel 3 TEP `0x4001F52C` to already equal `TIMER1.TASKS_CAPTURE[2]` `0x40009048`. That value is written by `0x767c`, which runs from `0x7c44`. Sensor 17’s handler `0x8ee0` does not call `0x7c44`. Mode 1 does not either. Phase 172’s command 21 publication left `FA7=0`, so this precondition is unproven. If it fails, `0x79ce` returns 0 and the body is a no-op.

## K03933
Source `D0613`, original line7, SHA256 `8bc80ad4031a18cf51d673c6c74e254b843e74373ae2280644052b1cc8f1cb34`. Status: historical/unadjudicated narrative; English source prose.

Body 0 also does not start the timer or set the gate. `0x79e0` adds `0x1D3` to the signed body, and `0x7a0c` subtracts that from the current `CC[2]` using the period at `0x2000011c`. The new tests plant `CC[2]=100000` and period `533333`, so 0 becomes `99533` and TEP is written 0 at `0x79ea`. `FA7` stays 0 until a separate `0x7760` call with `0x148`. `0x785a` then sets `FA7` and branches to `0x767c`, which puts `0x40009048` back. There is no `TASKS_START` write on this path. `0x78f4`, which fills `0x2000011c`, is not called by command 10. A live `CC[2]` of 0 or a zero period takes the signed path at `0x7a26` and can write PPI `TASKS_CHG[0].EN` at `0x4001F000` (`0x7a3c`). That is not the planted `99533` result.

## K03934
Source `D0613`, original line9, SHA256 `8bc80ad4031a18cf51d673c6c74e254b843e74373ae2280644052b1cc8f1cb34`. Status: historical/unadjudicated narrative; English source prose.

`0x77c6` writes TIMER1 `SHORTS=1` (`COMPARE0_CLEAR`) only on callback argument `0x140`, and only when `CC[0]` differs from `0x2000011c`. That does make a 268-second wrap unnecessary once COMPARE0 interrupts are already running. Command 10 does not enter `0x7760`, and this shortcut does not run if `0x7b50` never started TIMER1.

## K03935
Source `D0613`, original line11, SHA256 `8bc80ad4031a18cf51d673c6c74e254b843e74373ae2280644052b1cc8f1cb34`. Status: historical/unadjudicated narrative; English source prose.

The sequence-filter byte at `0x20001868+10*16+3` is 0, so `0x94fc` accepting sequences 0, 1, and 63 only means the record is not dropped for sequence. It does not make `0x79b8` perform the fixture rewrite. Do not send command 10, command 9, command 21, or a sequence sweep on this image.

## K03936
Source `D0615`, original line1, SHA256 `7a79af2d524bec2a1c8618a98b06082dd6c8b4a05f3181dc6845a1fe3d11b7c3`. Status: historical/unadjudicated narrative; English source prose.

No source blocker remains for the before/after stamp classifier. Candidate `040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d`, 98868 bytes, is offline, `approved_for_ota` is false, and it is not installed.

## K03937
Source `D0615`, original line3, SHA256 `7a79af2d524bec2a1c8618a98b06082dd6c8b4a05f3181dc6845a1fe3d11b7c3`. Status: historical/unadjudicated narrative; English source prose.

The five hooks perform the native store they replace, then record the sequence under the saved PRIMASK, then restore APSR and `r0`–`r3`, `r12`, and `LR`.

## K03938
Source `D0615`, original line5, SHA256 `7a79af2d524bec2a1c8618a98b06082dd6c8b4a05f3181dc6845a1fe3d11b7c3`. Status: historical/unadjudicated narrative; English source prose.

- `0x8d58` still tails to `0x78f4` with the selector in `r0`. Its stamp is guard-accepted, before `0x78f4` returns.
- `0x7b8e` writes `TASKS_START` (`str r4,[r3]`, `r4` is 1) and resumes at `0x7b92`.
- `0x7be2` writes the FA7 zero and returns with `bx lr`.
- `0x65c0` writes the other FA7 clear and resumes at `0x65c4`.
- `0x785a` writes FA7 one and resumes at `0x785e`. That store is reached from `0x7760` only through the `0x148` branch at `0x776c`.

## K03939
Source `D0615`, original line11, SHA256 `7a79af2d524bec2a1c8618a98b06082dd6c8b4a05f3181dc6845a1fe3d11b7c3`. Status: historical/unadjudicated narrative; English source prose.

Kinds 3 and 4 have separate stamps at `0x20005164` and `0x2000516c`. The shared u8 at `0x20005156` is not that distinction. Stamp 0 means that action has not completed since the last zeroing. A nonzero stamp is the sequence after that action’s last store, so the last start, stop, gate-set, and other-clear can be ordered even when the u8 counts are mixed. `0x20005153` is only the latest kind.

## K03940
Source `D0615`, original line13, SHA256 `7a79af2d524bec2a1c8618a98b06082dd6c8b4a05f3181dc6845a1fe3d11b7c3`. Status: historical/unadjudicated narrative; English source prose.

Both heap literals at `0x1348` and `0x134c` are `0x20005170`. Startup zeroes through that address and then places the heap header there; `0x20005350 - 0x20005170` is 480 bytes. `profile_boot_entry` and `profile_power_off` each store zero through all eight words at `0x20005150`. The observation pages only load; each 32-bit field is one load split into bytes. A load of `0x40009148` does not clear the event.

## K03941
Source `D0615`, original line15, SHA256 `7a79af2d524bec2a1c8618a98b06082dd6c8b4a05f3181dc6845a1fe3d11b7c3`. Status: historical/unadjudicated narrative; English source prose.

Use schemas 9, 10, and 11 for the branch choice. Schema 9 is the only page that carries the sequence, so it has to be read again after schemas 10 and 11; an advanced sequence is mixed-page evidence. Schema 5’s u8 names are summary only: its third count still merges stop and other-clear, and its fourth count is the FA7-set store, not callback entry.

## K03942
Source `D0615`, original line17, SHA256 `7a79af2d524bec2a1c8618a98b06082dd6c8b4a05f3181dc6845a1fe3d11b7c3`. Status: historical/unadjudicated narrative; English source prose.

A new kind-5 stamp means `0x785a` completed. `EVENTS_COMPARE[2]`, MODE, BITMODE, PRESCALER, or `CC[2]` still do not prove that store when the stamp is absent. `CC[2]` movement with capture TEP shows a running counter only when those registers are measured. These pages do not include a post-command-19 query.

## K03943
Source `D0616`, original line1, SHA256 `898ec4c8b80cb28c7a767c568e8f62a3578806b578f5e279ea4d38f5153816e4`. Status: historical/unadjudicated narrative; English source prose.

No. This observer can separate those three branches on one later read only when a single signature is present. Mixed counts, a wrapped compare byte, or a register pattern with compare count 0 do not choose a branch. Candidate `833e4bba667889efe41d82d7175282d412609129d6be382c2de5f74ca98fc41e`, 98560 bytes, is offline. `approved_for_ota` is false. It is not installed.

## K03944
Source `D0616`, original line3, SHA256 `898ec4c8b80cb28c7a767c568e8f62a3578806b578f5e279ea4d38f5153816e4`. Status: historical/unadjudicated narrative; English source prose.

The five hooks match the relocated native instructions. `observe21` at `0x8d58` is a BL, restores `r0` and `LR`, and tail-jumps to `0x78f4`, so `0x8d5c` still reaches `0x7c44`. `0x7b50`, `0x7bd4`, and `0x65c0` replay their first four bytes with absolute addresses and resume at `0x7b54`, `0x7bd8`, and `0x65c4`. `0x7760` restores the saved APSR, then repeats `cmp.w r0,#0x144` before `0x7764`. Counters sit in the existing eight-byte reservation at `0x20005154`–`0x20005157`. `profile_boot_entry` and `profile_power_off` store zero through `mode_flag+4`.

## K03945
Source `D0616`, original line5, SHA256 `898ec4c8b80cb28c7a767c568e8f62a3578806b578f5e279ea4d38f5153816e4`. Status: historical/unadjudicated narrative; English source prose.

Blockers for one measured run:

## K03946
Source `D0616`, original line7, SHA256 `898ec4c8b80cb28c7a767c568e8f62a3578806b578f5e279ea4d38f5153816e4`. Status: historical/unadjudicated narrative; English source prose.

- `+4` increments only after the `0x8d40` guard accepts and is about to call `0x78f4`. It is not a `0x94fc` or `0x15d4` result. Page 5 bytes 30–31 are `0x2000066c` and `0x2000067c`, not that mailbox. A zero there means the accepted handler did not enter. The sequence in page 5 byte 26, from bits 6–11 of `0x2000102E`, is what splits “not dispatched” from “dispatched but not entered.”
- `+5` is `0x7b50` entry, not proof that `TASKS_START` at `0x7b8e` executed. `+7` increments on entry to `0x7760` when `r0==0x148`, before `0x785a`. Page 8’s `EVENTS_COMPARE[2]` at `0x40009148`, and page 6’s MODE, BITMODE, PRESCALER, SHORTS, and `CC[2]`, do not prove that callback when `+7` is 0.
- `+6` is one byte for both `0x7bd4` and `0x6588`. Those stores are `ldrb`/`adds`/`strb` with interrupts left enabled, so overlapping clears can drop a count. The byte cannot order a clear against a compare.
- Each counter wraps at 256. At the 31.25 ms compare interval, `+7` can be 0 again after 8 seconds. A zero is “no entry” only inside that window and only before boot or power-off clears `mode_flag+4`.
- If `+6` and `+7` are both nonzero, or `+4` is nonzero while both of the others are zero and `CC[2]` is still 500000, this snapshot has no event order and no timer counter. It cannot choose between “not yet,” “counter not advancing,” and “capture replaced `CC[2]`.” Capture remains a `CC[2]`/`TEP` pattern beside `+7==0`, not a callback.

## K03947
Source `D0618`, original line1, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

### Executive Summary & Candidate Identity

## K03948
Source `D0618`, original line3, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

* **Candidate Image**: `outputs/phase176/candidate10-timer-observer/fw189-dual-mode.bin`
* **Size**: `98,868` bytes (source stock: `96,532` bytes; appended code: `2,332` bytes).
* **SHA256**: `040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d`
* **Current Status**: **UNINSTALLED, UNAPPROVED (`approved_for_ota=false`), OFFLINE**.
* **Active Hardware State**: Tracker installed revision stays `diagnostic09` (`07918813e8...`). Last hardware Phase 172 baseline remains **4,987/5,016 CRC = 99.42185% CONTROL keepalive, IMU = 0**. Target goal remains **90–100% sustained real unique IMU delivery with BOTH controllers healthy** in SteamVR. Tests are offline checks only and are **not** hardware proof.
* **Audit Mode**: READ-ONLY. No edits, no hardware/SSH/RF/SWD/OTA executed.

## K03949
Source `D0618`, original line12, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

### 1. Concrete Audit Findings & Blocker Inventory

## K03950
Source `D0618`, original line24, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

### 2. Independent Challenge of Grok Verdict

## K03951
Source `D0618`, original line26, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

Grok’s verdict in `outputs/phase176/grok-final-review-verbatim.md` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) found "no source blocker for the before/after stamp classifier" and approved the hook semantics, heap reservation, and 9->10->11->9 bracket.

## K03952
Source `D0618`, original line28, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

We independently challenge Grok's verdict on four distinct grounds:

## K03953
Source `D0618`, original line30, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

1. **Grok approved `classify_observation.py` without noticing that line 26 causes silent failure**:
   Grok noted that `CC[2]` movement shows a running counter, but failed to identify that `classify_observation.py` strictly requires `cc2_before` to be passed as a non-None argument. Without it, the classifier cannot return `active_address_capture_gate_closed` and defaults to `unresolved`. The existing proposal and test suite never showed an in-session runner implementation that captures and threads `cc2_before`.
2. **Grok failed to identify the omission of Schema 5 in the classifier**:
   Grok correctly noted in the earlier u8 review that byte 26 (`0x2000102e` bits 6..11) splits "not dispatched" from "dispatched but not entered". Yet in the final review, Grok approved a classifier that completely omits Schema 5. If wire sequence rejection occurs at `0x94fc`, the classifier has no visibility and reports `unresolved`.
3. **Grok’s bracket analysis overlooked peripheral query atomicity**:
   Grok endorsed the 9->10->11->9 sequence, but failed to notice that Schemas 6 (Timer MMIO) and 8 (PPI MMIO) are detached from this bracket. A stable event sequence across 9->10->11->9 does not guarantee that the MMIO snapshots from 6 and 8 were taken while the same clock/counter state was active.
4. **Grok treated "capture starvation" as an unverified external hypothesis**:
   Grok did not trace native `0x7bf0` and `0x7842` to their architectural root. Grok noted that `CC[2]` registers do not prove callback entry, but failed to notice that `0x7842` *already* branches to `0x767c` to enable capture upon compare completion, proving that the preceding call `007bf6: bl #0x767c` inside `0x7bf0` is the architectural defect that starves the compare timer in the presence of continuous RF packets.

## K03954
Source `D0618`, original line41, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

### 3. Detailed Technical Verification

## K03955
Source `D0618`, original line43, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

#### A. Native Hook ABI, APSR & PRIMASK Preservation
* **[STATIC & VERIFIED] Register Preservation**:
  All 5 hook trampolines (`observe21`, `observe_start`, `observe_stop`, `observe_clear`, `observe_compare`) execute:

## K03956
Source `D0618`, original line58, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

* **[STATIC & VERIFIED] Replacement Instruction Equivalence**:
  1. `0x8d58` (`observe21`): Replaced `bl #0x78f4`. Hook stamps Kind 1, jumps via `ldr.w pc, =0x78f5` to `0x78f4` with `LR=0x8d5c` intact. `0x78f4` begins with `cmp r0, #1`, which reads `r0` (preserved `r4` selector) and overwrites APSR immediately.
  2. `0x7b8e` (`observe_start`): Replaced `str r4, [r3]` (`0x40009000.TASKS_START`) and `ldr r3, =0xe000e100`. Hook executes `str r4, [r3]`, stamps Kind 2, pops registers, executes `ldr r3, =0xe000e100`, resumes at `0x7b92` (`mov.w r2, #0x200`).
  3. `0x7be2` (`observe_stop`): Replaced `strb r2, [r3]` (`0x20000fa7 = 0`) and `bx lr`. Hook executes `strb r1, [r3]` (where `r1` has entry `r2 = 0`), stamps Kind 3, pops registers, returns via `bx lr` directly to caller of `0x7bd4`.
  4. `0x65c0` (`observe_clear`): Replaced `strb r4, [r3]` (`0x20000fa7 = 0`) and `ldr r3, =0x2000045c`. Hook executes `strb r4, [r3]`, stamps Kind 4, pops registers, executes `ldr r3, =0x2000045c`, resumes at `0x65c4`.
  5. `0x785a` (`observe_compare`): Replaced `strb r2, [r3]` (`0x20000fa7 = 1`) and `ldrb r3, [r1, #1]`. Hook executes `strb r1, [r3]` (where `r1` has entry `r2 = 1`), stamps Kind 5, pops registers, executes `ldrb r3, [r1, #1]`, resumes at `0x785e` (`add r3, r2`).

## K03957
Source `D0618`, original line65, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

#### B. Completed-Store Event Ordering
* **[STATIC & VERIFIED] Ordering Guarantee**:
  In all cases, `{pre_instruction}` executes **before** the sequence counter (`[r0, #8]`) is incremented and stamped:
  - Kind 2 (`start_completed_sequence` at `0x20005160`) is stamped only **after** `TASKS_START` store executes.
  - Kind 3 (`stop_clear_sequence` at `0x20005164`) is stamped only **after** FA7 is cleared to 0 by `0x7bd4`.
  - Kind 4 (`other_clear_sequence` at `0x2000516c`) is stamped only **after** FA7 is cleared to 0 by `0x6588`.
  - Kind 5 (`gate_set_sequence` at `0x20005168`) is stamped only **after** FA7 is set to 1 by `0x785a`.
  - Kind 1 (`accepted21_sequence` at `0x2000515c`) is stamped only **after** `0x8d40` guard and chip match succeed.
* A stamp value of 0 strictly means the action has not completed since the last reset.

## K03958
Source `D0618`, original line75, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

#### C. Heap Reservation and Reset Behavior
* **[STATIC & VERIFIED] Reservation**:
  - `mode_flag` allocated 32 bytes at `0x20005150 .. 0x2000516f`.
  - Heap start literals at `0x1348` and `0x134c` patched from `0x20005150` to `0x20005170`.
  - Fixed heap end remains `0x20005350`.
  - Available heap is reduced from 512 to 480 bytes (32 bytes consumed).
* **[STATIC & VERIFIED] Reset**:
  Both `profile_boot_entry` (lines 223–230) and `profile_power_off` (lines 306–313) in `dual-mode.S` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) clear all 8 words (32 bytes) at `0x20005150` with zero. Boot and shutdown completely zero the sequence counter and all 5 event stamps.

## K03959
Source `D0618`, original line84, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

#### D. Observer Read-Only Pages
* **[STATIC & VERIFIED] Side-Effect Freedom**:
  - Response packets are assembled in a 32-byte frame on the stack (`sub sp, #32; mov r5, sp`).
  - No writes to RAM outside stack or to MMIO registers occur during query processing.
  - Peripheral reads load `TIMER1` and `PPI` status registers without clearing events: in nRF52, `0x40009148` (`EVENTS_COMPARE[2]`) is read-only unless 0 is explicitly written to it.
  - All 32-bit fields are written to unaligned stack offsets (22 and 26) using 4 sequential `strb` byte writes, strictly preventing `UsageFault` under `CCR.UNALIGN_TRP`.

## K03960
Source `D0618`, original line91, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

#### E. Classifier Sufficiency & Discrimination
* **[STATIC & VERIFIED] Discrimination Matrix**:
  - If `pages[10]['fa7'] == 0` and `stop_clear > max(start, set_gate)`: Returns `'native_stop'` (Kind 3).
  - If `pages[10]['fa7'] == 0` and `other_clear > max(start, set_gate)`: Returns `'other_native_clear'` (Kind 4).
  - If `pages[10]['fa7'] == 1` and `set_gate > max(stop, other)`: Returns `'gate_open'` (Kind 5).
  - If `pages[10]['fa7'] == 0`, `start > clear`, `capture` is true, and `advancing` is true: Returns `'active_address_capture_gate_closed'`.
* **[STATIC & VERIFIED] Limitation**:
  If `cc2_before` is omitted, or if Command 21 is dropped by `0x94fc` sequence check before `0x8d40`, the classifier returns `unresolved`, with no diagnostic distinction.

## K03961
Source `D0618`, original line102, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

### 4. Sufficiency for ONE Evidence-Backed Intervention

## K03962
Source `D0618`, original line104, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

**Is the planned measurement sufficient for ONE evidence-backed intervention?**
**YES**, provided the two execution blind spots (B1 and B2) are addressed by the host runner.

## K03963
Source `D0618`, original line107, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

The measurement cleanly separates the problem space into mutually exclusive physical branches, each pointing to exactly ONE minimal intervention:

## K03964
Source `D0618`, original line122, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

#### Detailed Minimal Remedies:

## K03965
Source `D0618`, original line124, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

1. **If Branch 1 (`accepted21 == 0`)**:
   - *Evidence*: Command 21 was sent, but Hook 1 at `0x8d58` never stamped.
   - *Minimal Remedy*: Read Schema 5 byte 26 to discover the live expected sequence from `0x2000102e`. Send Command 21 using that exact sequence number.
2. **If Branch 2 (`native_stop`)**:
   - *Evidence*: Hook 3 at `0x7be2` stamped after Hook 2.
   - *Minimal Remedy*: Native shutdown `0x7bd4` was called. Check Schema 9 byte 31 (`0x20001f99`) and Schema 7 bytes 30..31 (`0x20000e10`/`0x20000e48`) to identify which caller invoked `0x7bd4` and remove the disconnect/producer flag condition.
3. **If Branch 3 (`other_native_clear`)**:
   - *Evidence*: Hook 4 at `0x65c0` stamped after Hook 2.
   - *Minimal Remedy*: Command 10 (`0x8d88`) failed chip ID matching and branched to `0x6588`. Cease sending Command 10 or correct its 4-byte chip payload to `[TRACKER_ID]`.
4. **If Branch 4 (`active_address_capture_gate_closed`)**:
   - *Evidence*: Hook 2 stamped, Hook 5 never stamped, PPI channel 3 is active, and `CC[2]` is moving.
   - *Minimal Remedy (Non-flash)*: After Command 21 transport ACK, host introduces a single quiet pause of **40 ms** (exceeding 31.25 ms / 500,000 ticks) before resuming radio polling. This allows TIMER1 to count to 500,000 without `RADIO.EVENTS_ADDRESS` overwriting `CC[2]`, triggering `EVENTS_COMPARE[2]` at `0x7760` and executing `0x785a` to set `FA7 = 1`.
   - *Minimal Remedy (Native firmware)*: Replace `007bf6: bl #0x767c` (4 bytes at `0x7bf6`) with `nop; nop` in `0x7bf0`. Because `0x7842` already executes `b.w #0x767c` after setting `FA7 = 1`, capture will only activate *after* the compare callback has completed, completely eliminating the starvation race condition.

## K03966
Source `D0618`, original line140, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

### 5. Concrete Recommendations for Pre-OTA Validation Gate

## K03967
Source `D0618`, original line142, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

Before any request to the user to authorize flashing candidate 10:

## K03968
Source `D0618`, original line144, SHA256 `c7e5e117732148f6ad07832f3a1e3c3ba8e2d1d28c6975aadb4f741826ecdd12`. Status: historical/unadjudicated narrative; English source prose.

1. **Fix Host Classifier Integration**:
   Update the runner script to:
   - Query Schema 6 immediately before the 9->10->11->9 bracket.
   - Query Schema 6 again immediately after.
   - Pass `cc2_before = first_schema6['cc2']` into `classify()`.
2. **Include Schema 5 in Diagnostic Evaluation**:
   Add Schema 5 parsing to the diagnostic report to capture `command21_expected_sequence` (byte 26) and queue occupancy (bytes 30..31) in case Hook 1 does not advance.
3. **Strict Adherence to Contract**:
   Keep candidate 10 offline and uninstalled until the user explicitly reviews these findings and decides whether to test the 40-ms quiet pause on the currently installed `diagnostic09` image first, or authorize candidate 10 OTA for verified diagnostic observability.

## K03969
Source `D0619`, original line1, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

The revision-10 image is still an uninstalled, unapproved diagnostic observer. The prepared service38 package does not keep OTA closed, and the classifier does not enforce identity or a real before/after bracket. Do not flash it. Phase 172 remains 99.42185% control CRC with zero IMU; nothing in this package is hardware proof of 90–100% IMU with both controllers.

## K03970
Source `D0619`, original line3, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

## Launch blockers

## K03971
Source `D0619`, original line5, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

**1. The stored command can pass clearance while `job.json` still says unapproved.** Verified.

## K03972
Source `D0619`, original line7, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

`outputs/phase176/ota-proposal/job.json` has `preparation_only: true` and `approved_for_ota: false`, and its `sha256` is `040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d`. `outputs/phase176/ota-proposal/invocation.json` already contains `--authorize-unverified-fork` and does not contain `--pause-before-ota-start`.

## K03973
Source `D0619`, original line9, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

`load_job` replaces the on-disk flag with its argument, which defaults to false:

## K03974
Source `D0619`, original line16, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

The loader calls it with only the CLI flag:

## K03975
Source `D0619`, original line24, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

Clearance then accepts that overwritten job. `approved_for_ota` is never read:

## K03976
Source `D0619`, original line37, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

Because the authorize flag is present, the early no-job block at `roy_ram_loader.py` lines 445–450 is skipped. The same isolated whitelist also still admits `07918813…` (98032 bytes), `40941b34…` (97832), and `395ce84b…` (97320) at lines 23–29. The main tree `pc/radio/tracker_ota_readiness.py` does not contain `040b304d…` or `07918813…`.

## K03977
Source `D0619`, original line39, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

Minimal remedy: `load_job` must reject a disk job whose `preparation_only` is true or whose `approved_for_ota` is false, and must not overwrite those fields. Keep `--authorize-unverified-fork` out of the stored argv until the runner adds it. Admit only `040b304d…` / 98868 bytes in this copy.

## K03978
Source `D0619`, original line41, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

**2. The runner’s approval check is a single boolean and does not pin the image.** Verified.

## K03979
Source `D0619`, original line51, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

Unread: `approved_for_ota`, `job['sha256']`, `invocation['image_sha256']`, and `image.bin`. The hash check covers five Python files. Any other argv that still has the authorize flag and omits pause will be executed. `run_after_approval_template.py` and `ota-proposal/run_after_approval.py` match in full; the generator copies the template at `prepare_ota_proposal.py` line 44.

## K03980
Source `D0619`, original line53, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

Minimal remedy: require `approved_for_ota is True`, require the job, invocation, and `image.bin` digests to equal `040b304d…`, and require `cmd` to equal one frozen argv.

## K03981
Source `D0619`, original line55, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

**3. “Fresh nonce / activation / mode / controllers / profile” is a trusted JSON blob.** Verified. `prestart-proof.json` is absent, so this runner cannot start today. The checks it will accept are booleans plus a 60-second `verified_unix`:

## K03982
Source `D0619`, original line65, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

There is no nonce, reply bytes, or running-image SHA. `diagnostic_activation_verified` does not name installed `07918813…`. `TrackerOtaRuntime._poll` checks live FW189 and serial `[TRACKER_SERIAL]` (`tracker_ota_runtime.py` lines 232–234) and does not check mode, N, or pending. Controller-off is the boolean plus `station-config.json` `controllers_off: true`; that config does match `validate_service27_plan` (cycle 1, N 0, M 2, 900 µs / 4727 µs, optics off, peers 1 and 2 paused). That is a static profile match, not a fresh service38 admission.

## K03983
Source `D0619`, original line67, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

Minimal remedy: the proof must carry the raw mode reply, nonce, chip, and running SHA, and the runtime must reject the sender until mode 0 / N 0 / pending 0 is read from that reply in the same session.

## K03984
Source `D0619`, original line69, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

**4. Replay control exists; the attempt directory does not.** Verified. `load_job` rejects an existing result (`tracker_ota_runtime.py` line 66) and `TrackerOtaRuntime.__init__` creates it with `open('x')` (lines 160–161). The runner also refuses an existing `ota-result.json` and does not loop. `stdout.log` is created with `'x'` only if `phase176/ota-proposal/attempt` already exists. The generator never creates that directory. A failed start that exits before the result file is created can be invoked again; the script does not do that by itself.

## K03985
Source `D0619`, original line71, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

## Classifier

## K03986
Source `D0619`, original line73, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

`observer_protocol.parse` (`observer_protocol.py` lines 11–16) rejects a wrong length, schema, 16-byte nonce, or 4-byte chip. It does not check a radio CRC. `classify()` never calls `parse` and has no identity input:

## K03987
Source `D0619`, original line85, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

`sequence_before` and `cc2_before` are caller numbers, not a second schema-9 or schema-6 page. The Phase 176 statement that this file requires identity, nonce, and CRC describes a comment, not a check.

## K03988
Source `D0619`, original line87, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

Two bracket results are wrong when the stamps disagree:

## K03989
Source `D0619`, original line89, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

- Lines 17–20: if `stop_clear_sequence == other_clear_sequence` and that value is strictly after START and SET, the result is `other_native_clear`. Equal stamps are unordered.
- Lines 21–30: if page 10 `fa7==0` but page 11 `gate_set_sequence` is strictly newer than both clears, the SET branch is skipped. With `start > clear`, capture channel 3, EEP `0x40001104`, TEP `0x40009048`, CHEN bit 3, and a changed CC2, the result is `active_address_capture_gate_closed`.

## K03990
Source `D0619`, original line92, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

`test_classification.py` covers unequal sequence, a later stop, a later other-clear, and `gate_open` still needing a producer. It does not cover an equal clear pair, an `fa7`/SET contradiction, or a foreign nonce. `gate_open` is returned without a producer check; the reason string only mentions one.

## K03991
Source `D0619`, original line94, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

Minimal remedy: return `unresolved` on a clear-stamp tie and whenever live FA7 disagrees with the newer of SET versus clear. Require one nonce and chip across pages 6, 8, 9, 10, and 11, plus a second schema-9 sequence inside the input. Leave CRC with the radio layer that actually has it.

## K03992
Source `D0619`, original line96, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

`0x40001104` is RADIO `EVENTS_ADDRESS` and `0x40009000` is TIMER1. TEP `0x40009048` is `TASKS_CAPTURE[2]`. Those constants match the capture branch.

## K03993
Source `D0619`, original line98, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

## Grok’s hook verdict

## K03994
Source `D0619`, original line100, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

The five wrappers in `candidate10-timer-observer/dual-mode.S` match the replaced halfwords in `patch-manifest.json`:

## K03995
Source `D0619`, original line110, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

`mov r1,r2` before the stop and compare stores is what keeps original `r2` after `PRIMASK` reuses `r2`. Push/pop covers `r0`–`r3`, `r12`, and `lr`; APSR is saved and restored. Heap literals `0x1348` and `0x134c` change from `0x20005150` to `0x20005170` (32 bytes). `0x20005170 + 480` and `0x20005150 + 512` are the same end. The pool constants `0xe000e100` and `0x2000045c` are hardcoded in the wrapper; this pass did not re-read them from stock flash.

## K03996
Source `D0619`, original line112, SHA256 `eeb1b825d428836a21bdc1e919553b42a08e23b0ea73487a11586a31c6ded702`. Status: historical/unadjudicated narrative; English source prose.

Grok’s “no source blocker” applies to that stamp mechanism. It does not cover the Python tie / FA7 contradiction above, and it does not make the OTA job a closed gate.

## K03997
Source `D0620`, original line1, SHA256 `b1311bc22ac673941eb6e2faeeaf6c9d99642c0bab1aa9c634523d1adc72b67f`. Status: historical/unadjudicated narrative; English source prose.

### Verdict: Concrete Observer Findings Are CLOSED

## K03998
Source `D0620`, original line3, SHA256 `b1311bc22ac673941eb6e2faeeaf6c9d99642c0bab1aa9c634523d1adc72b67f`. Status: historical/unadjudicated narrative; English source prose.

All previously identified observer defects are **fully closed** in the current source:

## K03999
Source `D0620`, original line5, SHA256 `b1311bc22ac673941eb6e2faeeaf6c9d99642c0bab1aa9c634523d1adc72b67f`. Status: historical/unadjudicated narrative; English source prose.

1. **Dual Timer Configuration Invariance (Closed)**:
   In `verified_observation.py` L44-49 (original reference: `[PRIVATE_SOURCE_REFERENCE]`), both Schema 6 samples are explicitly preserved (`records[2]` and `records[6]`) and checked across `('timer_mode', 'timer_bitmode', 'timer_prescaler', 'shorts_low8', 'inten_compare_low8', 'capture_channel')`. Any drift immediately aborts with `'Timer configuration changed across samples'`.
2. **Bracket Order & Status Page Equality (Closed)**:
   In `verified_observation.py` L43-47 (original reference: `[PRIVATE_SOURCE_REFERENCE]`), initial and trailing Schema 9 records are explicitly compared across `event_sequence`, `accepted21_sequence`, and `last_event_kind`, preventing unobserved state transitions across the query window.
3. **Pairwise Stamp Uniqueness Across All 5 Actions (Closed)**:
   In `classify_observation.py` L17-19 (original reference: `[PRIVATE_SOURCE_REFERENCE]`), all five nonzero stamps (`start`, `stop`, `set_gate`, `other`, `accepted21_sequence`) must satisfy `len(stamps) == len(set(stamps))`. Any duplicate nonzero stamp is rejected as `'Distinct actions share a nonzero event stamp'`.
4. **Canonical FA7 & Gate SET Provenance (Closed)**:
   In `classify_observation.py` L20-21 (original reference: `[PRIVATE_SOURCE_REFERENCE]`), `pages[10]['fa7'] not in (0, 1)` or `pages[10]['fa7'] == 1` with `set_gate == 0` is strictly rejected as `'FA7 lacks a canonical completed SET'`.
5. **Neutral CC2 Branch Semantics (Closed)**:
   In `classify_observation.py` L38-39 (original reference: `[PRIVATE_SOURCE_REFERENCE]`), the branch is renamed to `cc2_changed_with_capture_configured_gate_closed`, explicitly declaring that clock progression, capture causality, and capture starvation remain unproven.
6. **Overclaims Rejected**:
   Unconditional 40 ms quiet window (counterexample proven by `SHORTS` `COMPARE0_CLEAR`) and speculative native NOP at `0x7bf6` remain rejected and excluded from all host plans.

## K04000
Source `D0620`, original line20, SHA256 `b1311bc22ac673941eb6e2faeeaf6c9d99642c0bab1aa9c634523d1adc72b67f`. Status: historical/unadjudicated narrative; English source prose.

### Remaining Source Blockers in Host Observer Package
