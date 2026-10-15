# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05681
Source `D0359`, original line148, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Local artifacts: `outputs/service-fast-poll20/`.

## K05682
Source `D0359`, original line150, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- `analysis.json`, `analyze.py`: rate calculations and final table;
- `service-measure20/`: baseline and original assert472;
- `service-measure20b/`: intermediate SESSION_IDLE-only variant;
- `service-measure20c/`: successful fast runs without command16;
- `service-measure20d/`: N0 and failure with optics;
- in each folder: json report, config, live snapshots, stdout/OpenOCD, recovery and backup;
- `service-build-fixed20c/`: regular2020 ELF and deployment manifest;
- `measured-fixed-c.elf`:2036-byte measurement ELF;
- `prepare_measurement.py`, `run_measurement.py`, `test_callback.py`: preparation/controlled run/native verification. The run script uses a unique before-run directory and will refuse to run repeatedly in the same folder without an explicit new directory.

## K05683
Source `D0359`, original line160, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Metadata/internal state dumps remain local. For external reviewers, the report, callback source, and selected anonymized counters are sufficient, not all stack/recovery dumps.

## K05684
Source `D0359`, original line162, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What is installed on Frame after measurement

## K05685
Source `D0359`, original line164, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Permanent build: `/home/steamos/pico-ram-work/service-build-fixed20c/roy_station.elf`.
- `release/roy_station_service.elf` and active `release/roy_station.elf`: fixed2020 ELF SHA `8f784fc334c1e24a455513ff5565385b63a309427994b10d89ed20b7ceca44e4`.
- Code SHA `b8c7047a76d16ec43e142420b87364b20e955580a4edce38686a6886df79f78a`.
- `release/roy_station_combat.elf`: original production2016 ELF, unchanged.
- `service-config-fast-poll20.json` and `station-config.json`: radio-only N0 config of the successful test, optics turned off, controllers_off guard enabled. Full config SHA `2e2cf170eb7ea53e840673d61cbf56075997b7d8393289d7ec9784572bbf1b90`.
- Previous active ELF/config preserved in `service-measure20*/before-run/`; original service ELF preserved before installing the new one.
- unit `frame-pico-radio.service` was not started in continuous/autostart mode: tests launched the same loader with finite duration. The unit is currently inactive/failed, radio is stopped.

## K05686
Source `D0359`, original line172, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Final conclusion:** service re-request works and increases the number of slots by approximately 19 times. This is not yet a high-quality channel: about11–12% RX/TX and on the order of13 IMU samples/s. Integration with optics remains unstable. The result does not permit OTA and is not proof of project readiness for flashing or 6DoF.

## K05687
Source `D0360`, original line1, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Service polling: TX timing and new command16 numbers — iteration21

## K05688
Source `D0360`, original line3, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-08. User authorized the following test. Two limited RF runs, SRAM Roy only, one stock FW189 tracker, optics turned off.

## K05689
Source `D0360`, original line7, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**N=0 with the new command number did not resolve the problem: 446 RX for3777 granted slots,11,81%.** N=8 with another new number yielded190 RX for3788 slots,5,02%. The slot rate of both experiments is virtually identical: about125/s.

## K05690
Source `D0360`, original line9, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In adjacent answered slots, TXEN software timestamps have an interval of ~4727µs. But the schedule as a whole is not a continuous grid: between answered slots with a number difference of7, 42,5ms often elapses instead of the nominal33,1ms. Upon repeated requests following denials, delays/gaps occur.

## K05691
Source `D0360`, original line11, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The test does not establish the exact moment of RX opening on the tracker and does not prove a single cause for all losses. The hypothesis that "the host merely did not apply N=0 due to sequence0 repetition" did not resolve the problem with new sequence1/2. Configuration switching is accompanied by a change in response behavior, but the value of N was not directly read from device memory.

## K05692
Source `D0360`, original line13, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Both runs completed without assert; session/RAM/MPU cleanup was confirmed. Radio is stopped, installed2020 ELF and original service config were restored.

## K05693
Source `D0360`, original line15, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Plan and actual configuration

## K05694
Source `D0360`, original line17, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Test bench verified prior to test: Frame is accessible via SSH; HMD valid, both Valve controllers disconnected/invalid; loader/OpenOCD not running. Tracker slot0,chipLE3eed1979. Other slots1/2 disabled.

## K05695
Source `D0360`, original line19, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Order of experiments, each with duration30s:

## K05696
Source `D0360`, original line24, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In all other parameters the mode is identical: interval4727µs,slot900µs,end_margin100µs,cycle1,priority0,phase_lockfalse,opticsfalse,controllers_off guardtrue. Autostart/continuous RF were not enabled.

## K05697
Source `D0360`, original line26, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Command numbers were set only in a separate diagnostic copy of the loader via intercepting `PeerEngine._begin` for command16. The working PeerEngine was not edited. Other commands retained stock numbers and behavior. Between sessions, firmware and pairing flash were not written.

## K05698
Source `D0360`, original line28, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Target ID, body,sequence,generation confirmed in command-log:

## K05699
Source `D0360`, original line30, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Experiment | ID | Body | Sequence | Generation |
| --- | --- | --- | ---: | ---: |
| N8 | [TRACKER_ID] | 0802 | 1 | 4 |
| N0 | [TRACKER_ID] | 0002 | 2 | 4 |

## K05700
Source `D0360`, original line35, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Local packing of individual command-records with checksum: `01 10 02 08 02 1d` and `02 10 02 00 02 16`. These are command-record bodies in a stock radio packet, not the complete RF payload.

## K05701
Source `D0360`, original line37, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

New numbers differ from sequence0 used in previous experiments. Host-log and transport ACK do not themselves replace application readback; N is not declared directly read/proven on the tracker.

## K05702
Source `D0360`, original line39, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Measurement build

## K05703
Source `D0360`, original line41, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The base is the service2020 ELF with fixed low-priority request API, without returning REQUEST. Changes exist only in the separate source `service-measure21/roy_ram_station_v4.c`:

## K05704
Source `D0360`, original line43, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- immediately before `RADIO TASKS_TXEN=1`, DWT CYCCNT is saved in DATA+1904;
- on RX capture, this saved timestamp is written into the previous field `Record.number`;
- loader outputs it as `tx_enable_cycles`, and sets `sequence` to null so as not to pass off the timestamp as an RX packet number;
- source_chip_id field, peer identity guard, payload, CRC, RX counter, deadlines, and cleanup are preserved.

## K05705
Source `D0360`, original line48, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is a **software timestamp before TXEN**, not a hardware-captured ADDRESS/END and not the exact start of the on-air signal. Ramp-up and packet preparation prior to transmission exist. For comparing the interval grid, the timestamp is suitable, but it is not passed off as an RF-edge timestamp.

## K05706
Source `D0360`, original line50, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Timestamps in the ring are available only for slots where RX is recorded. Additionally, loader reads the latest timestamp value and header counters, but these two readings are not atomic. This test does not provide a complete series of timestamps for all TXs.

## K05707
Source `D0360`, original line52, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Size2036bytes≤2045. ELF SHA256:

## K05708
Source `D0360`, original line60, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Executable offline Unicorn verification confirms: timestamp is captured, written into RX-record, source_chip_id remains[TRACKER_ID], MCU RX-counter increments as before, stop is not asserted. Native/MMIO behavior is modeled; this is not a check of the real scheduler, which is verified by RF runs.

## K05709
Source `D0360`, original line62, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Counter results

## K05710
Source `D0360`, original line64, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Metric | N8,sequence1 | N0,sequence2 |
| --- | ---: | ---: |
| Counter span | 29,942s | 29,988s |
| Slots,total | 3788 | 3777 |
| RX MCU,total | 190 | 446 |
| Slots/s by increment | 125,61 | 124,95 |
| RX/s by increment | 6,35 | 14,81 |
| RX/slots | 5,02% | 11,81% |
| blocked/cancelled/idle signals,total | 3009 | 2996 |
| Dropped,total | 8 | 1 |
| RX-records read | 182 | 445 |
| Of which working channel2,CRCvalid | 181 | 444 |
| Max observed IMU samples | 114 | 376 |
| State | waiting_camera_sync | waiting_camera_sync |
| assert | none | none |

## K05711
Source `D0360`, original line80, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The MCU RX counter includes frames lost during ring overflow. Therefore, 190/446 is the true cumulative RX, and 182/445 is the number of records for timing analysis. Hardware CRC is verified for selected working records. IMU rate is about 3,8/12,5 samples/s, not 120.

## K05712
Source `D0360`, original line82, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

There is no separate TX-END counter in this build: a timestamp takes its place. Therefore, the table gives **RX/slots**, rather than a newly measured RX/TX-END. Iteration 20 proved the match between slots and TX-END in the corresponding successful radio-only runs; we do not carry this over automatically into new exact TX totals. The number of packets received by the tracker is not directly known.

## K05713
Source `D0360`, original line84, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Actual TXEN grid

## K05714
Source `D0360`, original line86, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For an RX-record pair, the slot number is computed modulo 32768. At a difference of 1, the TXEN timestamps of two adjacent answered slots are compared:

## K05715
Source `D0360`, original line88, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Metric | N8 | N0 |
| --- | ---: | ---: |
| Number of adjacent pairs | 5 | 99 |
| Min TXEN interval | 4724,688 µs | 4724,688 µs |
| Median | 4726,875 µs | 4726,688 µs |
| Max | 4727,875 µs | 4728,875 µs |

## K05716
Source `D0360`, original line95, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Adjacent answered slots indeed almost correspond to 4727 µs. The error 'each interval is actually 6000/12000 µs' is not confirmed by this dataset. However, the sample is limited to **answered** slots; it cannot be declared the grid of all provided slots.

## K05717
Source `D0360`, original line97, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In N0, the most frequent differences in answered slot numbers are: 7 (142 pairs), 1 (99), 3 (40), 2 (39), 24/16 (24 each). Between the TXEN of answered slots, the most frequent intervals are: 42,5 ms (134), 4,7 ms (99), 37,5 ms (36), 151,5 ms (23), 32,8/32,7 ms (20/17).

## K05718
Source `D0360`, original line99, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For a difference of 7, the nominal time is 7 × 4727 = 33089 µs; the observed 42,5 ms contain about 9,4 ms of additional time. This is direct evidence of schedule gaps/stretching between these slots. About 100 low-priority events/s confirm that the absence of connected controllers does not mean the absence of MPSL failures. The cause of these events—a specific scan/advertising/other reservation—is not established by this test; this is not an RF-spectrum capture.

## K05719
Source `D0360`, original line101, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Switching N8 → N0 changed the character of the long intervals: for N8, 86 ms, 321,3 ms, 72,8 ms are frequent; for N0 — 42,5 ms and 4,7 ms. This is consistent with the effect of the tracker configuration, but sequential A/B does not isolate all environmental changes and is not a reading of its memory.

## K05720
Source `D0360`, original line103, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## RX observation delay

## K05721
Source `D0360`, original line105, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

DWT difference `roy_cycles - tx_enable_cycles`, taking into account 32-bit wrap:

## K05722
Source `D0360`, original line107, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Metric | N8 | N0 |
| --- | ---: | ---: |
| Records | 181 | 444 |
| Min | 218,94 µs | 202,56 µs |
| Median | 420,75 µs | 420,75 µs |
| Max | 734,06 µs | 733,50 µs |

## K05723
Source `D0360`, original line114, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is the time until the RX record callback, not until ADDRESS/start of the response. With an RX deadline of 800 µs, the latest recorded responses are close to the end of the window. Late responses that no longer completed before the deadline are not visible in this log. Therefore, **an overly short RX window remains a possible cause of part of the losses**, and the absence of such records does not prove the absence of late responses.

## K05724
Source `D0360`, original line116, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The receiver window of the tracker itself has not been measured. The test shows the station rhythm and received responses, not the moments when the tracker heard/did not hear the poll.

## K05725
Source `D0360`, original line118, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Do not draw conclusions beyond the data

## K05726
Source `D0360`, original line120, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- A clear RF environment is not proven: there is no spectral measurement; the absence of connected Valve controllers is proven.
- It is not proven that all 11–12% are explained by a single factor. There are TX schedule gaps, a limited RX deadline, and unverified actual tracker windows.
- New command numbers did not raise N0 to 100%. Previous sequence 0/deduplication may affect earlier sessions, but they do not explain the entire current result.
- Camera processing is excluded. `waiting_camera_sync` is expected; pose_valid=false. There is no new verification of 6DoF, 3 trackers, coexistence, or stable optics.
- There is no OTA permission. Firmware/hash/whitelist/readiness gates were not changed.

## K05727
Source `D0360`, original line126, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Cleanup, saving, files

## K05728
Source `D0360`, original line128, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For both runs, session_closed_verified,ram_restored_verified,mpu_restored,unloaded=true; flash_written=false. Host-restoration: module_restored,config_restored,combat_preserved=true. Between experiments and after them, the success of cleanup is verified; continuation after assert was not performed.

## K05729
Source `D0360`, original line130, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Locally: `outputs/service-fast-poll21/`:

## K05730
Source `D0360`, original line132, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- `prepare.py`, `run.py`, `make_runner.py`: preparation and limited operator launch;
- `test_timestamp.py`: native/MMIO verification of timestamp and identity;
- `analyze.py`: reproducible calculation of statistics;
- `service-measure21/`: complete build, source, report, live snapshots, command logs, OpenOCD/stdout, manifests/recovery, and backup;
- `service-measure21/timing-analysis.json`: machine-readable results.

## K05731
Source `D0360`, original line138, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

On Frame, `/home/steamos/pico-ram-work/service-measure21/` was saved. Production 2016 and working service 2020 builds are not modified. Measurement 2036 was not left active. Radio was stopped after the test; the default service config from the beginning of the test was restored byte-for-byte.

## K05732
Source `D0360`, original line140, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Summary:** confirmed local TX intervals ~4727 µs, the effect of the new N setting, and disruptions in the overall schedule. Channel reliability remains low; the exact share of losses due to the scheduler, RX deadline, and tracker window has not yet been separated.

## K05733
Source `D0361`, original line1, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 22: offline reconciliation of Swift FW189 and PICO PhoenixSta 2.96 settings

## K05734
Source `D0361`, original line3, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-08. Scope: local source files, saved binary listings, and logs of iteration 21. SSH, RF, OTA, reset, and writes to devices were not performed. Production code and images were not modified.

## K05735
Source `D0361`, original line7, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Our station does not yet reproduce the entire PICO operating cycle. Differences in the countdown for applying command 16, the method of starting TX, and incomplete synchronization in the last test were confirmed. None of them individually has been proven to be the cause of the 88% losses. The conclusion that 'RX opens only once every 42,5 ms' does not follow from the available data.

## K05736
Source `D0361`, original line9, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. Command 16: native path exists

## K05737
Source `D0361`, original line11, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The old outputs/pico4e/station296-poll-schedule-gemini.md claims the absence of command 16 formatting. This is refuted by the direct path 0xe6e6–0xe700: a two-byte body, r2=0x10, r3=2, call to packer 0x1658c. The latter writes the command number to +0xe, the length to +0xf, and copies the body to +0x10 (0x16610–0x1661c).

## K05738
Source `D0361`, original line13, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Origin of parameters: 0x10f30–0x10f36 stores the bytes of the incoming request; 0x10f48 sets r1=8 and calls 0xeadc. There, the second argument is stored at +3, the third at +4 in record 0x20004d88 + index*6. In 0xe6f0–0xe6f8, these fields are copied into the body in reverse order: [N, 8]. This is a verified path, not an assertion that all requests in all modes always use 8.

## K05739
Source `D0361`, original line15, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In our setup, station_v4_runtime.py appends [N, decimation_m], default=2. In FW189, 0x6100 stores pending N and C separately. 0x6df6–0x6e0a applies N only after C is exhausted; 0x6e94 decrements C. Therefore, C is a transition countdown; it cannot be called an independent period or expressed in microseconds without a trace. Replacing C=2 with 8 does not prove a fix for stable reception.

## K05740
Source `D0361`, original line17, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. Sequence and proofs of its completion

## K05741
Source `D0361`, original line19, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Command | Our ABI4 station | What is established in PICO / verification boundary |
|---|---|---|
| 9 | MCU confirms connection, seq0, body [1,0, chipID4]; host waits for product-info | Full identity of all native handshake branches is not proven in this iteration |
| 16 | First in configuration, [N, 2] | Verified native path [N, 8] above |
| 17 | [07], unless config overrides | 0x10f66/0x1194a send 0x11 with one byte from state/host request; a fixed 07 cannot be claimed for any native mode |
| 25 | [02] | 0x10f80/0x11962 send 0x19 with one byte from state/request; universal matching of value 02 is not proven |
| 19 | [00] | A separate native path 0x10456 is found, command 0x13 of length 1; no general fixed startup order follows from it |
| 10 | After configuration, phase synchronization is needed; in test 21 optics is turned off, state WAIT_SYNC | 0xac76–0xac9c: request by flags, capture TIMER4 CC1 (0x4001a000+0x544), call to 0x16084; there command=10, length=4, body is the passed capture |

## K05742
Source `D0361`, original line28, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Our peer_engine.py executes commands sequentially by transport ACK: configuration defaults to 17/25/19; runtime prepends it with 16. Transport ACK is the correlation of a response with publication, not a reading of the actual N or IMU state on the tracker.

## K05743
Source `D0361`, original line30, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the logs of 21, transport_acks=4 and waiting_camera_sync are consistent with configuration passing. The file *-commands.jsonl contains only the instrumented call 16: it is NOT a complete over-the-air trace of 9/17/25/19/10 commands. Inter-command delays cannot be reconstructed from it. Thus, the nominal 211,53 polls/s of the native grid are not a measured comparative result under the same conditions.

## K05744
Source `D0361`, original line32, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. TX and schedule

## K05745
Source `D0361`, original line34, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The native slot index in 0xe3fa–0xe416 increments modulo 5. Our service test used one slot, a requested interval of 4727 µs, but the actual average was about 125 slots/s. According to 21 data, adjacent answered TXs are around 4727 µs apart; some pairs with an index difference of 7 have 42,5 ms instead of 33,089 ms. This proves schedule stretching between these answered transmissions, not the tracker RX turn-on frequency.

## K05746
Source `D0361`, original line36, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In native code 0x14880–0x148c8, TIMER0 CC0 is computed with rounding to a grid of 625 units, and PPI connects COMPARE0 to RADIO TASKS_TXEN. The units of this specific path and its applicability to each tracker poll are not established in this iteration. The old interpretation of '625 µs BLE' is not proven: conversion requires the corresponding PRESCALER and call path.

## K05747
Source `D0361`, original line38, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Our C code triggers TXEN in software in MPSL START. Equality of the requested interval to the native period does not prove equality of phases. New timestamps refer to software TXEN and exist only for captured responses.

## K05748
Source `D0361`, original line40, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The tracker's TIMER0 has PRESCALER=3: 18910 ticks = 9455 µs, not 18910 µs. The RTC path uses divider 30,5. Do not mix units of different timers.

## K05749
Source `D0361`, original line44, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Our mailbox always sets S1 bit1; bit0 acknowledges reliable writes; receive allows S1=2/3. These rules were verified in the source code. The full native algorithm for constructing S1, including startup and idle poll, was not reconstructed in this iteration; claiming byte-by-byte equivalence is not permitted. FW189 distinguishes the sleep branch by bit1, so arbitrarily changing it for testing is not allowed.

## K05750
Source `D0361`, original line46, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 5. What to do next

## K05751
Source `D0361`, original line48, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Prepare a separately verifiable radio-only phase-sync path for command 10, without turning on cameras or modifying firmware. The exact origin of capture, units, transmission timing, native response, and phase proof criterion are required. Turning on optics for the sake of command 10 is not permitted: previously, optics-on led to assert 106/501, with the cause unestablished.
2. For one future agreed-upon session, add in advance a complete trace of sent configuration writes, ACKs, and all TX STARTs, including unanswered transmissions. Current answered-only timestamps are insufficient for diagnosis.
3. Verify native S1 and the PPI path before implementing phase transfer. C=8 is acceptable as a candidate for reproducing the transition, but not as a promised percentage fix.
4. Do not perform new N/period sweeps and do not proceed to OTA based on the outcome of this reconciliation.

## K05752
Source `D0361`, original line53, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Artifacts and verification

## K05753
Source `D0361`, original line55, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

outputs/service-fast-poll22/ contains address fragments of both listings and SHA256 of sources/images in manifest.json. This is a static check, not a dynamic emulation of the listings. There are no production edits; tests do not replace hardware proof. The master log and AGENTS have been updated with current boundaries of knowledge.

## K05754
Source `D0362`, original line1, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 23: command 10 without cameras — preparation and hardware test

## K05755
Source `D0362`, original line3, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. The user explicitly authorized preparing and running the test on the powered-on Frame/tracker. One limited run; OTA/flash/reset/unlock were not performed. After the assert, a repeated RF run was not conducted.

## K05756
Source `D0362`, original line7, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Command 10 received 14 transport acknowledgments out of 15 queued attempts; the last one was queued immediately before the assert. Prior to it, RX/slots=11,59%; after transmission started, =11,61%. There is no significant improvement in the observed section. The planned 60-second test was interrupted by stock assert 106/501 after approximately 32 counter seconds (34 s wall including processing/cleanup). The full 40-second section after synchronization was NOT completed.

## K05757
Source `D0362`, original line9, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Section by MCU counter_trace | Seconds | Slots | Received packets | Slots/s | RX/s | RX/slots |
|---|---:|---:|---:|---:|---:|---:|
| Before command 10 | 19,986 | 2511 | 291 | 125,64 | 14,56 | 11,589% |
| After start of command 10 | 11,535 | 1464 | 170 | 126,91 | 14,74 | 11,612% |

## K05758
Source `D0362`, original line14, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The division boundary is the last counter sample before the first phase_attempt; it is ~78 ms earlier than publication. The table uses counter deltas, excluding the first 30 slots before the first sample. Last host header: 4005 slots, 461 RX, 5 dropped, 3151 blocked-events. At fault snapshot: 4014 slots, 463 RX, 5 dropped, 3157 blocked-events. The number of START/slots is not an exact TX-END counter; RX here refers to MCU hardware CRC-valid packets, not the number of transport ACKs and not the number of IMU samples.

## K05759
Source `D0362`, original line16, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What was prepared

## K05760
Source `D0362`, original line18, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Diagnostic phase_probe.py was installed only in a separate loader_measure23. Production loader/PeerEngine/StationV4Runtime was not changed. The ELF was reused from the verified timestamp test 21: 2036 B, SHA256 3c9014d1894041aa4323e47cb6cd030de17aa71fdb75c2a37ad6fe0d9aafa8f6. The value of Record.number is a software TXEN timestamp only for captured responses; source_chip_id and identity guard are preserved.
