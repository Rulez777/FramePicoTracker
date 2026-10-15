# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05601
Source `D0340`, original line3, SHA256 `6f60974fb7f6a9d199906300e8fddc86cc90d9dcdc90a6d7b64b642ebfbeb382`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-09, Claude Opus 5.5. Controllers turned off by the user, tracker turned on.
Module phase78 with END check (ELF `8c314daf…5d272`), loader and host from
`phase67/release`. Read-only: without setter and without OTA, firmware was not written.
In runs 2 and 3, the session was closed, RAM/MPU restored, and module unloaded —
everything confirmed by reading. Evidence is located in
`outputs/phase78/attempt-control{1,2,3}/`.

## K05602
Source `D0340`, original line10, SHA256 `6f60974fb7f6a9d199906300e8fddc86cc90d9dcdc90a6d7b64b642ebfbeb382`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Run | Difference from Phase67 profile | Outcome |
|---|---|---|
| control1 | `controllers_off=true`, `min_controllers=0` | Rejected before RF: no quiet window in BLE schedule, busy profile is uniform, about 0.65 |
| control2 | plus `ble_phase_lock=false` | 60 s, 426 slots, 60 polls, 22 records |
| control3 | plus slot 900 (deadline 700 µs) | 60 s, 443 slots, 63 polls, 27 records |

## K05603
Source `D0340`, original line16, SHA256 `6f60974fb7f6a9d199906300e8fddc86cc90d9dcdc90a6d7b64b642ebfbeb382`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Observations

## K05604
Source `D0340`, original line18, SHA256 `6f60974fb7f6a9d199906300e8fddc86cc90d9dcdc90a6d7b64b642ebfbeb382`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Tracker is present.** In control3, product info was caught 3 times (443–445 µs):
   serial number [TRACKER_SERIAL], firmware 189, chip [TRACKER_ID].
2. **END check in phase78 works over the air.** Discovery response
   `0c0204001f0000000a013eed1979` (14 bytes) was captured 15 times at 409 µs,
   that is, after deadline 400. In coexist56/64 and phase67 with the old module, there were
   zero such captures. The user's caveat remains in effect: observation
   time does not prove that END occurred before 400 µs.
3. **Product info runs into deadline.** With deadline 400 (control2) — 0 captures,
   with 700 (control3) — 3 captures. The sequence in both runs is the same:
   discovery → empty `010300` on operating channel 2 → then silence or (with
   deadline 700) product info.
4. **Connection never completed.** In control3, the state remained
   `confirming`: IMU 0, mode not read. **Current tracker mode remains unknown.**
5. **Without controllers, the air is not free.** Frame radio is busy approximately 65%
   uniformly, likely searching for controllers. Without phase locking, MPSL gave only
   about 7 slots/s (versus about 167/s in Phase67) and about 1 tracker polls per
   second. Therefore, control2 and control3 cannot be compared by reception percentage either
   with Phase67 or with Service52, and "controllers off" is not an idle radio.

## K05605
Source `D0340`, original line37, SHA256 `6f60974fb7f6a9d199906300e8fddc86cc90d9dcdc90a6d7b64b642ebfbeb382`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Conclusions

## K05606
Source `D0340`, original line39, SHA256 `6f60974fb7f6a9d199906300e8fddc86cc90d9dcdc90a6d7b64b642ebfbeb382`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- The deadline of 400 µs is confirmed as a handshake blocker: product info does not
  pass through it even without controllers.
- A run with controllers enabled and a relaxed gate (variant 2) on the current
  600/400 profile will almost certainly repeat the drop at product info. It risks the
  controllers without providing new information, which is why it was not launched in this session.
- To achieve a deadline of about 500–700 µs with controllers, a quiet window of
  about 700–900 µs is needed, whereas the measured one is 600. The bottleneck is time, not the channel.
- A separate question: why, with deadline 700, did the connection still not reach
  CONFIGURING. Probably, about 1 polls/s does not fit within the host response timeout of 3 s.
  This is a hypothesis; it has not been tested.

## K05607
Source `D0341`, original line1, SHA256 `0178f8e8128f11acef10ab940c095ec75ac1dd7d520052126b85ef944490d463`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Phase78 detect1: tracker discovery with controllers rejected before RF

## K05608
Source `D0341`, original line3, SHA256 `0178f8e8128f11acef10ab940c095ec75ac1dd7d520052126b85ef944490d463`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-09. User request: run tracker discovery (presumably
mode1) with working controllers. Performed by Claude Opus 5.5.

## K05609
Source `D0341`, original line6, SHA256 `0178f8e8128f11acef10ab940c095ec75ac1dd7d520052126b85ef944490d463`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What was run

## K05610
Source `D0341`, original line8, SHA256 `0178f8e8128f11acef10ab940c095ec75ac1dd7d520052126b85ef944490d463`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Module `phase78/build/roy_station.elf` with END check before exit on deadline
  (ELF SHA256 `8c314daf…5d272`). Was not on the air prior to this attempt.
- Loader and host from `phase67/release` (`roy_ram_loader.py` 2a95747b…,
  `peer_engine.py` 22d585cb…). Phase67 profile unchanged: 6000×7, slot 600 µs,
  margin 200 µs, BLE phase wait 10 s, `ble_max_busy_fraction=0.05`.
  Peers 1/2 paused, optics disabled.
- Read-only: `tracker_desired_mode` not specified (schema1 request, without setter), no OTA.
- Protection "2 controllers / 150 ms" enabled.
- Exact command: `outputs/phase78/attempt-detect1/run.sh`.

## K05611
Source `D0341`, original line20, SHA256 `0178f8e8128f11acef10ab940c095ec75ac1dd7d520052126b85ef944490d463`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. First launch crashed on import (`No module named 'radio'`): there was no
   `PYTHONPATH` in `run.sh`. `openocd.log` was not created, debugger did not open.
   Log saved as `stdout-import-failure.log`.
2. Repeated launch with `PYTHONPATH=phase67/release`. BLE phase sampler collected
   3967 counts and found a quiet window of **600 µs** (center 1600 µs) at a threshold
   of 5% occupancy. The loader needs ≥ slot+200 = 800 µs, so it threw
   `RuntimeError: BLE quiet window 600us too short for 600us slots` before loading
   the module and before the MPSL session.
   - Report: `flash_written=false`, `mpu_restored=true`, `unloaded=false` (module
     was not loaded).
   - No loader/OpenOCD processes remained.
   - Controllers: 10 metric windows, both 99/99 changes, 0 invalid, 0 ms freeze.

## K05612
Source `D0341`, original line33, SHA256 `0178f8e8128f11acef10ab940c095ec75ac1dd7d520052126b85ef944490d463`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Tracker mode, its presence, and reception were not measured.** This is not a result of module
phase78 on the air. Protection was not relaxed, there were no automatic retries.

## K05613
Source `D0341`, original line36, SHA256 `0178f8e8128f11acef10ab940c095ec75ac1dd7d520052126b85ef944490d463`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Conclusion

## K05614
Source `D0341`, original line38, SHA256 `0178f8e8128f11acef10ab940c095ec75ac1dd7d520052126b85ef944490d463`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The window of 600 µs matches Phase72 and Phase76, where each half of the measurement also yielded
600 µs. With controllers and a 5% threshold, this slot gate of 600 µs will consistently
reject the launch. Further options are up to the user to decide:

## K05615
Source `D0341`, original line42, SHA256 `0178f8e8128f11acef10ab940c095ec75ac1dd7d520052126b85ef944490d463`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- **Control without controllers** with the same profile and phase78. The gate will pass, and the run
  will show whether product info/IMU is caught with a deadline of 400 µs.
- Changing the occupancy threshold or the slot+200 rule means relaxing protection.
  This is a separate decision, only with user consent.
- Reducing the slot is pointless: the deadline would become shorter than the shortest responses (226 µs).

## K05616
Source `D0341`, original line48, SHA256 `0178f8e8128f11acef10ab940c095ec75ac1dd7d520052126b85ef944490d463`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Evidence: `outputs/phase78/attempt-detect1/`, copy on Frame:
`~/pico-ram-work/phase78/attempt-detect1/`. SHA256 `radio-run.json`: `b8dbc7be…bde5c`.

## K05617
Source `D0358`, original line1, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Fast Tracker Polling Service Mode (controllers off) — Status 2026-10-08

## K05618
Source `D0358`, original line3, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

> Codex update, iteration20: hardware verification completed. See full report (original reference: `SERVICE_FAST_POLL_ITERATION20_2026-10-08.md`). Original1988 ELF returns prohibited REQUEST from low-priority callback and triggers assert106/472. Fixed2020 ELF uses request API and yields ~126slots/s, but only14–15RX/s, RX/TX~11–12%. With optics reached streaming, then another assert106/501; continuous radio stopped, RAM/MPU restored. Production ELF preserved. The original plan is left below; do not consider its predictions and the old Frame state as confirmed by the current result.

## K05619
Source `D0358`, original line5, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Project `K:\AI\pico`. Station in Roy RAM, trackers are NOT reflashed (this is only about polling from the station). Before working, read `AGENTS.md` and general log `docs/EXECUTION_STATUS_2026-10-06.md`.

## K05620
Source `D0358`, original line7, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Idea (from user)

## K05621
Source `D0358`, original line9, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

When controllers are turned off and their drop is not a concern, the Roy module does not need to adapt to the controllers' BLE grid. A separate service mode "for the tracker":
- request slots from MPSL so as not to waste time on failures between them;
- poll the tracker according to its own grid of 4727.5 µs (like the stock PICO station), rather than the grid of 6 ms;
- keep reception open for the entire remainder of the window.
This should give close to ~120 samples/s per tracker instead of the current ~30%. Needed for service operations: pairing, firmware transfer — where a clean channel without interference is important.

## K05622
Source `D0358`, original line15, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The mode must be SEPARATE (service), production mode (BLE locking, coexistence with controllers) remains the primary one. In the Frame application, there must be an ability to select the mode according to the situation (not done yet).

## K05623
Source `D0358`, original line17, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What was established on data and the bench (2026-10-08)

## K05624
Source `D0358`, original line19, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Bench: headset turned on, 1 tracker (serial [TRACKER_SUFFIX], chip [TRACKER_ID], slot 0), controllers turned off, service mode (min_controllers=0, freeze off).

## K05625
Source `D0358`, original line21, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **The hypothesis about series was confirmed.** Tracker responses come in deterministic series, intervals are multiples of our slot; series lengths up to 50–72 consecutive responses. When we hit the tracker's reception window, it re-anchors to our packet and holds on for many polls. There are massive drops of exactly ~108 ms — fallouts from the window.

## K05626
Source `D0358`, original line23, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2. **The ~30% ceiling on stock firmware is due to grid mismatch.** The internal period of the tracker receiver is ~18.9 ms (in decimation) and is not a multiple of the controller grid of 3000 µs, the window drifts ~910 µs per period → aligns with the quiet zone ~1 times out of 3. Locking to the last response will make this ~30% stable, but will not raise it higher as long as the tracker firmware is stock and we share the air with controllers.

## K05627
Source `D0358`, original line25, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

3. **MAIN FINDING for service mode.** Without BLE locking, the station gets only **~11–13 MPSL slots per second** (one slot per ~108 ms SoftDevice activity period), at any interval. The reason was found in the MCU: when MPSL rejects a slot (signals 5–7: BLOCKED/CANCELLED/SESSION_IDLE), the module returns action NONE and **the session falls asleep until restarted by the host**. In production mode, BLE locking lands in the quiet window, almost no blocks → ~166 slots/s. Without locking, blocks are constant → session sleeps all the time → ~13/s. Hence the 108 ms drops in the data, and why the tracker in service mode does not reach streaming (almost no samples).

## K05628
Source `D0358`, original line27, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Conclusion: for service mode to work, upon slot blocking one must NOT fall asleep, but immediately request the earliest free slot (EARLIEST). Then the slots will fill all gaps between SoftDevice events → expected ~150+ slots/s, tracker streams at full speed.

## K05629
Source `D0358`, original line29, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The controllers are off at the same time — there is no one to interfere. What exactly creates events every ~108 ms with controllers turned off has not been definitively established (SoftDevice is likely advertising/scanning to reconnect them, or the connection with the controllers has not been torn down at the BLE level).

## K05630
Source `D0358`, original line31, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What was done in code (on PC, partially on Frame)

## K05631
Source `D0358`, original line33, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Files changed on PC (`K:\AI\pico\pc\radio\ram\`):
- **`roy_ram_loader.py`**: pass-through of `slot_us` from station-config (line 446); setting service mode flag `D(1896)` from `station_config['service_poll']` (after copy peer_state). Deployed to Frame.
- **`roy_ram_station_v4.c`**: in the signal handler 5–7 with `D(1896)&&!s->stop` — re-request of EARLIEST slot (`request.type=0, a=slot_us, b=100000`, `reply.action=3`). Also `r->reserved=0` instead of the diagnostic slot number. **DOES NOT BUILD: module overflowed by 39 bytes** (limit 2045). Working ELF on Frame was NOT changed (old Oct-7).
- `controller_guard.py`, `station_v4_runtime.py`, `peer_engine.py`: synchronized on Frame (they contained `forbid_controllers`, `enable_dual_mode` from previous sessions; due to desync the station crashed at startup; now reconciled).

## K05632
Source `D0358`, original line38, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Scripts on Frame (`/home/steamos/pico-ram-work/state/`): `fstat.py` (dev|cfg|live|opt), `watchlive.py`, `replytiming.py`, `replyrate_now.py`, `service_mode.py` (on/off — min_controllers=0 + freeze off), `fast_poll.py` (on/off — fast polling via config). Config backups: `station-config.json.before-vision`, `.before-fastpoll`.

## K05633
Source `D0358`, original line40, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What remains to be done

## K05634
Source `D0358`, original line42, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Implement "do not fall asleep on blocking".** Two paths:
   - **MCU (clean, but no space).** Need ~40 bytes. Removing radio failure protection `if(!R(RADIO,0x550)){s->stop=1;stop_radio();}` is not allowed (blocked as relaxing safety, and rightly so). Must find ~40 bytes otherwise: factor out duplicate code, simplify phase correction (not needed in service mode), or a separate `ROY_SERVICE_MODE` build with `#ifdef`, where BLE locking is removed (saves a lot). I recommend a separate build: the service module is simpler than the production one, BLE correction, the 7-slot cycle, and part of diagnostics are not needed → space will appear, and the production module remains untouched.
   - **Host (without MCU editing, faster to test).** In the `roy_ram_loader.py` loop (around line 709, next to the heartbeat on DATA+1608), detect that the session is falling asleep (s->phase==3 blocked, or slots is not growing), and re-invoke `invoke_native(debug,entry,1,...)` to request a new slot. Ceiling ~host loop frequency. Check whether the slot rate rises. This is a cheap experiment — do it FIRST to confirm that re-requesting raises the rate at all, before MCU surgery.
2. **Run the service config and measure.** Config: `service_poll=true, ble_phase_lock=false, tracker_poll_decimation=false, interval_us=4727, slot_us=900, slot_cycle=1, mpsl_priority=0`, coexistence min_controllers=0 freeze off. Success criterion: slots/s → 100+, tracker reaches streaming, samples hundreds/s (see `watchlive.py`, `replyrate_now.py`). For 3 trackers: interval ~1575 (4727/3), slot_cycle=3 — but validation in `station_v4_runtime.initialize_v4` requires interval 3000..10000, needs to be relaxed for service mode.
3. **Move mode selection into the Frame application** (`pc/frame_app/main.cpp`, buttons + `pc/radio/frame_debug_agent.py` endpoints). Production mode / service mode. Service mode — only with controllers off; in it, `forbid_controllers` in `controller_guard.py` already knows how to provide a safeguard (error if a controller is connected during a service run).
4. **Measure slot rate correctly.** The `radio-live-records.jsonl` log is limited by the SWD ring read speed — this is NOT the true response rate. True indicators: the `slots` counter in radio-live.json (increasing = slot rate) and tracker `samples` (IMUs arrived). Ring of 10 records, host reads ~every 6 ms.

## K05635
Source `D0358`, original line49, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Frame state now

## K05636
Source `D0358`, original line51, SHA256 `5b084ee9ade196b8a2cde22bba0841072f1bee928ef803f32596c3552b2605c7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Production config restored (`fast_poll.py off`, `service_mode.py off`), station stopped, working ELF was not changed. Deployed .py files on Frame are reconciled and contain service parameters (disabled by default, production mode unaffected). Disk ~47 GB free.

## K05637
Source `D0359`, original line1, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Service fast polling — hardware measurement, iteration 20

## K05638
Source `D0359`, original line3, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-08. Performer: Codex. User authorized building, preserving the production ELF, service substitution, and radio traffic measurement. Tracker and Roy firmwares were not written.

## K05639
Source `D0359`, original line7, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Slot rate raised by hardware: approximately 6,7 → 126 slots/s, about 19 times.** Reception grew approximately 2 → 14–15 packets/s, about 7 times. However, the ratio of received packets to completed TX remained **11–12%**, rather than approaching 100%. The mere increase in the number of slots did not resolve the loss problem.

## K05640
Source `D0359`, original line9, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The initial service build of **1988 bytes is incorrect**: the first MPSL rejection leads to an assert due to returning REQUEST from a low-priority callback. The corrected standard service build is **2020 bytes**, the measurement one is **2036 bytes**, both below the limit of 2045.

## K05641
Source `D0359`, original line11, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Radio without optics passed limited runs of 60/20/40 seconds with successful session closure and RAM/MPU restoration. When optics were turned on, the tracker reached `streaming`, but after approximately 10 seconds another Roy assert occurred, module106/line501. This mode cannot be considered stable.

## K05642
Source `D0359`, original line13, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

After completion, **radio is stopped**, loader/OpenOCD are not running. The corrected service ELF and radio config have been saved and installed for the next run; the production ELF has been preserved without changes. Autostart was not enabled.

## K05643
Source `D0359`, original line15, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Bench and measurement boundaries

## K05644
Source `D0359`, original line17, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Steam Frame: SSH `steamos@[DEVICE_IP]`, `/home/steamos/pico-ram-work`.
- Tracker slot0, chipID LE `[TRACKER_ID]` / `[TRACKER_ID]`, serial `[TRACKER_SERIAL]`, stock FW189.
- Valve controllers on tracking_watch reading: both disconnected/invalid; HMD valid. Verification repeated after experiments.
- Slots 1 and 2 explicitly disabled by config `diagnostic_pause_peers=[1,2]`.
- Radio is launched in Roy SRAM, via stock MPSL; flash untouched.
- In config `controllers_off=true`: guard terminates custom RF when a controller connects. Guard checks were not disabled.
- OTA was not run, hardware OTA readiness gate and whitelist unchanged.

## K05645
Source `D0359`, original line25, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What is specifically counted

## K05646
Source `D0359`, original line27, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Slots**: MCU counter `State.slots`, DATA+12, increments on START signal. These are granted slots, not merely host log lines.
2. **RX packets**: MCU counter `State.packets`, DATA+16. Capture is invoked on RADIO END and CRCSTATUS=true. The counter includes received frames, including handshake; this is not only IMU and not only ACK.
3. **TX-END**: in a separate measurement variant, `D(1904)++` was added when RADIO EVENTS_END is observed after transmission. Before each transmission, END is cleared. These are observed Roy transmission completions, not proof of receipt of each packet by the tracker.
4. **Dropped**: MCU counter DATA+20; includes ring overflow/frames that are too long. In successful measurement runs, it equals 0.
5. **Samples**: IMU samples decoded by the host; they are not equal to the number of RF packets.

## K05647
Source `D0359`, original line33, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The number of packets received by the tracker cannot be directly measured with these tools. There are completed Roy TXs and Roy received replies. **RX/TX is the observed ratio of counters, not an exact delivery probability**: there are discovery/handshake transmissions, autonomous messages, and possible multiple frames in a single slot.

## K05648
Source `D0359`, original line35, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Host `reply_ratio` and `transport_acks` were not used as the primary counter: ACK tag updates and ring reading may count a different set of events.

## K05649
Source `D0359`, original line37, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Header and TX-END snapshots are read sequentially, not atomically. In the N0 run, the last header contains 5032 slots, while the TX-END read slightly later contains 5035: this is a time skew of readings, not physical transmission outside of slots. The rates below are calculated from counter increments and the monotonic span between the first/last snapshots, with rounding. Totals refer to the last available snapshots, not to the guaranteed last RF event before closing.

## K05650
Source `D0359`, original line39, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Exact results

## K05651
Source `D0359`, original line41, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Variant | Counter span | Slots, total | TX-END, total | RX, total | Slots/s | RX/s | RX/TX |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Production ELF, service unlocked config, baseline | 19,871 s | 136 | not measured | 39 | 6,74 | 1,96 | not measured |
| First fix: API only on SESSION_IDLE | 59,841 s | 399 | 399 | 111 | 6,63 | 1,85 | 27,82% |
| Fix: API on BLOCKED/CANCELLED/IDLE, without command16 | 59,999 s | 7584 | 7584 | 864 | 125,98 | 14,40 | 11,39% |
| Same fix, regular ELF without measurement instrumentation | 19,974 s | 2532 | not measured | 270 | 125,26 | 13,52 | RX/slots=10,66% |
| Same fix, explicit command16 N=0,M=2 | 39,967 s | 5032 | 5035 | 590 | 125,25 | 14,69 | 11,72% |
| N=0 + optics, regular ELF, aborted by assert | 9,456 s | 1220 | not measured | 150 | 126,06 | 15,76 | RX/slots=12,30% |

## K05652
Source `D0359`, original line50, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The baseline does not reproduce the historical configuration with active controllers: it is a control experiment on the same test bench with controllers turned off and without BLE phase lock. Its 28,68% RX/slots cannot be passed off as the previous "10%" or compared as the same measurement. The slot rate and absolute reception are compared against it.

## K05653
Source `D0359`, original line52, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

IMU maximum in observed live snapshots: 4 in baseline; 777 over the one-minute fast run; 520 over the N0 run. This is about 13 samples/s, not the expected 120. In the radio test, optics are disabled, so the state remains `waiting_camera_sync`, even though IMUs are arriving. In a separate test with optics, state reached `streaming`; pose_valid was false the entire time. 6DoF is unconfirmed.

## K05654
Source `D0359`, original line54, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Found build error 1988

## K05655
Source `D0359`, original line56, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A fresh build of the original source gave 1988 bytes and ELF SHA256:

## K05656
Source `D0359`, original line60, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The same ELF was already installed on Frame prior to the actions of this executor. Consequently, the old artifact "working ELF was not changed" is outdated. For safety, both the actually installed ELF and a separate production `roy_station_combat.elf` have been preserved.

## K05657
Source `D0359`, original line62, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The first measurement variant of 2016 bytes received 2 slots, after which a stock assert was intercepted:

## K05658
Source `D0359`, original line69, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In stock.bin @ `0x103b6`, the callback is invoked with signal5. After returning, action is read; a non-zero value leads to assert106/472. This matches official mpsl_timeslot.h (original reference: `https://github.com/nrfconnect/sdk-nrfxlib/blob/main/mpsl/include/mpsl_timeslot.h`): low-priority signals must return NONE.

## K05659
Source `D0359`, original line71, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

After interception, the loader performed cleanup: session_closed_verified, ram_restored_verified, mpu_restored, unloaded=true; flash_written=false. Removing assert/breakpoint protection or continuing with a faulty callback was not used.

## K05660
Source `D0359`, original line73, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Fix

## K05661
Source `D0359`, original line75, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Only the service `#ifdef ROY_SERVICE_MODE` in `pc/radio/ram/roy_ram_station_v4.c` was changed:

## K05662
Source `D0359`, original line77, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- callback preserves reply.action=NONE;
- BLOCKED/CANCELLED/SESSION_IDLE invoke the existing request API `0x18949`, with EARLIEST(length=slot_us,timeout=100000);
- a successful request transitions phase to1, so that host does not re-request an already restored session upon a new snapshot;
- a non-zero API code sets stop rather than being hidden;
- stop disallows a new request.

## K05663
Source `D0359`, original line83, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The intermediate variant, which waited only for SESSION_IDLE, did not increase the rate: 399 slots in a minute. Therefore, the final fix already acts on BLOCKED/CANCELLED. In stock dispatch @ `0x10394`, the session is transitioned to a requestable state before this callback; the API @ `0x18948` accepts this state. The admissibility of this specific behavior was additionally confirmed by limited hardware runs of the final variant.

## K05664
Source `D0359`, original line85, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Assert protection was not added in this iteration. Both the original deployed loader and the measurement copy check fault after entry and in the main loop. `--swift-poll` sets internal `args.rx_only=True` to reuse the fault/cleanup lifecycle, even though the radio is actively transmitting. The measurement copy adds counter reading, not a new assert interception policy. Today's halts cannot be considered solely the consequence of intensified monitoring; comparison with a specific older executing loader requires its preserved version.

## K05665
Source `D0359`, original line87, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Radio failure protection, fixed RX end, heartbeat, cleanup, and MPU/stock guards are preserved. The linker limit was not increased.

## K05666
Source `D0359`, original line89, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The production ELF on Frame was not rebuilt over the working file. A control build without the service define was preserved separately in `combat-rebuild20/`; it was not installed. The current production file was preserved byte-for-byte, SHA256:

## K05667
Source `D0359`, original line93, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## N=0 and config

## K05668
Source `D0359`, original line95, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`tracker_poll_decimation=false` **skips command16**, rather than sending N=0. The previous value of N on the tracker itself prior to verification is unknown; the hypothesis of a stored large N was tested, not accepted as fact.

## K05669
Source `D0359`, original line97, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In a separate run, `tracker_poll_decimation=true`, `tracker_poll_decimation_n=0`, and `tracker_poll_decimation_m=2` were explicitly enabled. Configuration reached waiting_camera_sync, but RX/TX remained 11,72%. Consequently, setting N=0 alone did not eliminate losses.

## K05670
Source `D0359`, original line99, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Clarification: this is the **requested N=0**, not a value read from the tracker. Transport ACK does not prove application-level adoption of the command; possible deduplication and stored mode are not separately ruled out.

## K05671
Source `D0359`, original line101, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Additional analysis of 590 RF records of the N0 run: 589 on channel2, one on78; all hardware CRC valid. Between responses, the most frequent gap is about42,5 ms (138 intervals with rounding0,1ms), followed by4,7ms (79),42,6ms (40),33,2ms (30). Between answered slots, gap7 occurs197 times, gap1 —125, gap3 —64. Observations of the response end relative to slot start: min226, median447,max759µs. These times were captured by callback, not by hardware capture ADDRESS.

## K05672
Source `D0359`, original line103, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Responses sometimes occur consecutively at a distance close to4727µs, followed by long dropouts. This contradicts the model of a "constantly clean regular grid with a response to every poll". The interval42,5ms is approximately equal to9×4727,5µs, but on its own does not prove the current N: such intervals can also result from station/window skips. Radio interference as the cause of these dropouts is unproven. One must distinguish between actual START/TX, the tracker's listening window, and confirmed adoption of command16; this report does not conflate them into a single slot rate.

## K05673
Source `D0359`, original line105, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Radio config of the successful N0 test:

## K05674
Source `D0359`, original line131, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is a nominal integer distance of 4727 µs, not a proven grid of actual START 4727,5 µs. MPSL denials/re-requests break regularity: the one-minute fast run contains 5998 low-priority blocked/cancelled/idle signals. The cause of the remaining losses and the effect of jitter are not determined here. Margin100 was tested only with controllers turned off; it is not declared safe for co-existence mode.

## K05675
Source `D0359`, original line133, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Separate failure with optics

## K05676
Source `D0359`, original line135, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The planned 20-second test with `optics_enabled=true` completed with code1. Approximately 9,46 seconds of usable counter snapshots: 1220 slots,150 RX, dropped4. In the fault snapshot, there are already 1227 slots,152 RX — counters continued to increment after the last host snapshot.

## K05677
Source `D0359`, original line137, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

State reached streaming, there were real phase ACKs. Then stock assert: r0=106,r1=501,PC=0x13e54,LR=0x10507,CFSR/HFSR=0. Stock @ `0x10428..0x10434` checks internal state before processing the overstayed path and triggers this assert on mismatch. The specific root cause — callback duration, IRQ latency, scheduler/host/optics interaction, or something else — is unproven. It cannot be attributed solely to the optical stream: there is a correlation with enabling optics, not an isolated causal test.

## K05678
Source `D0359`, original line139, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This run is not counted as a successful 20 seconds. Automatic transition to the next RF run was canceled. All cleanup flags are true, flash_written=false. Continuous mode with optics was not left running.

## K05679
Source `D0359`, original line141, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Checks and saved files

## K05680
Source `D0359`, original line143, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- 11 scenarios execute the compiled callback in Unicorn: NONE for low-priority signals, API session/request, stop, API error; old1988 reproduces the forbidden REQUEST on signal5. Native API is mocked in the model; this tests caller code, not scheduler emulation.
- `pytest pc/tests/test_roy_ram_loader.py pc/tests/test_station_runtime.py`: **25 passed**.
- Cleanup verified after each hardware run, including both asserts. All completed reports: session_closed_verified/ram_restored_verified/mpu_restored=true, flash_written=false.
- No loader/OpenOCD processes remain after tests. Production ELF retained SHA.
