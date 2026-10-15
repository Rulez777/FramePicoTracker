# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K04721
Source `D0170`, original line59, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The raw application is considered with **base VA `0x1000`**:

## K04722
Source `D0170`, original line68, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Do not apply VA directly as a file offset, include the container header in the radio payload, or take the second copy together with the first. The sender accepts a raw image, not a container.

## K04723
Source `D0170`, original line70, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Bundle: `outputs/tracker-grid3000-20261007/` contains candidate, separate stock, manifest, and wire examples. Full vendor binaries are not embedded in this MD. They must be provided separately if review requires independent re-disassembly/emulation.

## K04724
Source `D0170`, original line72, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 5. Full patch table

## K04725
Source `D0170`, original line74, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

All hexadecimal sequences below are **file bytes in original order**, not ARM instruction values in textual notation.

## K04726
Source `D0170`, original line76, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| VA | Offset dec / hex | Before | After | Meaning |
|---|---|---|---|---|
| `0x6214` | 21012 / `0x5214` | `44f6de14` | `42f6e064` | `MOVW r4,18910` → `MOVW r4,12000` |
| `0x6218` | 21016 / `0x5218` | `41f27720` | `40f6b830` | `MOVW r0,4727` → `MOVW r0,3000` |
| `0x630a` | 21258 / `0x530a` | `44f6de10` | `42f6e060` | `MOVW r0,18910` → `MOVW r0,12000` |
| `0x630e` | 21262 / `0x530e` | `41f27721` | `40f6b831` | `MOVW r1,4727` → `MOVW r1,3000` |
| `0x6cde` | 23774 / `0x5cde` | `40f6a760` | `40f2e870` | hunt delay 3751 → 2024 |
| `0x6ce4` | 23780 / `0x5ce4` | `40f61b50` | `40f25c60` | hunt delay 3355 → 1628 |
| `0x6d40` | 23872 / `0x5d40` | `000000008077b240` | `000000000070a740` | IEEE754 LE double 4727.5 → 3000.0 |
| `0x6d48` | 23880 / `0x5d48` | `0000000000a4ab40` | `00000000004a9c40` | double 3538.0 → 1810.5 |
| `0x6d50` | 23888 / `0x5d50` | `0000000000bdae40` | `00000000003ea140` | double 3934.5 → 2207.0 |
| `0x6f80` | 24448 / `0x5f80` | `000000008077b240` | `000000000070a740` | double 4727.5 → 3000.0 |

## K04727
Source `D0170`, original line89, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The reserved edit ranges have a total size of 56 bytes: 6×4 + 4×8. **In reality, 30 bytes differ**, since some bytes within the ranges remain the same. The image does not change length or addresses; the literal pool is not shifted; branches are not rewritten.

## K04728
Source `D0170`, original line91, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The patch builder requires the exact stock SHA and expected original bytes at each location. Reverse requires the candidate SHA, restores original bytes, and verifies the stock SHA. Applying the patch to an already modified image is rejected.

## K04729
Source `D0170`, original line93, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 5.1. Why four TX constants

## K04730
Source `D0170`, original line95, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In `0x61f4` and `0x62e4`, there are two native branches for setting TIMER0 CC0. In each, when N=0, 4727 is selected, and when N≠0, 18910 is selected. The candidate sets 3000 and 12000 respectively.

## K04731
Source `D0170`, original line97, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`4 × 4727.5 = 18910`, which is why `4 × 3000 = 12000` was chosen. This is arithmetic consistency, **not proof** that autonomous TX must be exactly a fourfold period in the new system or that it no longer interferes with the controllers.

## K04732
Source `D0170`, original line99, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 5.2. Why RX offsets are shifted rather than scaled

## K04733
Source `D0170`, original line101, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Difference between base periods `Δ = 4727.5 − 3000 = 1727.5 µs`.

## K04734
Source `D0170`, original line103, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Author's choice:

## K04735
Source `D0170`, original line112, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The goal is to preserve the nominal reception aperture width relative to the end of the period, rather than scaling RF airtime/IRQ latency. For example `4727.5−3538 = 3000−1810.5 = 1189.5 µs`.

## K04736
Source `D0170`, original line114, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Verify the rationale for this choice.** Two hunt values are rounded up by 0.5 µs. Normal double after conversion to integer yields 1810, not 1810.5. Do not conflate the double intermediate with the actually set timer count.

## K04737
Source `D0170`, original line116, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 5.3. Preserved compensation of −25 µs

## K04738
Source `D0170`, original line118, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the miss branch `0x6f5e..0x6f78`, `0x13754` is called, which inverts the sign of the second double and branches to an addition operation. The result is **subtraction** of 25, not addition.

## K04739
Source `D0170`, original line125, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The fact of the sign itself is verified by ARM code execution. Designating −25 as compensation for software service latency is an engineering interpretation that still needs verification against the full control flow and timer origin. It cannot be considered proven that actual processing takes exactly 25 µs under the new mode.

## K04740
Source `D0170`, original line127, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 6. What exactly was checked in radio emulation

## K04741
Source `D0170`, original line129, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Branch | Emulator entry | Expected stock result | Expected patched result |
|---|---|---|---|
| TX path A | `0x61f4` | CC0=4727 or 18910 | CC0=3000 or 12000 |
| TX path B | `0x62e4` | CC0=4727 or 18910 | CC0=3000 or 12000 |
| Normal reopen | `0x6cac` | `int(N*4727.5+3538)` | `int(N*3000+1810.5)` |
| Alternate reopen | `0x6d22` | `int(N*4727.5+3934.5)` | `N*3000+2207` |
| Miss scheduling | `0x6f5e`, input r0=N+1 | `int((N+1)*4727.5−25)` | `(N+1)*3000−25` |
| Hunt A | `0x6cde` | 3751 | 2024 |
| Hunt B | `0x6ce4` | 3355 | 1628 |

## K04742
Source `D0170`, original line139, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

RX N verified for 0,1,3,9,13,40,255; TX N for 0,1,9. Input registers/RAM are configured manually. For RX, emulation stops **before** `0x641c` and reads r0, that is, verifies the computed argument of the timer routine. It does not execute actual hardware sleep/wakeup or RADIO interrupt.

## K04743
Source `D0170`, original line141, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Neither `N=255` in the offline branch nor testing an arbitrary synthetic payload implies an allowed product configuration. The runtime restricts command16 N to the range 0..40.

## K04744
Source `D0170`, original line143, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 7. Why the period grid does not yet guarantee a solution

## K04745
Source `D0170`, original line145, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 7.1. N13 at 42 ms polling — only a nominal calculation

## K04746
Source `D0170`, original line147, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Station: one MPSL slot every 6 ms, cycle of 7 slots, one target poll every 42 ms. Nominally `(13+1)*3000 = 42000`, therefore N13 is proposed for the patched tracker. Stock remains on empirical N9.

## K04747
Source `D0170`, original line149, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For N13:

## K04748
Source `D0170`, original line157, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

These are not three absolute times from the end of the station poll. It is necessary to add/account for the timer start location, RADIO ADDRESS/END, RX shutdown, response generation, ISR, and the chosen TIMER/RTC path. The old assumption "RX is strictly within [N*period+offset,(N+1)*period] after the station packet" was not reproduced by hardware measurements.

## K04749
Source `D0170`, original line159, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 7.2. Autonomous transmission remains

## K04750
Source `D0170`, original line161, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The candidate changes autonomous TX18910→12000 when N≠0, but does not disable it and does not grant Roy the right to reserve its window. With 42 ms polls, TX may occur at 12/24/36 ms between polls. In the case of three peers, intervals may coincide with polls of another peer or with each other.

## K04751
Source `D0170`, original line163, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Verify a specific scenario: station polls peers at phases of 0/6/12 ms of the 42 ms cycle. If each peer's autonomous TX starts 12 ms from its anchor, then peer0 TX at 12 ms may coincide with the station poll of peer2; at 24 ms coincidences of autonomous transmissions are possible. **This is a counterexample for verification, not an established over-the-air fact**: it is necessary to determine the gating/restart/stop conditions for TIMER0 and the actual binding to ADDRESS.

## K04752
Source `D0170`, original line165, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 7.3. Length of RF responses

## K04753
Source `D0170`, original line167, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Coincidence of RX with poll does not reduce payload. A native reply may contain several IMU samples and queued command records. In the existing system, some responses ended later than 400 µs.

## K04754
Source `D0170`, original line169, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

To estimate airtime, check framing against actual `PCNF0=0x40008`, `PCNF1=0x01040096`, `MODE=4`, `CRCCNF=2`, and MCU clock. Assuming preamble 8 bits, address 40 bits, length 8 bits, S1=4 bits, CRC 16 bits, 2 Mbps:

## K04755
Source `D0170`, original line177, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is a conditional estimate based on register layout, not measured latency. Check bit fields, DMA S1 representation, PPI/SHORTS, queued replies, and actual ramp. The measured slot budget cannot be replaced by the sum of ideal airtimes.

## K04756
Source `D0170`, original line179, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 7.4. Inter-domain phase and frequency

## K04757
Source `D0170`, original line181, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Roy phase lock uses LF RTC 32768 Hz, while the tracker uses its own timer/clock domains. 3000 µs is equal to 98.304 RTC ticks. Roy uses rational phase computation; for the tracker, clocks and start events are independent. Be sure to check drift after missed polls, rounding, RTC wrap at 512 s, latency compensation, and the selection of hardware/software timer path.

## K04758
Source `D0170`, original line183, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 7.5. Event frequency and side effects

## K04759
Source `D0170`, original line185, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

TX interval at N≠0 decreases 18.91→12 ms, frequency grows approximately 52.88→83.33 Hz, that is, by approximately 57.6%. For N0, interval 4727→3000 also increases frequency by approximately 57.6%.

## K04760
Source `D0170`, original line187, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Check battery/CPU/radio duty cycle, IMU batching/sample timestamps, interrupt interference with camera LED timer, queue overflow/exhaustion, reconnect thresholds 200/(N+1), channel hopping, and application of command16 after M rotations. "Remaining bytes unchanged" does not rule out these effects.

## K04761
Source `D0170`, original line189, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 8. OTA: outer packet and body

## K04762
Source `D0170`, original line201, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

S1 and hardware radio CRC are outside this application record. Sequence is used modulo64. Working checksum is a sum, **not CRC8/MAXIM**. CRC8/MAXIM is required for the image and matches the stock table at VA0x16200.

## K04763
Source `D0170`, original line203, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 8.1. Start body, 9 bytes

## K04764
Source `D0170`, original line205, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Offset in body | Size | Value |
|---|---:|---|
| 0 | 1 | `25` |
| 1 | 1 | `01` start |
| 2 | 4 | raw image length, BE32 |
| 6 | 1 | CRC8/MAXIM of the entire raw image |
| 7 | 2 | transmitted field=189, BE16 |

## K04765
Source `D0170`, original line213, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Calling the last field "version" is the current interpretation of the builder, not an independently proven semantic of the entire host protocol. In the native handler, it is saved and used in diagnostic/status events. Check whether this is a transaction/version token with additional requirements.

## K04766
Source `D0170`, original line215, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Length 96532=`0x00017914`; candidate CRC=`0xe6`; 189=`0x00bd`.

## K04767
Source `D0170`, original line223, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Native start takes the lower 24 bits of size, CRC byte, and BE16 field. Builder restricts length and block count in advance. For any images smaller than 24 bytes, current builder rejects transmission: native final CRC always processes the first full 24 byte block.

## K04768
Source `D0170`, original line225, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 8.2. Data body, 29 bytes

## K04769
Source `D0170`, original line227, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Offset in body | Size | Value |
|---|---:|---|
| 0 | 1 | `25` |
| 1 | 1 | `02` data |
| 2 | 2 | block index BE16, starting from 1 |
| 4 | 1 | actual data length 0..24 |
| 5 | 24 | block data, padding `ff` |

## K04770
Source `D0170`, original line235, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The meaning of the actual length byte requires independent verification: the current native writer writes 24 bytes and readback verifies 24 bytes; the final CRC accounts for the real total length. The field is saved by the handler, but does not replace the fixed writer count.

## K04771
Source `D0170`, original line237, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For image 96532:4022 full blocks + last 4 bytes = 4023 blocks. Last index 4023=`0x0fb7`; native formula blockcount=`floor(length/24)+1`, therefore for an exact multiple 24, an additional empty sentinel is needed. Padding must not be included in the whole-image CRC.

## K04772
Source `D0170`, original line246, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

These wire examples use sequence 0 for illustration. The actual sender sequence changes after application ACK; the entire firmware cannot be transmitted with tag 0 from these examples.

## K04773
Source `D0170`, original line248, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 8.3. Finish body and ACK

## K04774
Source `D0170`, original line260, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Suffix `aa` exists in the native duplicate-block ACK branch. The decoder allows only full forms of length 3/5/6 and success values 0/1. Matches operation/index. Current sender does not verify incoming record sequence for OTA ACK and does not bind start/data/finish ACK to tx_generation; tx_generation correlation is used only in ACK drain. Evaluate the possibility of stale/replayed ACK.

## K04775
Source `D0170`, original line262, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 9. SPI staging, CRC and activation

## K04776
Source `D0170`, original line266, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

According to application disassembly:

## K04777
Source `D0170`, original line268, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- start calls erase `0xd2cc`: 4×64 KiB staging address space;
- writer `0xd130` uses SPI program opcode `0x02` and 24-bit address;
- reader `0xd1f0`/`0xd218` uses SPI read opcode `0x03`; in the model, the first 3 returned bytes before image are treated as address/dummy bytes;
- ten 24-byte blocks per 256-byte page, 16 bytes page gap;
- native write offset for 1-based index i:

## K04778
Source `D0170`, original line279, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- block readback checks all 24 bytes;
- `0xf7d8` traverses this layout and calculates CRC8 of the entire image; `0xf89c` compares expected CRC;
- success finish sets volatile valid/status/metadata in RAM, including the value `0xdeadbeaf`; this is not yet proof of bootloader execution;
- later native path `0xfa98` writes 24-byte metadata into flash `0x2f000`, with activation words, including `0xaaaaaaaa`/`0xbbbbbbbb` depending on the argument, verifies the CRC of specifically these 24 bytes and triggers a system reset;
- calls to activation are found in `0x8548..0x855e` and `0xa638..0xa648`. Complete semantics of user/power/disconnect triggers must be verified separately.

## K04779
Source `D0170`, original line285, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Disassembled tracker bootloader is missing.** It is not proven how it copies the SPI image, verifies magic/version/signature/CRC/length, protects settings/calibration, behaves upon power loss, and recovers after a bad application boot.

## K04780
Source `D0170`, original line287, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The old `outputs/tracker-ota-gemini.md` claimed internal flash Bank1 above 0x30000, absence of image CRC, and guaranteed boot of modified image. These conclusions are not accepted: the discovered SPI path and native whole-image CRC contradict them, and the bootloader is absent. RSA check on PICO host does not prove the absence of additional verification on the tracker bootloader.

## K04781
Source `D0170`, original line289, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Having a stock binary ≠ having guaranteed recovery. OTA rollback works only if an executable native OTA path is preserved or a separately verified rescue/debug mechanism is available. `reverse()` offline cannot be passed off as a hardware unbrick.

## K04782
Source `D0170`, original line291, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 10. Sender and integration

## K04783
Source `D0170`, original line301, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

During operation S1=2. After decoded application ACK, the sender sends a separate no-op payload `00`, S1=3, to acknowledge queued record. Advancing further requires a CRC-valid reply with tx_generation of one of the drain attempts. The next command packet again has S1=2. Thus, retransmitted data packets should not automatically acknowledge their not-yet-read ACK.

## K04784
Source `D0170`, original line303, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Retry every 0.25 s changes publication generation, but preserves command sequence/body. Native MCU continues to poll each target slot; host retries field is not equal to the number of RF attempts. Command sequence increment modulo64 occurs after ACK, including start/finish.

## K04785
Source `D0170`, original line305, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Limits: data/drain 10 s, start/finish 30 s, total 1800 s; initial target configuration 120 s. MCU reply timeout 40 s allows staging erase/CRC, but independent HMD/controller watchdog and MCU host heartbeat continue to operate.

## K04786
Source `D0170`, original line307, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Nominally 4025 OTA operations and 4025 ACK drains at a 42 ms poll period give a minimum of about 338.1 s with one RF reply per operation/drain and instantaneous host processing. With a conditional p=0.3 of independent replies, a rough estimate is about 1127 s. Real losses are bursty, callbacks/host reads/SPI are costly: this is not an ETA and not proof of the sufficiency of 1800 s.

## K04787
Source `D0170`, original line311, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Explicit CLI `--tracker-ota-job`; service config alone does not run OTA.
- Exact target LE32 identity and slot from private registry, one target per run.
- Exact image size 96532/hash; patch job additionally verifies reverse/rebuild and the entire manifest.
- Stock rollback hash must match frozen stock.
- Actual source cadence is set explicitly 4727.5 or 3000: product version is always 189.
- Result is created exclusively; existing result prohibits replay of the same job.
- Optics turned off, phase lock turned on, minimum 2 controllers, freeze≤150 ms.
- HIGH 600/6000 µs, margin minimum 200 µs; initial config cycle 7, N9. In-memory target N changes to 13 for source 3000.
- After loss of stage 3/search generation sender abort; no automatic reconnection/resume.
- After finish_verified, another 1 s of no-op polls, then loader exits via normal cleanup. This stop can lead to native disconnect and activation. Do not consider transfer "stage only" a safe mode without bootloader/activation review.

## K04788
Source `D0170`, original line322, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Normal station supports explicit `tracker_poll_decimation_by_id` for mixed stock/patched peers. The map itself does not confirm installed firmware. The host cannot obtain a new version marker from the patch because version bytes are deliberately not modified.

## K04789
Source `D0170`, original line324, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 10.2. Author's notes for independent analysis

## K04790
Source `D0170`, original line326, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

These are **defect candidates** noticed during preparation of the document; fixes are not introduced here and reproduction is not asserted:

## K04791
Source `D0170`, original line328, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. `OtaSession.generation` starts from 0/2, although normal `PeerEngine` prior to OTA has already used its namespace generations. `MailboxWriter.sent` and ring are not zeroed by a separate OTA epoch. Can a delayed pre-OTA reply with tag 4 satisfy a new drain tag 4? A counterexample with real provenance/slot ordering is required, rather than comparing numbers in a vacuum.
2. Start/data/finish ACKs are verified by operation/index, but incoming tag/tx_generation is not correlated. Are old same-index ACKs possible in ring/queued tracker records after a session boundary? Are target identity and no-resume sufficient?
3. A no-op reply with matching tx_generation proves transmission attempt, but not necessarily clearing of the entire native queued record or processing of S1 bit 0. How does the native handler treat duplicate ACK-only packets and sequence caching?
4. The emulator calls dispatcher/background handler directly and skips real outer working-record sequence deduplication, IRQ delivery, and task scheduling. Therefore, a successful full-size staging test does not prove live transmission of all 4023 blocks via the current sender.
5. If target does not reach OTA and 120 s expires, result may remain `waiting_target`, because failure persistence works primarily after `OtaSession` creation. Check errors before begin, guard failure, and process termination.
6. `.tmp`/status/events writing is performed frequently; SWD reads/write latency and disk stalls may delay heartbeat. Atomic replace by itself does not provide durability upon power loss.
7. A number of config fields are checked by ranges/truthiness; malformed values/type/NaN/unexpectedly large margin may lead to bypassing assumptions or abnormal termination. Check fail-closed prior to the first hardware write.
8. In runtime, 40 s MCU silence tolerance is set, while the native OTA timer resets to 10 s in known branches. Evaluate the interaction of these budgets and peer disappearance during long erase/final CRC.
9. Generation 16-bit is stored in RF provenance; counter is 32-bit host. Evaluate wrap/ABA and sets attempt_tags during a long transmission.
10. Firmware program file remained the same 189; after OTA, the first reconnect may receive erroneous N9 until per-ID map is switched. Check the order of evidence/registration/config update for patch and rollback.

## K04792
Source `D0170`, original line339, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 11. Real tests and model boundaries

## K04793
Source `D0170`, original line341, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Latest preparation suite: 79 passed. The appendix provides an up-to-date replay of the offline suite and file hashes at the time of MD packaging.

## K04794
Source `D0170`, original line351, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The test's full-size native handler payload is synthetic `bytes(i % 251 for i in range(96532))`, rather than candidate execution as a full application after reset. A separate sender test uses stock binary and fabricated successful ACKs. They cannot be described as a real end-to-end boot of patched firmware.

## K04795
Source `D0170`, original line355, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Thumb mode; ROM bytes base 0x1000, RAM 0x20000000..0x20005fff, peripheral register memory 0x40000000..0x4001ffff;
- fixed stack/LR for calls; registers/input flags set manually;
- stubs logger, timers, diagnostic events, autonomous TX side effect;
- erase/write/read — Python bytearray model without SPI timing/error/wear/power loss;
- radio queue send is intercepted and returns success, without native queue backpressure;
- RX checks stop at entry timer routine;
- firmware background processing is forcibly invoked after each test packet;
- bootloader, true interrupt priorities, MPSL, hardware clocks, NVMC activation, and controllers are not emulated.

## K04796
Source `D0170`, original line364, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

On the first Windows pytest run, faulthandler printed native access-violation diagnostics in Unicorn mem_map, although assertions passed. Re-run was performed with `-p no:faulthandler`. The reason for these diagnostics was not independently investigated. A reviewer can reproduce the emulation in a separate Linux environment; disabling diagnostic printing must not substitute for checking execution correctness.

## K04797
Source `D0170`, original line366, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 12. Tasks for independent reviewers

## K04798
Source `D0170`, original line368, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Profile A: ARM/binary patch

## K04799
Source `D0170`, original line370, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Recheck all 10 sites, Thumb MOVW encoding, literal pool addresses, little-endian doubles, call graph, all references, and alternative timers. Find other period-derived constants that the patch missed. Refute or confirm subtract-offset policy and necessity/meaning of −25. Propose a minimal byte-level diff only after justification.

## K04800
Source `D0170`, original line372, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Profile B: RF/timing
