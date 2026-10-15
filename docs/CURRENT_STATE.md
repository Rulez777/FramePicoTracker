# Current evidence and exact continuation state

## Goal and sequence

Build real standalone optical/inertial body tracking on Steam Frame using onboard Roy, no external receiver and no PICO-headset runtime bridge. First establish one clearly stable tracker, then enroll/verify the other two and body roles, then simultaneous controller coexistence. Do not divert into mixed firmware or new diagnostic OTA while the protected first-tracker baseline remains the agreed target.

## Protected baseline

- PICO Swift application FW189, installed authored diagnostic10 image:98868bytes,SHA256`040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d`. This package contains only its stock-relative difference.
- Exact stock application:96532bytes,baseVA`0x1000`,SHA256`477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`.
- Original210 Roy module source: `pico:outputs/phase210/roy_ram_station_v4.c`, sourceSHA`3db4e36a99648cca4c1f4af175a694b3bc0d8b769cd18ea7f3d56c177385163f`. Executed ELF source reference: `pico:outputs/phase273/optical-pass/build/roy_station.elf`,SHA`d8328717a5f6b757fd002ad6622e4fa5f07c45a1de935f6631267792deb286a3`. Only the first peer is admitted. Copied historical three-peer release C is not its executed provenance.
- Roy stock reference:237920bytes,SHA`0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50`; image/build-specific SRAM loader, no Roy flash modification.

## Latest radio/application result

Saved actual393-v2: one150s RF-only Game-mode app check;16 request-matched fresh nonce replies,15 distinct nonces confirming mode1/N2/pending0 over140.104938491s. Child exit0 and all four actual MCU restoration barriers=true. Radio was inactive and the consumed catalog disabled afterwards. IMU0/optical0/posefalse were deliberate test scope, not proof that the project never received IMU or optical candidates historically.

The earlier391 app detection retained2998 replies over10149 slots but lacked persisted nonce correlation, so it was not completed applied-mode proof.392 repaired an ACK deadlock;393-v2 supplies bounded hardware confirmation. Do not repeat it merely to reproduce the same result.

Historical235/236 did receive real IMU and publish optical pilot measurements under their own conditions. Sparse delivery, calibration gaps, causal identity/latency and end-to-end quality prevent calling those complete inertial fusion or stable body tracking.

## Newer optical artifacts than CURRENT_WORK

The private `docs/CURRENT_WORK.md` still describes the passive stereo probe as awaiting readiness. Saved `outputs/optical-latency-review/passive-stereo-result.json` proves it completed:900 packet-matched stereo pairs, every row rejected with fewer than four stereo-consistent points, zero model fits/inliers, no optical identity or publication, camera and watcher closed. No Roy/RF/OTA/IMU/LED actions were used. Recalculated intake age median67.4894885ms/max88.351313ms; completion age median77.6228725ms/max99.39342ms; all900 completion ages are below250ms. Geometry compute median9.765291ms/max24.60589ms. This particular result is not another age-only failure. See verification/LATEST_MEASUREMENTS.json.

Pixel sampling on39 frames per upper stream at actual37.233334us exposure found no frame with four threshold180 blobs or four local-contrast approximation blobs. Maximum ordinary blobs were0 and1. This establishes insufficient detected markers in the sampled short-exposure data, not proof of no photons or a uniquely identified root cause.

The actual camera-only latency result contains1618 exact packet-timestamp-matched frames, clean close, RAW age median66.84ms/max85.13ms and decode-to-consumer median1.45ms/max9.06ms. MONOTONIC-comparison ages are negative; do not mix clock domains. The later passive probe measured a current-boot RAW mapping with360 samples, uncertainty500000ns and causal fractions1/0/0 for RAW/MONO/BOOTTIME. Another boot needs another measurement.

The exposure-profile probe is PREPARED, not a completed result. Its candidate temporarily uses native profile3/power300, requires measured upper exposure750–850us, then restores original native settings/power. At packaging, `profile-probe-started.json`, `profile-clock-evidence.json` and `profile-stereo-result.json` were absent. A saved settings hash is not a fresh remote read. No readiness or authorization is carried forward by this handoff.

## Offline improvements

Strict candidate-age rejection avoids unnecessary geometry on already identity-stale pairs without widening guards. It is isolated, not deployed. Vectorized refinement on100 exact recorded simultaneous pairs preserved labels, inliers and all decisions; baseline median19.417345ms/max304.010661ms, variant8.2135965ms/max177.083511ms, maximum numeric difference3.9218977458e-12. Benchmark clock/head basis was explicitly simulated; these are not live freshness or world-pose results. Ray memoization did not improve the median and remains off.

## Next concrete blocker

Inspect exposure/marker visibility using the already prepared, unchanged-guard profile probe only after a new direct readiness answer, sealed dependencies and recovery. Expected observations are actual750–850us exposure and sufficient measured stereo markers/fits. A passive fit would still be unassigned geometry, not causal tracker identity or a published6DoF pose. If the expected exposure is not applied, stop rather than sweeping settings. If exposure is correct but fits remain absent, investigate phase/LED visibility/association from that changed evidence.

Full-body tracking, stable current optical+IMU fusion, three-tracker roles and simultaneous joint-controller quality remain unachieved. No device/live-state claim was refreshed while assembling this package.
