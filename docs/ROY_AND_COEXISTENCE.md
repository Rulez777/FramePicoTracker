# Onboard Roy, SRAM lifecycle and controller coexistence

Roy is the onboard Nordic radio path. Temporary executable SRAM is firmware-specific; preserving stock flash does not make an arbitrary debugger operation harmless. Record checkpoint/readback, MPU layout, code/data ranges, cache/trace controls, stock image hash and exact executable source before mutation. No external receiver satisfies the target.

Original210: code2043bytes under2045byte limit, CODE0x2001151c/entry0x2001151d, DATA0x20010c18/2048bytes, existing executable MPU region0x20011000..0x20012000 and static MPU count3. Native code hash`63d319d4b424bc5a291558e41d9a740962b20eeb33dba77ba288d0c1d0480575`. This is the admitted single-peer build. Do not enlarge limits or substitute a similarly named later source.

MPSL START/TIMER/high-priority callbacks and low-priority signals have different legal actions. Iteration20 fixed a forbidden low-priority REQUEST return causing assert106/472. Later106/501 occurred with and without cameras. Reviews suggested overstay/lifecycle and host reentry causes, but exact causality must be tied to the executed build and actual counter/IRQ evidence. Hiding the assert or weakening restoration does not repair lifecycle.

Saved iteration24 completed60s with7603TX/778RX and no fault, but adjacent deployment manifests described a different binary. The actual capture manifest/binary match matters more than folder labels. Original image/ELF hashes are in the asset register; none is bundled. Iteration25 was only an unvalidated1976code-byte build. Subsequent current work uses original210, not an assumed latest numerical build.

Host reports must distinguish scheduled slots, actual TXEN, actual TX-END, MCU RX totals, host ring records, dropped rows, retries and block/grant counters. ISR observation timestamps are not exact RF arrival/end. Host SWD read timestamps are not sample timestamps. A pattern of answered-poll gaps cannot reconstruct every unanswered TX or uniquely identify tracker RX duty cycle.

## Controller protection

Controller-off bench admission is a separate scoped mode, not proof of joint gaming. Track HMD/controller pose validity and freshness on the actual runtime path. Joint operation needs the150ms guard and measured controller health while actual trackers are active. The right-controller regression recovered after radio stop; sparse or low-priority failures are not permission to remove the guard. Headset-only pose rows cannot prove both controllers coexist.

Historical114 stock-controller-only observation completed30s with7616raw/7583accepted zero-span register rows,3907busy rows allRADIO.MODE4, and both controller invalid/freeze maxima0 in that observation. FREQUENCY observations span37register offsets:4,6,8,10,12,14,16,18,20,22,24,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78. In the audited2.4GHz mode these are2400+offsetMHz. See verification/CONTROLLER_RADIO_OBSERVATIONS.json for exact counts and source hash. The source aggregate is outputs/phase114/analysis.json, one level above the controllers-channels30 raw capture directory.

These are actual observed register channels, not a complete/fixed per-hand channel map, BLE connection interval, exclusion guarantee or joint tracker admission. Locally covered700us quiet runs did not establish a whole-period scheduler window because some phase bins had zero coverage. Selecting another tracker channel does not eliminate contention on the same physical radio.

## Deferred multi-peer memory branch

Preferred offline candidate328 used separate state islands while keeping the existing DMA258bytes and MPUcount3. Main2043bytes and50 additional code/literal bytes occupied reserved DATA regions. Historical56byte mailbox writers could overwrite the new code; a32byte writer and range/readback helpers were investigated. Review343 intentionally refused hardware before debugger access because lifecycle/admission closure was incomplete. Do not load candidate328 because it compiles or because single-peer210 works.

Finish one tracker first, then prove fresh identities/calibration, three-peer source-layout/readback and scheduling, finite applied-mode proof per peer, roles and simultaneous controller quality. A hypothetical42ms schedule and calculatedN values are not three-peer hardware admission.
