# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K04001
Source `D0620`, original line22, SHA256 `b1311bc22ac673941eb6e2faeeaf6c9d99642c0bab1aa9c634523d1adc72b67f`. Status: historical/unadjudicated narrative; English source prose.

**ZERO source blockers remain** in `verified_observation.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`), `classify_observation.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`), and `observer_protocol.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`).

## K04002
Source `D0620`, original line24, SHA256 `b1311bc22ac673941eb6e2faeeaf6c9d99642c0bab1aa9c634523d1adc72b67f`. Status: historical/unadjudicated narrative; English source prose.

The host observer classification package is complete, conservative, and mathematically sound. As required by project rules, candidate image `040b304d...` remains uninstalled and unapproved offline (`approved_for_ota=false`). No OTA or hardware execution is authorized.

## K04003
Source `D0621`, original line1, SHA256 `f1ef5a080cede4fa72a9761c39c5bdecd50cff15240d2d70ccb3e313a7c91937`. Status: historical/unadjudicated narrative; English source prose.

### Final Host Audit Verdict & Observer Status

## K04004
Source `D0621`, original line3, SHA256 `f1ef5a080cede4fa72a9761c39c5bdecd50cff15240d2d70ccb3e313a7c91937`. Status: historical/unadjudicated narrative; English source prose.

**Candidate Image**: `outputs/phase176/candidate10-timer-observer/fw189-dual-mode.bin` (98,868 B, SHA256 `040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d`).
**State**: **UNINSTALLED, UNAPPROVED (`approved_for_ota=false`), OFFLINE**. Hardware baseline remains CONTROL 99.42185%, IMU = 0.

## K04005
Source `D0621`, original line8, SHA256 `f1ef5a080cede4fa72a9761c39c5bdecd50cff15240d2d70ccb3e313a7c91937`. Status: historical/unadjudicated narrative; English source prose.

### 1. Corrected Architectural & Dispatch Facts

## K04006
Source `D0621`, original line10, SHA256 `f1ef5a080cede4fa72a9761c39c5bdecd50cff15240d2d70ccb3e313a7c91937`. Status: historical/unadjudicated narrative; English source prose.

1. **`TIMER1 + 0x200` is `SHORTS`, not `TASKS_CLEAR`**:
   `0x40009200` is the `SHORTS` shortcut register. Writing `1` (bit 0) enables `COMPARE[0]_CLEAR` (clearing `COUNTER` automatically on `COMPARE[0]`), which executes only if that specific native branch path is selected. Writing `0` to `0x40009200` disables shortcuts. Actual `TASKS_CLEAR` is at offset `+0x00c` (`0x4000900c`).
2. **Native Dispatcher Mapping (`0x901c`)**:
   QueueFirmware ground-truth confirms `0x901c` maps wire Command 9 to handler `0x8d88` (`0x8d89`) and Command 10 to handler `0x8df0` (`0x8df1`), not vice versa.
3. **Modular Delta Equivalence**:
   For unsigned 32-bit values in Python, `((after - before) & 0xffffffff) > 0` is mathematically identical to `after != before`. Neither inequality proves forward monotonic progression or hardware capture provenance—it only establishes that the register value was not identical across queries.
4. **Sequence Bracket Verification**:
   `classify()` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) explicitly checks `if sequence_before != sequence_after: return 'unresolved'`, which guards the bracket against state changes, resets, or sequence wraps. The check `order['event_sequence'] != sequence_after` on the trailing page is redundant, but does not constitute an execution bypass.
5. **Dynamic Schema 3 Tail**:
   Bytes 22..31 of Schema 3 are volatile RAM sensor snapshot fields (`0x20000442`, `0x20001fa0`, etc.) and vary dynamically at runtime; they cannot be compared against a static image hash. Live admission correctly verifies structure, length (32 B), and valid flag bounds (`b[24] in (0, 1)`).

## K04007
Source `D0621`, original line23, SHA256 `f1ef5a080cede4fa72a9761c39c5bdecd50cff15240d2d70ccb3e313a7c91937`. Status: historical/unadjudicated narrative; English source prose.

### 2. Observer Findings Final Status

## K04008
Source `D0621`, original line25, SHA256 `f1ef5a080cede4fa72a9761c39c5bdecd50cff15240d2d70ccb3e313a7c91937`. Status: historical/unadjudicated narrative; English source prose.

1. **First/Last Timer Configuration Mismatch (Verified Code Issue)**:
   `verified_observation.py` L42 (original reference: `[PRIVATE_SOURCE_REFERENCE]`) builds `pages = {r['schema']: r['fields'] for r in self.records}` across `PLAN=(9, 5, 6, 8, 10, 11, 6, 9)`. The second Schema 6 query overwrites `pages[6]`.
   * *Status*: Verified defect in host dictionary collation. While `cc2_before` is explicitly retrieved from `records[2]`, the first query's timer control registers (`timer_mode`, `timer_bitmode`, `timer_prescaler`, `shorts_low8`, `capture_channel`) are overwritten by the second query. The classifier checks mode `(0, 3, 0)` only on the second sample.
   * *Remedy*: Pass `timer_before` and `timer_after` distinctly; verify configuration invariance across both samples before evaluating `cc2` deltas.
2. **Pairwise Nonzero Action Stamp Uniqueness**:
   `classify_observation.py` L18 (original reference: `[PRIVATE_SOURCE_REFERENCE]`) checks `stop == other and stop != 0`, but does not verify pairwise uniqueness across all nonzero action stamps (`start`, `stop`, `set_gate`, `other`).
   * *Status*: Incomplete contradiction check. If memory corruption produces equal nonzero stamps across SET and CLEAR, the logic falls through to generic `unresolved` rather than flagging an impossible native action sequence.
   * *Remedy*: Enforce `len(stamps) == len(set(stamps))` for all nonzero stamps.
3. **Contradictory Gate State with Zero Actions**:
   If `FA7 == 1` while `set_gate == 0`, native `0x785a` has never completed since reset. Line 20 misses this contradiction because `0 > 0` is false, falling through to generic `unresolved`.
   * *Remedy*: Explicitly reject `pages[10]['fa7'] == 1 and set_gate == 0`.

## K04009
Source `D0621`, original line39, SHA256 `f1ef5a080cede4fa72a9761c39c5bdecd50cff15240d2d70ccb3e313a7c91937`. Status: historical/unadjudicated narrative; English source prose.

### 3. Rejection of Unmeasured Overclaims

## K04010
Source `D0621`, original line41, SHA256 `f1ef5a080cede4fa72a9761c39c5bdecd50cff15240d2d70ccb3e313a7c91937`. Status: historical/unadjudicated narrative; English source prose.

* **Capture Starvation is Unproven**: Candidate 10 with native event hooks has never been run on physical hardware; installed hardware remains `diagnostic09`. Attributing `FA7 == 0` to continuous 6 ms RF capture starvation over alternative causes (upstream sequence rejection at `0x94fc`, queue exhaustion at `0x15d4`, or software shutdown at `0x7bd4`/`0x6588`) is an unmeasured hypothesis.
* **40 ms Quiet Does Not Guarantee Compare**: If native `SHORTS` enables `COMPARE[0]_CLEAR`, `TIMER1` clears periodically at the frame period (3 ms or 6 ms), preventing the counter from reaching 500,000 ticks (31.25 ms). Furthermore, a 40 ms silence risks triggering link-loss disconnect (`0x64fe -> 0x7bd4`).
* **Native NOP at `0x7bf6` is Unjustified**: Modifying `0x7bf6` violates the rule against unmeasured firmware patching. If `0x7842` is never reached, PPI channel 3 is never armed, permanently breaking radio packet timestamp capture.

## K04011
Source `D0621`, original line45, SHA256 `f1ef5a080cede4fa72a9761c39c5bdecd50cff15240d2d70ccb3e313a7c91937`. Status: historical/unadjudicated narrative; English source prose.

### Verdict
The host package repairs address critical approval, argv, and raw evidence validation. Candidate 10 remains an offline, uninstalled read-only observer. Flashing or hardware testing is blocked pending explicit user authorization.

## K04012
Source `D0622`, original line1, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

Error calling tool 'antigravity_ask': agy hit its 600s timeout before finishing, so it returned only part of an answer. Raise timeout_s, or split the task into smaller ones.

## K04013
Source `D0622`, original line3, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

--- partial answer, cut off mid-turn (15585 chars) ---
I am locating the relevant source files and documentation for the Phase 178 audit. I will inspect them as soon as the listing returns.
### Executive Audit Status & Candidate Identity

## K04014
Source `D0622`, original line7, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

* **Candidate Image**: `outputs/phase176/candidate10-timer-observer/fw189-dual-mode.bin`
* **Size**: `98,868` bytes (SHA256: `040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d`).
* **Hardware Status**: **UNINSTALLED, UNAPPROVED (`approved_for_ota=false`), OFFLINE**. Installed revision remains `diagnostic09` (`07918813e8...`). Last hardware Phase 172 baseline remains **CONTROL 99.42185%, IMU = 0**. Offline unit tests do not prove hardware delivery.
* **Audit Mode**: **READ-ONLY**. No source code edits, no shell execution of firmware/runner/generator, no hardware/SSH/RF/SWD/OTA operations.

## K04015
Source `D0622`, original line12, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

The detailed planning artifact has been prepared at phase178_host_audit_and_verification_plan.md (original reference: `[PRIVATE_SOURCE_REFERENCE]`).

## K04016
Source `D0622`, original line16, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

### 1. Concrete Code-Proven Findings

## K04017
Source `D0622`, original line18, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

#### Finding 1: Dictionary Key Overwrite of Multi-Sample Pages (Schemas 6 and 9)
* **File & Line**: `outputs/phase176/verified_observation.py` L42 (original reference: `[PRIVATE_SOURCE_REFERENCE]`)
* **Trigger**: `PLAN = (9, 5, 6, 8, 10, 11, 6, 9)` contains two Schema 6 queries (indices 2 and 6) and two Schema 9 queries (indices 0 and 7). Line 42 constructs `pages` using a dictionary comprehension:

## K04018
Source `D0622`, original line24, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

* **Effect**: The second Schema 6 sample (`self.records[6]`) silently overwrites the first Schema 6 (`self.records[2]`) at `pages[6]`, and the second Schema 9 (`self.records[7]`) overwrites the first Schema 9 at `pages[9]`.
  1. The first Schema 6 timer configuration registers (`timer_mode`, `timer_bitmode`, `timer_prescaler`, `shorts_low8`, `capture_channel`) are completely lost. In `classify_observation.py` L32 (original reference: `[PRIVATE_SOURCE_REFERENCE]`), `classify()` only verifies `(timer['timer_mode'], timer['timer_bitmode'], timer['timer_prescaler']) == (0, 3, 0)` on the second sample. If timer configuration or capture channel changed or was corrupted between query 2 and query 6, `advancing` compares `timer['cc2']` with a `cc2_before` taken from an unverified timer state.
  2. The first Schema 9's status fields (`accepted21_sequence`, `last_event_kind`, `calibration_state_1f99`) are discarded from `pages`.
* **Minimal Fix**: Pass separate, unoverwritten objects to `classify()` (e.g. `timer_before=self.records[2]['fields'], timer_after=self.records[6]['fields']`), and verify in `classify()` that timer mode, bitmode, prescaler, and capture channel were identical `(0, 3, 0, 3)` across both samples.

## K04019
Source `D0622`, original line31, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

#### Finding 2: CC2 Movement Semantics & Test Fixture Blind Spot
* **File & Line**: `outputs/phase176/classify_observation.py` L30 (original reference: `[PRIVATE_SOURCE_REFERENCE]`) & `outputs/phase176/test_audit_repairs.py` L135 (original reference: `[PRIVATE_SOURCE_REFERENCE]`)
* **Trigger**: `advancing = cc2_before is not None and timer['cc2'] != cc2_before`.
* **Effect**:
  1. In `test_audit_repairs.py` (L135), the test fixture generates mock Schema 6 packets with `data = bytearray(10)` where `cc2` bytes 4..7 are `0`. Both Schema 6 records have `cc2 = 0`, so `timer['cc2'] == cc2_before == 0`. As a result, `advancing` evaluates to `False`, and line 31 never fires. The offline tests never actually validate the `active_address_capture_gate_closed` branch against valid progression data.
  2. On hardware, checking `!=` treats any change as forward progression. If `TIMER1` is periodically reset by `COMPARE[0]` (e.g. every 3 ms or 6 ms frame period), `CC[2]` captures modulo the frame period. A simple `!=` check fails to confirm whether `CC[2]` advanced positively mod 2^32 or was overwritten by software (e.g. `0x7b50` setting `CC[2] = 500,000`).
* **Minimal Fix**: Check positive modular progression: `(timer['cc2'] - cc2_before) & 0xffffffff > 0`, and update test fixtures in `test_audit_repairs.py` to supply distinct, nonzero `CC[2]` values when validating capture branch classification.

## K04020
Source `D0622`, original line41, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

#### Finding 3: Incomplete Action Stamp Inequality Checks
* **File & Line**: `outputs/phase176/classify_observation.py` L18-21 (original reference: `[PRIVATE_SOURCE_REFERENCE]`)
* **Trigger**: Line 18 only checks `stop == other and stop != 0`. It omits checks for `set_gate == stop != 0`, `set_gate == other != 0`, or `start == stop != 0`.
* **Effect**: If memory corruption or uninitialized RAM causes `set_gate == stop != 0`:
  `clear = max(stop, other) = set_gate`.
  Line 20 checks `(set_gate > clear and pages[10]['fa7'] != 1) or (clear > set_gate and pages[10]['fa7'] != 0)`.
  Because `set_gate == clear`, both inequalities evaluate to `False`. Line 20 fails to detect the physical impossibility that a SET and a CLEAR action completed with the identical sequence number. While it falls through to line 35 (`unresolved`), it reports `'No supported distinguishing signature'` instead of flagging a contradictory stamp state.
* **Minimal Fix**: Enforce pairwise uniqueness across all nonzero action stamps:

## K04021
Source `D0622`, original line57, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

#### Finding 4: Contradictory FA7 Gate State when Actions are Zero
* **File & Line**: `outputs/phase176/classify_observation.py` L20 (original reference: `[PRIVATE_SOURCE_REFERENCE]`)
* **Trigger**: `pages[10]['fa7'] == 1` while `set_gate == 0` and `clear == 0`.
* **Effect**: In native firmware, `0x785a` is the sole instruction that sets `0x20000fa7 = 1`. If `set_gate == 0`, `0x785a` has never completed since boot/reset. Line 20 checks `set_gate > clear` (0 > 0 is False) and `clear > set_gate` (0 > 0 is False). It fails to flag the contradiction that `FA7` is set without any completed gate SET action, falling through to generic `unresolved`.
* **Minimal Fix**: Add explicit check:

## K04022
Source `D0622`, original line69, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

#### Finding 5: Order Page Self-Comparison Tautology
* **File & Line**: `outputs/phase176/classify_observation.py` L8-10 (original reference: `[PRIVATE_SOURCE_REFERENCE]`)
* **Trigger**: Line 8 sets `order = pages[9]`. Because of the overwrite in Finding 1, `pages[9]` is `self.records[-1]['fields']`.
* **Effect**: Line 9 executes `if order['event_sequence'] != sequence_after:`. Since `sequence_after` is also extracted from `self.records[-1]['fields']['event_sequence']`, this check compares an object reference with itself. The condition can never evaluate to `True`.
* **Minimal Fix**: Verify that `sequence_before == self.records[0]['fields']['event_sequence']` and `sequence_after == self.records[-1]['fields']['event_sequence']`, and ensure the bracketed check explicitly verifies that `pages[9_first]['event_sequence'] == pages[9_second]['event_sequence']`.

## K04023
Source `D0622`, original line77, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

### 2. Native Observer10 Hooks and Pages Audit

## K04024
Source `D0622`, original line79, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

Forensic inspection of `dual-mode.S` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) and `build_observer.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):

## K04025
Source `D0622`, original line81, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

1. **Register & Flag Preservation**:
   All 5 hook trampolines (`observe21`, `observe_start`, `observe_stop`, `observe_clear`, `observe_compare`) execute:

## K04026
Source `D0622`, original line95, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

APSR flags, PRIMASK interrupt state, SP 8-byte alignment, and LR are preserved. The replaced instructions execute inside the interrupt-masked window **before** the stamp write.
2. **Replacement Instructions**:
   - `0x8d58` (`observe21`): Replaced `bl #0x78f4`. Resumes at `0x78f5` via `ldr.w pc, =0x78f5` with `LR=0x8d5c` intact.
   - `0x7b8e` (`observe_start`): Replaced `str r4, [r3]` (`0x40009000.TASKS_START`) and `ldr r3, =0xe000e100`. Executes store, stamps Kind 2, restores NVIC literal, resumes at `0x7b93`.
   - `0x7be2` (`observe_stop`): Replaced `strb r2, [r3]` (`0x20000fa7 = 0`) and `bx lr`. Executes `strb r1, [r3]` (storing 0), stamps Kind 3, returns via `bx lr`.
   - `0x65c0` (`observe_clear`): Replaced `strb r4, [r3]` (`0x20000fa7 = 0`) and `ldr r3, =0x2000045c`. Executes store, stamps Kind 4, restores literal, resumes at `0x65c5`.
   - `0x785a` (`observe_compare`): Replaced `strb r2, [r3]` (`0x20000fa7 = 1`) and `ldrb r3, [r1, #1]`. Executes `strb r1, [r3]` (storing 1), stamps Kind 5, executes `ldrb r3, [r1, #1]`, resumes at `0x785f`.
3. **Query Handler Stack Safety**:
   Responses for Schemas 5..11 assemble on the stack (`sub sp, #32; mov r5, sp`). Peripheral MMIO reads load `TIMER1` (`0x40009xxx`) and `PPI` (`0x4001fxxx`) status registers without clearing events or writing to RAM outside the stack frame. Unaligned 32-bit fields (offsets 22 and 26) are serialized using 4 sequential `strb` byte writes, strictly preventing `UsageFault` under `CCR.UNALIGN_TRP`.

## K04027
Source `D0622`, original line107, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

### 3. Real Serialization, Checksum & DMA Body Format

## K04028
Source `D0622`, original line109, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

1. **Wire Record Framing**:
   In `tracker_ota_runtime.py` L213-216 (original reference: `[PRIVATE_SOURCE_REFERENCE]`):
   - Wire DMA buffer layout is validated: `dma[0]` is length, `dma[1]` is S1 (`in (2, 3)`), and `len(dma) == dma[0] + 2`.
   - Swift light_sync records are decoded via `decode_records(dma[2:])`.
   - Command 1 records contain a 32-byte body: `b'\xfe' + schema (1B) + nonce (16B) + chip_id_le (4B) + data (10B)`.
2. **Live Admission Query Integration**:
   In `ota_live_admission.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):
   - Schema 3 capability is queried first, verifying length 32 and byte 24 `in (0, 1)`.
   - Schema 2 mode is queried second, strictly requiring mode 0, revision 7, N 0, pending 0 (`b[22:] == bytes([0, 7, 0, 0])`).
   - In `peer_engine.py` L246-247 (original reference: `[PRIVATE_SOURCE_REFERENCE]`), `ack_tag` immediately returns `False` when `ota_admission_active` is `True`. MCU transport ACKs cannot retire the pending query; only the matching application reply in `receive()` clears `engine.pending` and advances sequence `engine.sequences[1]`.
   - When transitioning to `OtaSession`, `sequence = engine.sequences[1]` maintains unbroken command sequencing.

## K04029
Source `D0622`, original line123, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

### 4. Critical Re-Evaluation of Prior Phase 177 Assertions

## K04030
Source `D0622`, original line125, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

#### A. Assertion 1: "Capture starvation is proven"
* **Status**: **UNPROVEN / INFERRED HYPOTHESIS ONLY**.
* **Native State Requirements**:
  For capture starvation to occur:
  1. Command 21 must be accepted by `0x8d40` (chip ID matches).
  2. `0x7c44` must call `0x7bf0`, executing `0x7b8e` to start `TIMER1` with `CC[2] = 500,000` (Hook 2 stamps).
  3. PPI channel 3 must be enabled connecting `RADIO.EVENTS_ADDRESS` (`0x40001104`) to `TIMER1.TASKS_CAPTURE[2]` (`0x40009048`).
  4. Incoming RF packet rate (every 6 ms) must trigger `TASKS_CAPTURE[2]` repeatedly, overwriting `CC[2]` before `COUNTER` reaches 500,000 (31.25 ms).
  5. Software shutdown (`0x7bd4`, Hook 3) and mismatch clear (`0x6588`, Hook 4) must not execute.
  6. `TIMER1` counter must not be cleared by other compare events before 31.25 ms.
* **Explicit Disproofs & Unknowns**:
  In Phase 172, physical hardware ran `diagnostic09`, which contains **zero native hooks**. We have zero measurements of whether Command 21 was accepted (`0x8d58`), whether `0x94fc` rejected the sequence, whether the queue saturated, or whether software shutdown occurred. Declaring capture starvation "proven" without hardware hook data is an unmeasured overclaim.

## K04031
Source `D0622`, original line140, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

#### B. Assertion 2: "40 ms quiet guarantees 500,000 compare match"
* **Status**: **FALSE / DISPROVED BY NATIVE CODE ARCHITECTURE**.
* **Architectural Disproof**:
  In `swift189-full.asm` L10210-10214 (`0x77c0-0x77c6`) (original reference: `[PRIVATE_SOURCE_REFERENCE]`):

## K04032
Source `D0622`, original line148, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

Offset `0x200` in `TIMER1` (`0x40009000`) is `TASKS_CLEAR`. When `EVENTS_COMPARE[0]` fires, it triggers `TASKS_CLEAR`, resetting `TIMER1.COUNTER` to 0.
  In Frame mode, `CC[0]` is configured to the frame period (nominally 1810.5 us, 3000 us, or 6000 us = 28,968 to 96,000 ticks). If `TIMER1` resets every 3 ms or 6 ms, `COUNTER` counts to 48,000 or 96,000 and **resets to 0**.
  It will **never reach 500,000 ticks (31.25 ms)**, regardless of whether the radio is quiet for 40 ms or indefinitely.
  Furthermore, a 40 ms silence risks triggering native link-loss supervision (`0x64fe`), invoking `0x7bd4` to stop the timer and clear `FA7 = 0`.

## K04033
Source `D0622`, original line155, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

#### C. Assertion 3: "NOP 7bf6 fix is justified"
* **Status**: **UNJUSTIFIED / VIOLATES PROJECT SAFETY CONTRACT**.
* **Why Unjustified**:
  1. `AGENTS.md` strictly prohibits factless sweeps and unmeasured firmware modifications. Patching native code before measuring the actual failure mode on hardware violates the core project contract.
  2. In native `0x7bf0`, `0x7bf6: bl #0x767c` enables PPI channel 3 (`RADIO.EVENTS_ADDRESS` -> `TIMER1.TASKS_CAPTURE[2]`). If `0x7bf6` is replaced with `NOP; NOP`, PPI channel 3 is only enabled if `0x7842` is reached. If `0x7842` never executes (e.g. due to `COMPARE[0]` resetting the counter), PPI channel 3 is **never armed**. `CC[2]` will never capture packet arrival timestamps, permanently breaking tracking time synchronization.
  3. Candidate 10 is designed as a read-only observer to diagnose the failure mode without modifying native operation. Replacing observation with a speculative NOP patch defeats the entire diagnostic methodology.

## K04034
Source `D0622`, original line164, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

### 5. Distinguishing Predictions for Future Hardware Observation

## K04035
Source `D0622`, original line166, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

No hardware execution should be proposed without distinguishing predictions:

## K04036
Source `D0622`, original line168, SHA256 `df64e6bf36fa4157ba57ede916c17eebb08ca9af50c0e20b3078901352061d98`. Status: historical/unadjudicated narrative; English source prose.

| Branch | Stamped Conditions | Distinguishing Cause | Evidence-Backed Remedy |
|---|---|---|---|
| **1. Wire / Queue Loss** | Hook 1 (`accepted21_sequence`) == 0 | Command 21 never entered `0x8d40` (dropped by RF, rejected at `0x94fc`, or queue full) | Read Schema 5 byte 26 (`command21_expected_sequence`); align host wire sequence to expected counter |
| **2. Native Stop** | Hook 3 (`stop_clear`) > Hook 2 (`start`), and `FA7 == 0` | Native shutdown `0x7bd4` was called (disconnect or producer shutdown) | Trace callers of `0x7bd4` (`0x64fe`, `0xaea6`); inspect Schema 9 byte 31 and Schema 7 bytes 30..31 |
| **3. Other Clear** | Hook 4 (`other_clear`) > Hook 2 (`start`), and `FA7 == 0` | `0x6588` was called (Command 10 chip ID mismatch or guard failure) | Check Command 10 chip ID payload (`[TRACKER_ID]`) or avoid sending Command 10 |
| **4. Gate Open** | Hook 5 (`gate_set`) > Hooks 3 & 4, and `FA7 == 1` | `EVENTS_COMPARE[2]` fired; gate opened successfully | Gate is open; inspect downstream producer flags for sensor publication |
| **5. Capture Active / Gate Closed** | Hook 2 (`start`) > Hooks 3 & 4, Hook 5 == 0, `FA7 == 0`, and `CC[2]` progressing | Timer started, not cleared by software, but compare never fired | Investigate `TIMER1.CC[0]` period and capture collision under measured data |

## K04037
Source `D0623`, original line1, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

The five earlier product defects are closed in the current isolated source. This is not physical OTA approval. The image is still uninstalled and unapproved, and offline tests do not prove radio delivery or IMU.

## K04038
Source `D0623`, original line3, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

**Closed**

## K04039
Source `D0623`, original line5, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

The 3-second wipe is closed. `PeerEngine.advance` (`peer_engine.py` 264–272), while `ota_admission_active` is true, keeps that command-1 `pending`, rejects a missing pending, and raises after 8 seconds instead of calling `disconnect`. `ack_tag` still returns first while the flag is set (line 247). The stage-0 rediscovery write therefore does not run on this timeout.

## K04040
Source `D0623`, original line7, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

The ratio hole is closed on the runtime path. `TrackerOtaRuntime.record` (214–216) requires S1 in {2,3} and a successful `decode_records` checksum before `LiveAdmission.receive`. `advance` (21–27) stores a fresh full `s->slots` baseline (`roy_ram_loader.py` 786, header word 3) and rejects a 16384-slot move. `receive` (58–61) counts only `1 <= delta <= window`, so the baseline slot, earlier slots, and later slots stay out. The service38 ELF `44f4e52b…` is now in the end-of-run `DATA+1912` hash set (`roy_ram_loader.py` 856).

## K04041
Source `D0623`, original line9, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

Schema3 is no longer length-only. `ota_live_admission.py` 72–73 requires a 32-byte body whose byte 24 is 0 or 1. The other tail bytes stay unchecked; that matches the stated dynamic-RAM rule and still does not read a flash hash.

## K04042
Source `D0623`, original line11, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

The loader bypass and the controller race are closed. Hardware `load_job` (73) calls `consume_start`, which requires `PICO_OTA_START_TOKEN`, exact `command[2:]` versus `sys.argv[1:]`, release and input hashes, then exclusive `start.consumed.json` plus unlink (`ota_start_binding.py` 9–27). Clearance requires `runner_start_validated` (`tracker_ota_readiness.py` 27). The runner and template point `--module`, `--profile`, and `--tracking-watch` at the hashed proposal ELF `44f4e52b…`, `bootstrap205.profile.json`, and `release/tracking_watch`, and they write the token file and environment only after `validate` and `attempt.mkdir()`. `_poll` (281) uses `checked_off_snapshot`, which tests and returns one deep-copied sample under one lock (`controller_guard.py` 130–141).

## K04043
Source `D0623`, original line13, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

The active-query ACK gap is closed. While a proof is active, `record` (220–223) applies the same S1-bit0 / non-IMU `ack_notification` rules and republishes the mailbox.

## K04044
Source `D0623`, original line15, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

**What remains**

## K04045
Source `D0623`, original line17, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

`roy_ram_loader.py` still calls `runtime.poll()` before the ring drain (789 versus the later `record` loop). If `advance` raises at 8 seconds, a proof already in the ring is not parsed. That raise happens before the reconnect write, so this is a fail-closed deadline miss, not the old pending wipe.

## K04046
Source `D0623`, original line19, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

`begin()` still follows the snapshot in the same `_poll`. `--pause-before-ota-start` is not in the frozen command. `--stock stock.bin` is still cwd-relative and outside `EXTRA_INPUTS`; the loader stock SHA check is unchanged.

## K04047
Source `D0623`, original line21, SHA256 `8bcdbc17ad691ec77f0f0b644ff874262b439fde86cf1bbcd50024e3c79fae96`. Status: historical/unadjudicated narrative; English source prose.

`test_runtime_does_not_begin_…` still replaces `StationV4Runtime.poll` and clearance. Beside it, the new tests do use a real `PeerEngine` for the 3s/8s split, real checksum rejection, the slot window, `checked_off_snapshot`, `consume_start`, and native09 replies for sequence 0 then 1. This file does not contain a separate assertion that a repeated sequence 0 is rejected or that an old ACK retires a queued reply.

## K04048
Source `D0624`, original line1, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

The Phase 178 host package is not cleared for physical OTA. The image is still the uninstalled, unapproved 98868-byte candidate `040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d`. Offline tests do not prove radio delivery, installed firmware, or IMU. Current IMU remains 0.

## K04049
Source `D0624`, original line3, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

These gates do hold. `load_job` rejects a disk job unless `preparation_only` is false and `approved_for_ota` is true, and it forces `unverified_fork_authorized` back to false. `_poll` calls `require_hardware_ota_clearance` in `tracker_ota_readiness.py`, which allows only this one fork hash, slot 0, chip `[TRACKER_ID]`, serial `[TRACKER_SERIAL]`, service38, and `source_grid_us` 4727.5. `PeerEngine.ack_tag` returns before touching `pending` while `ota_admission_active` is true. The same `_poll` calls `ControllerGuard.check(require_now=True)` and then `OtaSession.begin()`.

## K04050
Source `D0624`, original line5, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

## Code-proven defects

## K04051
Source `D0624`, original line7, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

**1. The 8-second proof window is killed at 3 seconds, and the reply is drained too late.**

## K04052
Source `D0624`, original line9, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`peer_engine.py` sets `REPLY_TIMEOUT_S = 3.0`. `station_v4_runtime.py` `poll` calls `engine.advance()` on every host loop. With dual-mode off (`station-config.json` has no `tracker_dual_mode_ids`, so `mode_required` stays false), `advance` at lines 278–291 hits the non-mode branch: if `pending.first_sent` is 3 seconds old it calls `disconnect()`, which clears `pending` and sequences. The same poll then writes peer stage 0 and `_reset_peer`. `ota_live_admission.py` lines 26–27 only enforce the 8-second limit inside `advance`, which `_poll` does not reach once the engine has left `WAIT_SYNC` or `STREAMING`.

## K04053
Source `D0624`, original line11, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

The loader loop in `roy_ram_loader.py` calls `runtime.poll()` before it drains the ring into `runtime.record()`. A proof that is already in the ring when the 3-second poll runs is parsed only after `pending` is gone. `LiveAdmission.receive` then raises `Application proof does not match pending query`. `ack_tag`'s early return does not cover this path.

## K04054
Source `D0624`, original line13, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

Effect: a schema2/schema3 answer slower than 3 seconds, or a host stall longer than 3 seconds, aborts admission and forces rediscovery. The advertised 8-second freshness limit does not govern the live loop.

## K04055
Source `D0624`, original line15, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

Minimal fix: while `ota_admission_active` is true, `advance` must not `disconnect` or clear that `pending`. Drain the ring before `advance`, or teach `receive` to accept the matching body after that specific disconnect and still refuse `begin`.

## K04056
Source `D0624`, original line17, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

**2. The 90% gate counts MPSL slot ids, including bad checksums and up to 10 pre-window frames.**

## K04057
Source `D0624`, original line19, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`roy_ram_station_service38.c` line 66 stores `(s->slots & 0x7fff) << 1` in the record halfword. `s->slots` increments on every granted slot (line 79). `D(1912)` increments only after TX END (line 159). The host maps that halfword through `partial >> 1` (`roy_ram_loader.py` line 832) and `radio_tx_end` through `counters[7]`, which is `DATA+1912` (`station_v4_runtime.py` line 279). That counter wiring matches this C file. This audit did not re-hash the external ELF `44f4e52b…`; `validate_service27_plan` is what requires that hash. The end-of-run report hash list at `roy_ram_loader.py` line 856 still omits `44f4e52b…`, so `actual_service27_tx_end` is not written for the ELF this job requires.

## K04058
Source `D0624`, original line21, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`LiveAdmission.receive` adds the slot id before any `decode_records` call. `capture()` still enqueues a frame when the working sum is bad; `valid` only affects `D(1900)` and stage promotion. `hardware_crc_valid` is `bool(profile['crccnf']) and not (partial & 1)`. Service38 always writes bit 0 as 0, and `outputs/bootstrap205.profile.json` has `crccnf` 2, so every queued frame counts. The ring holds 10 records. `poll` sets `tx_baseline` before that iteration's drain, so those 10 ids can be slots whose TX ENDs are already inside the baseline.

## K04059
Source `D0624`, original line23, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

Effect: `len(slots) / tx_delta >= 0.90` with `tx_delta >= 32` can pass when in-window checksum-valid replies are only about 22/32, because 10 older slot ids fill the numerator. Sum8-invalid frames count as replies.

## K04060
Source `D0624`, original line25, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

Minimal fix: increment the numerator only for a new TX-correlated reply whose `decode_records` succeeds, and start the baseline only after the ring has been drained once.

## K04061
Source `D0624`, original line27, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

**3. Schema3 admission checks length only.**

## K04062
Source `D0624`, original line29, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`ota_live_admission.py` lines 62–63 accept schema 3 when the body is 32 bytes, the nonce matches, and the chip matches. The 10-byte tail is not inspected. Schema 2 does require `00 07 00 00` (mode 0, revision 7, N 0, pending 0). A 32-byte schema3 body does not prove diagnostic capability or the installed image. No flash-SHA read exists on this path.

## K04063
Source `D0624`, original line31, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

Minimal fix: require the known schema3 capability bytes from the diagnostic image, and keep that distinct from the OTA image hash.

## K04064
Source `D0624`, original line33, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

**4. The frozen runner does not bind the process that actually starts OTA.**

## K04065
Source `D0624`, original line35, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`run_after_approval.py` hashes `proposal/release/**` and then appends `--authorize-unverified-fork`. `roy_ram_loader.py` `run` skips the no-argument clearance call whenever that flag is set, and the loader never checks `--confirmed-start-diagnostic10`, the attempt directory, or the release manifest. An approved `job.json` plus that flag reaches `preflight` and `_poll` without the runner.

## K04066
Source `D0624`, original line37, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

The controller binary in the frozen command is `/home/steamos/pico-ram-work/release/tracking_watch`. The hashed file is `proposal/release/tracking_watch`. `--profile bootstrap205.profile.json` is also outside `EXPECTED_FILES`; the service callback programs RADIO from the peer record, while this file selects the host CRC flag. `--pause-before-ota-start` is absent, so a passing proof calls `OtaSession.begin()` in that same `_poll`.

## K04067
Source `D0624`, original line39, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`controller_guard.py` `check` releases `lock` before returning. `_poll` then reads `history[-1]` under a new lock and begins. A sample appended between those two reads is saved and does not get the connected-controller test.

## K04068
Source `D0624`, original line41, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

Minimal fix: have the loader require the runner's one-shot marker; point `--tracking-watch` at the hashed binary; take the controller sample and the connected-controller test under one lock, then `begin`.

## K04069
Source `D0624`, original line43, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

## Inferred hardware theory

## K04070
Source `D0624`, original line45, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

While `live_admission.active` is set, `TrackerOtaRuntime.record` returns without `super().record()`, so `PeerEngine.receive` does not update `ack_notification` for frames seen during the wait. The 50 ms retry path increments `retry_count` and does not change generation; `MailboxWriter` will not rewrite an identical mailbox. The MCU still copies that same mailbox every TX slot (`service38.c` lines 133–144). Whether the tracker dedups that repeated command sequence, or withholds the schema reply until S1 bit 0 acknowledges an earlier record, is tracker behavior. This audit did not execute it. The proven host effect is finding 1.

## K04071
Source `D0624`, original line47, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

## Test stubs in `outputs/phase176/test_audit_repairs.py`

## K04072
Source `D0624`, original line49, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`test_mcu_transport_ack_cannot_retire_live_proof` calls `PeerEngine.ack_tag` on the `SimpleNamespace` from `admitted_guard`. It never builds a `PeerEngine`, never calls `advance`, and never runs `station_v4_runtime.poll`'s tag path. The 3-second `disconnect` is untested.

## K04073
Source `D0624`, original line51, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`test_runtime_does_not_begin_until_same_session_proofs_and_live_controller_check` replaces `StationV4Runtime.poll` with `lambda self: None`, `require_hardware_ota_clearance` with `lambda job: None`, and `OtaSession` with a stub. The real order is poll, MCU `ack_tag`, `advance`, then ring drain. The stub skips that order. Its guard is a `SimpleNamespace`, so freshness and `kind == 2` are untested.

## K04074
Source `D0624`, original line53, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`test_bad_service_ratio_does_not_issue_query` feeds synthetic slot ints. It does not use `partial >> 1`, sum8 failures, or the 10-deep ring.

## K04075
Source `D0624`, original line55, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`test_bad_or_stale_raw_reply_never_admits` uses `except RuntimeError: pass`, so a silent reject and a timeout are the same result. Schema3's unchecked tail is the success fixture (`b'\0'*10`).

## K04076
Source `D0624`, original line57, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`test_export_reproduces_repaired_package_without_authorization` runs `prepare_ota_proposal.py` and checks the happy path. It does not exercise a used `attempt/`, an approved source job, or the external ELF, profile, and `tracking_watch` paths.

## K04077
Source `D0624`, original line59, SHA256 `9a712e9b31c469fdd7bff5662443190a1f322f477d021a49b09999acef0f2abf`. Status: historical/unadjudicated narrative; English source prose.

`test_exact_approved_disk_job_load_and_clearance` sets `unverified_fork_authorized` in the test after `load_job`. It does not call `apply_cli_authorization`.

## K04078
Source `D0629`, original line5, SHA256 `08795135fafd8771797b24f23ea226d4574c15455be23a31f3b91cc5b4554869`. Status: historical/unadjudicated narrative; English source prose.

LATEST177: Read docs/PHASE177_SECOND_AUDIT_2026-10-10.md before any OTA. Independent audits found host launch blockers; proposal disabled. Image unchanged, IMU0, no OTA permission.

## K04079
Source `D0629`, original line7, SHA256 `08795135fafd8771797b24f23ea226d4574c15455be23a31f3b91cc5b4554869`. Status: historical/unadjudicated narrative; English source prose.

# PICO / Steam Frame: continuation handoff, Phase173

## K04080
Source `D0629`, original line9, SHA256 `08795135fafd8771797b24f23ea226d4574c15455be23a31f3b91cc5b4554869`. Status: historical/unadjudicated narrative; English source prose.

**Current continuation176:**read ordered observer candidate and execution boundary (original reference: `PHASE176_ORDERED_TIMER_OBSERVER_2026-10-10.md`) first.175native10control rejected/unrun; noRF/OTA. Offlineorderedobserver10SHA040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d/98868B,75offlinecases,notinstalled/notauthorized/finalGrokreviewCOMPLETE. Installed09 unchanged;90–100%IMU goalACTIVE. Original173Grokfinal recovered and saved; older pending text below historical. Maintain user'shard preOTA gate.
