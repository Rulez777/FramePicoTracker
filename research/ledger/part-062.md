# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K04881
Source `D0171`, original line199, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

4022 full blocks + last 4 bytes = 4023. Index 4023 = `0x0fb7`. Native `floor(length/24)+1`; for a multiple of 24 a sentinel is needed. The last 4 bytes of image `00000000`, padding `ff` is not included in whole-image CRC. CRC stock `0xd9`, candidate `0xe6`. Table VA `0x16200` = `crc8_maxim(bytes([i]))` for i=0..255. Test vector `123456789` = `0xa1`.

## K04882
Source `D0171`, original line201, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

SPI: writer `0xd130` opcode `0x02`, reader `0xd1f0` opcode `0x03`; `SPI_offset = floor((i-1)/10)*256 + ((i-1)%10)*24`; erase `0xd2cc` four steps `+0x10000` up to `0x40000` (4×64 KiB). Unicorn full-size 96532 and finish rejection upon corruption of staging byte pass.

## K04883
Source `D0171`, original line203, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Finish ACK confirms staging CRC. Activation `0xfa98` (metadata `0x2f000`, magic including `0xaaaaaaaa`/`0xbbbbbbbb`, reset) is a separate native path. Calls: `0x8548..0x855e`, `0xa638..0xa648`.

## K04884
Source `D0171`, original line205, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The airtime estimate in the document is consistent with the declared framing (`PCNF0=0x40008` → LFLEN 8, S1LEN 4; `PCNF1` BALEN 4 → 5-byte address; MODE 4; CRC 16 bit; preamble 8 bit):

## K04885
Source `D0171`, original line212, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is ideal airtime, not measured slot latency.

## K04886
Source `D0171`, original line214, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 5.2. Finding D-1: `result.json` gets stuck in `waiting_target`

## K04887
Source `D0171`, original line216, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Severity: **major** for host runtime. The document already listed this as candidate No. 5; fix has not been introduced.
- File/function: `pc/radio/ram/tracker_ota_runtime.py`.
- Lines: creation of result L86–87 (`state=waiting_target`); `_write_status` L99 `if self.ota is None: return`; deadline L123–124.
- Trigger: 120 s without `WAIT_SYNC`/`STREAMING` or `firmware != 189` → `RuntimeError`, while `self.ota is None`. The reason is not written to result. Repeating `load_job` rejects the existing file (`OTA result already exists`).
- Fact from code. Does not refute wire format.
- Minimal test: job + poll without peer until deadline → result should become failed with a reason, rather than blocking replay without a log.

## K04888
Source `D0171`, original line223, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Other author's candidates No. 1–4, 6–10 of the document were not analyzed down to a specific counterexample. Start/data/finish ACK is still without correlation of incoming tag/tx_generation (except drain). This is a boundary of the model, not a proven stale-ACK on hardware.

## K04889
Source `D0171`, original line225, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The emulator calls dispatcher/`0xf974` directly: successful full-size staging ≠ 4023 blocks over radio with IRQ, dedup, and queue.

## K04890
Source `D0171`, original line229, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 6. RF compatibility verdict: insufficient data

## K04891
Source `D0171`, original line231, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The document does not claim hardware success. The coincidence of the nominal period with the BLE grid of Roy 3000 µs is **not proven** as a solution. Refuting 'the patch is useless' by this verification is also impossible: there is no candidate RF trace.

## K04892
Source `D0171`, original line233, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Known limitations already named by the document are confirmed by the code and the log:

## K04893
Source `D0171`, original line235, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Autonomous TX when N≠0: 18910→12000 µs, not disabled. 12000 is a multiple of station slot 6000. Path `0x62e4` gates TX by `0x20000e10`, `0x20000f6a`, etc. On stock there was already autonomous TX of 18.91 ms. The scenario 'peer0 TX at 12 ms coincides with poll peer2' is a counterexample for verification, not an established over-the-air fact.
2. Responses 227–528 µs vs RX up to 400 µs in slot 600/200. Period does not shorten payload. Historical reply ratios of 3–30% / ~7% refer to different modes.
3. N13 during polling of 42 ms — only `(13+1)×3000 = 42000`. The old window `[N×period+offset, (N+1)×period]` after station packet is not reproduced by the log.
4. Version remains 189. Host will not distinguish stock from patch without `tracker_poll_decimation_by_id`. An erroneous N9 on patched peer will give reopen `int(9×3000+1810.5)=28810` during polling of 42 ms.
5. Five delays 9922 (finding A-1) break the completeness of 'entire radio cadence on 3000', even if the ten sites are arithmetically correct.

## K04894
Source `D0171`, original line241, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`MODECNF0=0x201` is a limitation of Roy, not bytes of this patch.

## K04895
Source `D0171`, original line245, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 7. Hardware activation/recovery verdict: insufficient data

## K04896
Source `D0171`, original line247, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

There is no dump of the tracker bootloader. Not proven: SPI→internal flash copy, RSA/ECDSA, magic/version, behavior upon power loss, dual-bank, rescue.

## K04897
Source `D0171`, original line249, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Finish ACK ≠ boot candidate. Stock file on disk ≠ hardware unbrick. Native disconnect after `finish_verified` (1 s no-op, then loader exit) can activate update — the document warns correctly.

## K04898
Source `D0171`, original line251, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The chip/flash layout of the tracker cannot be automatically equated to Roy nRF52833.

## K04899
Source `D0171`, original line255, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 8. Evaluation of the document itself as a review package

## K04900
Source `D0171`, original line257, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The document is factually accurate in what is claimed: bytes, hashes, wire, sign −25, SPI layout, caveats, Unicorn boundaries, 79 tests are reproducible, source code has not drifted from Appendix B.

## K04901
Source `D0171`, original line259, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Three verdicts are honestly separated. The list of the author's sender defects is not passed off as fixed.

## K04902
Source `D0171`, original line261, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The weak point of the package: the heading 'full patch table' despite the fact that five arguments of the same `0x641c` of the form `2×period+467` are not analyzed. Profile A compensates for this as a task, but the candidate of 30 bytes is therefore incomplete as a translation of the radio cadence.

## K04903
Source `D0171`, original line263, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Appendix A during packaging contains a reused JUnit line (`Previously executed offline run`). The XML is dated 20:48, artifact 20:50. This does not affect the suite content; this verification ran pytest anew.

## K04904
Source `D0171`, original line267, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 9. Missing inputs

## K04905
Source `D0171`, original line269, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Bootloader dump (bottom of flash up to `0x1000` and upper regions) — signature, copying, rollback, brick.
2. RF capture / TIMER0 trace on stock and on candidate: autonomous TX gate, real reopen vs ADDRESS, five delays 9922, hop `0x6eac`.
3. Confirmed recovery path of one selected tracker prior to any OTA.
4. Provenance test of ACK with tag ring/repeat after session boundary.
5. Measured airtime + ramp, not the sum of fields PCNF.

## K04906
Source `D0171`, original line277, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 10. Criterion for signal to hardware

## K04907
Source `D0171`, original line279, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The offline review is sufficient to accept the **encoding of the ten sites** and the **form of OTA start/data/finish + SPI CRC** within the verified scope.

## K04908
Source `D0171`, original line281, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Insufficient for OTA to device until resolved: bootloader/activation/recovery; decision on the five constants 9922; unhandled `waiting_target`; explicit stop/cleanup; single target; baseline of controllers. User pause remains.

## K04909
Source `D0171`, original line283, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Hardware success, if ever achieved: whole-image CRC accepted; candidate boots; measured 3000/12000; correct RX phase; no regression of controller freeze; reconnect; native IMU/LED/camera; return to stock via working path. None of this is claimed by this verification.

## K04910
Source `D0171`, original line285, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

FW version will remain 189. One cannot infer installed patch from product version.

## K04911
Source `D0172`, original line1, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Independent analysis of patch FW189 grid 3000

## K04912
Source `D0172`, original line3, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-07. Subject: `docs/FW189_GRID3000_INDEPENDENT_VALIDATION.md` and `outputs/tracker-grid3000-20261007/`. The source document and binary artifacts were not modified. Hardware, SSH, RF, and OTA were not run.

## K04913
Source `D0172`, original line12, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The patch correctly encodes the claimed ten edits. These numbers are not the 3000 µs grid on the path for which the original constants were gathered. The OTA sender accepts start/data/finish ACKs without binding to the transmission attempt.

## K04914
Source `D0172`, original line14, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Identity of the verified snapshot

## K04915
Source `D0172`, original line16, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

SHA256 of the document `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`, 184698 bytes, 3761 lines. Matches `outputs/tracker-grid3000-20261007/independent-validation-artifact.json`.

## K04916
Source `D0172`, original line18, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Hashes of Appendix B matched the files in the tree at the time of analysis, including:

## K04917
Source `D0172`, original line20, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Object | SHA256 |
|---|---|
| stock `fw189-stock.bin` and `outputs/swift189-image.bin` | `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89` |
| candidate `fw189-valve-grid-3000.bin` | `6a82a89206a609fdce92a3de764882ce53f79f0360b925bebe1ce7853cde6a0d` |
| vendor container | `41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54` |
| `patch-manifest.json` | `3ac2a6b141903c8c60b4505de0934dbaf15aca2e00b4607fde95a03f27d5f694` |
| `ota-wire-example.json` | `12f3c5f6ca1d80a2491b218b5f083d6f8fe98b4cf29456f5d011a991571164ac` |

## K04918
Source `D0172`, original line28, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Container of 193385 bytes: header of 321 bytes and two identical copies of raw image of 96532 bytes each. Both are equal to frozen stock. Vectors: SP `0x20006000`, reset `0x13a5`.

## K04919
Source `D0172`, original line30, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What is encoded correctly in the ten edits

## K04920
Source `D0172`, original line32, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Between stock and candidate there are exactly 30 differing bytes, the image length is the same. Independent decoding of Thumb-2 MOVW and little-endian double:

## K04921
Source `D0172`, original line47, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

There are no other aligned doubles from the set 4727.5 / 3538 / 3934.5 in the stock image. There are also no other MOVWs with immediates 4727, 18910, 3751, 3355. ADRs in `0x6cb8`, `0x6cc2`, `0x6d2a`, `0x6d34`, `0x6f62` hit these pools. The patch does not shift the pools.

## K04922
Source `D0172`, original line49, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

CRC8/MAXIM of the candidate is `0xe6`. Test vector `123456789` gives `0xa1`. The sum modulo 256 for start/first/last/finish from `ota-wire-example.json` matches. The first data block matches the first 24 bytes of the image. The last block: index 4023, length 4, tail of image `00000000`. The formula `floor(length/24)+1` is in `0xfc1c`. SPI offset `floor(j/10)*256+(j%10)*24` is in `0xf82c`. Finish writes `0xdeadbeaf` (`0xf8e2`) to RAM and does not call `0xfa98` from this handler. Activation calls remain at `0x855e` and `0xa648`.

## K04923
Source `D0172`, original line51, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. Blocker: period of 3000 µs is not what programs the RTC

## K04924
Source `D0172`, original line53, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Function `0x641c` is the common consumer of both MOVW delays and the result of RX formulas. Byte at `0x20000f02` selects the branch:

## K04925
Source `D0172`, original line55, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- zero: reads PRESCALER of TIMER2 (`0x4000a000+0x510`) and goes to `0x3ae0`, compare RTC does not write here;
- non-zero: `0x6478`.

## K04926
Source `D0172`, original line58, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Branch `0x6478`:

## K04927
Source `D0172`, original line60, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. `0x13e90` converts an integer to double.
2. The literal `0x403e8000` with a zero low word is the double `30.5`.
3. `0x13a40` divides by this double: in the prologue, exponents are subtracted (`subs r5, r5, r6`), this is division, not multiplication.
4. `0x13de8` truncates double toward zero.
5. The result is written to `RTC0` (`0x4000b000`) CC at offset `0x540`. Before this, residual `r2` after truncation is written to TASKS_CLEAR. A non-zero write triggers clear, meaning compare is a relative delay from the cleared counter.

## K04928
Source `D0172`, original line66, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The true RTC tick of 32768 Hz is `1000000/32768 = 30.517578125` µs. The firmware divider is exactly `30.5`.

## K04929
Source `D0172`, original line68, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Stock constants are built as integer multiples of `30.5`:

## K04930
Source `D0172`, original line70, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Value | Quotient |
|---|---:|
| 4727.5 | 155 |
| 3538.0 | 116 |
| 3934.5 | 129 |
| 18910 | 620 |
| 3355 | 110 |
| 3751.5 | 123 |
| 10980 | 360 |

## K04931
Source `D0172`, original line82, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The candidate substitutes numbers that are not multiples of `30.5`:

## K04932
Source `D0172`, original line84, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Candidate | `/30.5` |
|---|---:|
| 3000 | 98.360656 |
| 12000 | 393.442623 |
| 1810.5 | 59.360656 |
| 2207 | 72.360656 |
| 2024 | 66.360656 |
| 1628 | 53.377049 |

## K04933
Source `D0172`, original line93, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

One period of 3000 µs will still truncate to 98 ticks. 98 true ticks is `2990.72` µs. Four such periods are 392 ticks, while `12000/30.5` will truncate to 393. The relation "12000 = 4×3000" diverges by one tick after hardware division.

## K04934
Source `D0172`, original line95, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The fraction accumulates because there is only one division and it is performed on the already aggregated sum `trunc(trunc(N*period+offset)/30.5)`, rather than on the sum of truncated ticks. For normal reopen of the candidate:

## K04935
Source `D0172`, original line97, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| N | Requested, µs | Programmed ticks | Time 32768 Hz, µs | Ticks `N*98+59` |
|---:|---:|---:|---:|---:|
| 0 | 1810.5 | 59 | 1800.54 | 59 |
| 1 | 4810.5 | 157 | 4791.26 | 157 |
| 2 | 7810.5 | 256 | 7812.50 | 255 |
| 3 | 10810.5 | 354 | 10803.22 | 353 |
| 9 | 28810.5 | 944 | 28808.59 | 941 |
| 13 | 40810.5 | 1338 | 40832.52 | 1333 |

## K04936
Source `D0172`, original line106, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

At N=13, the requested 40810.5 µs becomes 40832.5 µs, while a stable grid of 98+59 ticks would be 1333, not 1338. This is five ticks, about 152 µs.

## K04937
Source `D0172`, original line108, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The live value of `0x20000f02` was not observed in this analysis. The stock constants match the divider of specifically the non-zero branch, so replacing with 3000 breaks the grid they were built for. The branch with the zero flag does not apply this divider; its time unit has not been determined by this analysis.

## K04938
Source `D0172`, original line110, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The minimal check that catches the error: execute `0x641c` up to the CC write and compare ticks. Current tests stop before `0x641c` and verify the microsecond `r0`, so they pass with an incorrect tick grid. This is the boundary of section 6 of the original document; the boundary itself is stated honestly, divider `30.5` is not accounted for there.

## K04939
Source `D0172`, original line112, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

If the goal is the nearest representable grid of this RTC path, the period should be encoded so that the quotient is stable across N. `98×30.5 = 2989`. With double period 2989.0 and integer offset 1800:

## K04940
Source `D0172`, original line118, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

for checked N 0, 1, 2, 3, 9, and 13. 59 ticks preserve the stock aperture of 155−116 = 39 ticks. This is still 98 ticks per period, that is 2990.72 µs, 9.28 µs shorter than the true 3000 µs of Roy. Over 13 periods, the accumulated shift is about 120 µs with a chain of relative delays. A window of about 39 ticks (~1190 µs) makes such a shift a matter of measurement, not a reason to declare phase matching.

## K04941
Source `D0172`, original line120, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This candidate is not assembled into bytes here and is not installed on the device.

## K04942
Source `D0172`, original line122, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. Major: constant 10980 remained in the same scheduler

## K04943
Source `D0172`, original line124, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`0x6eac`: `MOVW r0, #10980`, followed by `bl 0x641c`. `10980 = 360×30.5` exactly.

## K04944
Source `D0172`, original line126, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Branch `0x6e9a`:

## K04945
Source `D0172`, original line128, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- byte `0x20000ef4 == 0` goes to another path;
- otherwise the byte is cleared;
- byte `0x20000f69 == 0` leads to miss formula `0x6f5e`, which the patch modifies;
- otherwise waits a fixed 10980 µs and continues cycling the channel.

## K04946
Source `D0172`, original line133, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`0x20000ef4` is written with one in `0x6cf0` immediately after arming RX. The meaning of `0x20000f69` from the literal pattern has not been established: the full address of this byte appears as a literal in `0x6fb0`. As long as this branch might turn out to be normal mode, the list of ten locations is incomplete.

## K04947
Source `D0172`, original line135, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

10980 should not be scaled blindly. First, the value and writers of `0x20000f69` are needed.

## K04948
Source `D0172`, original line137, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Nearby are other calls to `0x641c` with `MOVW #9922` (`0x26c2`) at `0x6538`, `0x65a2`, `0x67de`, `0x6c42`, `0x6c5c`. `9922/30.5 = 325.31`, this is not the same grid. They are not included in the list of missed periodic constants.

## K04949
Source `D0172`, original line139, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The match of halfword 3355 at VA `0x187ec` is a fragment of an unrelated double next to ASCII `PICO`/`EOL`, not a second copy of the hunt delay.

## K04950
Source `D0172`, original line141, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. Major: subtraction of 25 µs is less than one tick

## K04951
Source `D0172`, original line143, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`0x6f6c` loads `0x40390000` and zeroes the low word: double `25.0`. `0x13754` inverts the sign and proceeds to addition `0x13508`. The minus sign is confirmed. The purpose of "exactly 25 µs of overhead delay" on the RTC path is not confirmed.

## K04952
Source `D0172`, original line145, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`25/30.5 = 0.82` ticks. After truncation, subtraction either does not change compare or shifts it by one tick (30.5 µs). For stock at N=0: `int(4727.5−25) = 4702`, `4702/30.5` will truncate to 154, just like bare `4727/30.5`.

## K04953
Source `D0172`, original line147, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 4. Major: OTA ACK is not tied to an attempt

## K04954
Source `D0172`, original line149, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`pc/radio/tracker_ota.py`, `OtaSession.receive`, lines 135–155.

## K04955
Source `D0172`, original line151, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In `drain_ack` (line 130), `tx_generation` must be included in `attempt_tags`. In `start`, `data`, and `finish`, only operation and index are compared. The sequence of the incoming record is also not compared.

## K04956
Source `D0172`, original line153, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The firmware enqueues a duplicate data-ACK: `0xfcd8` builds `25 02 index 01 aa` and calls `0x9368`. Drain exists so that a separate no-op with S1 bit0 removes this entry. Analysis assumption: the queue survives until the next session if the previous one was aborted before drain. This property of queue `0x9368` has not been fully traced.

## K04957
Source `D0172`, original line155, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Scenario. The previous session had already received the start ACK and died before S1 bit0. The new session is in `start` and accepts the old `25 01 01`. The index becomes 1, the next state is `data`. The new erase does not yet have to be completed: `0x726a` calls `0xd2cc` in background start processing. The final CRC `0xf7d8` can reject corrupted staging and thus does not replace the order "first erase of this session, then blocks".

## K04958
Source `D0172`, original line157, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Minimal test: the first session receives start ACK and does not perform drain; the second `OtaSession` is fed the same ACK; state must remain `start`. Fix: in `start`/`data`/`finish`, accept ACK only when `tx_generation` is in `attempt_tags` of the current operation.

## K04959
Source `D0172`, original line159, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Verified framework matching the disassembler that this bug does not invalidate:

## K04960
Source `D0172`, original line161, SHA256 `f338c7ad339618a061c3a4b77f3304cca50f150283d1befa154b305fe437ef07`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- start body: `25 01` + BE32 of length + CRC8 + BE16 of field 189;
- data: index from 1, length 0..24, tail `ff` up to 24 bytes;
- for 96532 bytes of blocks 4023, the last one carries 4 bytes;
- an exact multiple of 24 size requires an empty sentinel, and the CRC of the last block takes the remainder of the length, for a multiple size this is 0 bytes;
- the first CRC block reads `sp+3`, skipping 3 bytes of the SPI response.
