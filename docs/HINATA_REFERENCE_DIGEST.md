# Attributed packet reference digest

This is an original factual digest of the packet reference supplied by [Hinata Reira](https://x.com/hinata_reira), `pico_tracker_packets_en.md`, dated October 6, 2026. The original wording is not reproduced or relicensed. Its provenance is preserved by source cards D0021, D0146 and D0760 and the catalog hashes. [CREDITS.md](../CREDITS.md) explains the limited contribution to this project.

These are reference observations, not new tests by this publication. The document's hardware reports concern FW189 C2310 trackers and a custom nRF52840 dongle. Its static analysis concerns FW178 and PhoenixSta 2.88. Do not silently transfer those conclusions to the current onboard Roy baseline.

## Radio and connection facts

The reference describes nRF52 mode4 modulation at 2 Mbit, a five byte address, CRC16 with initialization 0xffff and polynomial 0x11021, and disabled whitening. The proprietary framing has a length field and an S1 nibble before payload data. Discovery uses 2401, 2437 and 2477 MHz. A connected channel mask selects even channels from 2402 MHz onward; the document's hardware setup used only 2402 MHz.

A peer address is derived by adding the station and tracker identifiers byte by byte modulo 256, then reversing the base bits. The prefix is derived from the reversed slot number. The connect request selects protocol version, command capacity, frequency mask, station identity and slot. The longer reconnect form also binds the target identity. Those identities and the original captured packets are not supplied by this digest.

The static station schedule has roughly 945.5 microsecond slots and a per tracker cadence of about 4727.5 microseconds. After a station packet, the reference describes an RX off interval beginning near 0.46 ms and ending near 3.99 ms, or about 4.39 ms for some S1 values. A connected peer can time out after 200 missed periods. Before confirmation, the reference gives a much shorter five period limit. These are version bound timing facts, not a proposed Frame schedule.

The station's S1 low bit acknowledges records from a previous reply. Another bit affects receiver timing. The special bit3 path interferes with command10 and should not be confused with ordinary polling. Replies echo S1, which can support matching but does not independently prove the desired mode was applied.

## Enabling data and reading records

The reported sequence progresses through discovery, identity, connect, peer address selection and command9 confirmation. Product information arrives as an event under command1. Commands17, 25 and 19 configure IMU behavior, IR behavior and notifications while polling continues. Command10 supplies phase synchronization. A command22 notification must be acknowledged before raw command2 sample output becomes active. The document describes both the normal S1 acknowledgement route and a command4 completion route. Disconnection clears the enable state and requires the sequence again.

Connected payloads contain records with a sequence counter, command number, body length and body. An additive sum8 is separate from the radio CRC. The lower six sequence bits wrap after 64 values; commands that enforce ordering reject an unexpected value. Empty polling replies can consist of only the additive check byte.

Command2 accepts body sizes 6, 18, 24, 36 or 42 bytes, representing up to two 18 byte samples with an optional six byte auxiliary block. Other sizes are malformed. In each sample, signed 16 bit gyro occupies offsets 0, 2 and 4; acceleration offsets 6, 8 and 10; temperature offset12; a 24 bit sync timestamp begins at14; and an eight bit sample counter is at17.

The reference gives acceleration scale 0.009580078534781933 m/s² per count and gyro scale 0.002130528911948204 rad/s per count for the relevant sensor branch. Temperature is converted as raw divided by 132.48, plus25 °C. Another sensor branch uses different scaling, so these must not become universal device assumptions. The exact sensor part was not established by the reference.

The sync counter uses a 16 MHz domain and wraps after 533333 counts, about one 33.3 ms frame, rather than at the 24 bit storage limit. The sample counter wraps after256 and supports gap and duplicate detection. The reported sample period is about4.98 ms, near200 Hz. The document describes an approximate sensor to body axis mapping and separately estimated gyro bias; this is not a replacement for per device calibration. The physical meaning and units of the six byte auxiliary block remain unconfirmed.

Related commands include command11 for loss recovery, command16 for polling decimation, command17 for IMU mode and command22 for state or orientation notification. Modes beyond the reported07 setting and the orientation component convention remain unresolved.

## LED scheduling and limits

Command10 aligns tracker time and affects LED pulse timing. Command14 sets the light schedule using a nonzero online count; zero is invalid as a divisor. Command25 has state dependent IR behavior, so value0 should not be described as a universal off switch. Command26 returns magnetometer calibration according to the static reference, not an IR control.

The reference's static stock sync interval is50 ms; its own host experiment used a much slower interval near0.8 s. It reports a skipped LED pulse after a phase correction. Which camera frame becomes dark depends on when correction takes effect relative to exposure, with an approximate half frame boundary near16.5 ms. The next frame rule alone is insufficient. This is useful timing context, not proof of causal identity in the current camera pipeline.

The described tracker ring stores eight samples and each reply carries at most two. The reference infers that recovering every200 Hz sample needs more than100 successful polls per second per tracker. Its three tracker dongle setup received about120 to135 samples per tracker per second, with loss. These rates are not our onboard Roy measurements.

The reference reconstructs sample times from an unwrapped sample counter and a fitted lower envelope of host receive times over a rolling window, with a separate camera alignment offset. That estimate includes minimum transport delay and restarts on reconnect. It does not use the sync timer as the sample time source. Our own live clock and identity requirements still apply.

The document also describes its custom USB text exchange format. That dongle interface is setup specific and is not the Frame API. Open questions include firmware version differences, additional IMU modes, auxiliary sensor meaning, LED drift rate, orientation convention, factory matrix application and a better three peer polling schedule.

For implementation use, inspect the version matched project source and current evidence. This digest preserves useful facts and provenance without importing a third party hardware claim into our test record.
