# Authors and acknowledgements

## Project origin

**L8OLOOE** started and led this research, developed the project with AI assistance, ran the Frame integration work and experiments, and assembled the continuation handoff.

[X](https://x.com/L8OLOOE) · [VRChat profile](https://vrchat.com/home/user/usr_937434c0-4a82-46bb-9457-f21cfa31ad6c)

Preserve the original author and contributor notices when sharing or extending the work. Add your own contribution honestly. A fork does not make the prior work yours.

## A focused contribution from Hinata Reira

[Hinata Reira](https://x.com/hinata_reira) encouraged me to take on the project and shared `pico_tracker_packets_en.md`, dated October 6, 2026. That packet reference was helpful in these specific areas:

| Reference sections | How they helped |
| --- | --- |
| Sections 2 and 3 | Cross checking the connection and IMU enable sequence, record framing, additive checksum, command counters and notification acknowledgement behavior. |
| Section 4 | Checking the 18 byte IMU sample layout, unit conversions, 16 MHz sync timer and 533333 count wrap. The corresponding scales and wrap appear in our `source/pc/optical/imu_calibration.py`. |
| Section 5 | A reference for command 10 phase sync, command 14 light scheduling and the reported skipped LED pulse after a sync. This informed timing questions; it does not validate our current camera synchronization. |

This acknowledgement is limited to that reference and encouragement. It does not attribute the Roy SRAM modules, Frame app, camera pipeline, firmware modifications, handoff, or our hardware results to Hinata. Those have their own project history and evidence.

The reference's reported hardware used an nRF52840 dongle. Those observations must not be presented as our onboard Roy results. Its static analysis used FW178 and PhoenixSta 2.88 while its trackers used FW189, so the version boundary matters.

The original third party prose is not relicensed here. Public ledger entries originating from that reference link to our attributed factual digest instead. Original document hashes, source cards and historical entry IDs remain available for provenance. Ask the author for the original document and any redistribution permission.

## Tools and external work

AI assistants were used for coding, research organization, translation and review. Their output was treated as work to inspect, not independent hardware proof. Grok and Gemini performed bounded reviews; the exact limits are recorded in `verification/`.

The overlay depends on frame-mic-tuner at the recorded revision and retains its upstream MIT terms when acquired. OpenVR and other dependencies keep their own licenses. They are not bundled here. Published geometry and filtering methods are research foundations, not claims of inventing those algorithms.

The banner is AI generated editorial artwork. The avatar screenshot was provided by L8OLOOE to identify the author, with no transfer of rights to any underlying avatar model or other creator assets.
