# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K01841
Source `D0315`, original line7, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

Phase304 fetchContour281690 and findContours281ad0 implement outer contour tracing. Native positive-parent and hole-transition handling excludes nested contours. Phase305 CCOMP parent filtering was corrected to EXTERNAL306. Actual120 camera observations in the two retained windows are identical; reuse305 geometry without an unchanged rerun. Full native contour/grouping/order equivalence remains unproven.

## K01842
Source `D0315`, original line9, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

## Recorded evidence and rejection

## K01843
Source `D0315`, original line11, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

The outer-only detector plus295 projected-region/full-reacquisition pipeline yields59/60 fits,56 computed under250ms on desktop. Stable30/30 and degraded29/30; compute median10.997ms,p95375.612ms,max983.372ms. These counts are NOT verified stereo poses: all59 are mono fallback. Against baseline180,33 common fits differ up to140.227mm and161.164deg. Against295,54 common positions differ up to147.535mm. Neither reference establishes physical ground truth.

## K01844
Source `D0315`, original line13, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

Phase308 independently uses the retained transforms only as correspondence hints with BOTH actual simultaneous camera observations and unchanged stereo/covariance gates. Zero of59 passes. Thirty exceed the existing bounded bootstrap hypothesis capacity;29 have fewer than four stereo-consistent points. Twenty-nine have zero projected-model matches in the right camera while5/6 match in the left. Stable-window examples have approximately6px disagreement in the other camera despite subpixel mono residual. Cause remains unproven: inspect calibration/coordinates and assignments before any repair. Do not increase residual tolerances or bootstrap capacity merely to admit these fits.

## K01845
Source `D0315`, original line15, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

The existing monocular association is a BOUNDED search with early exit and30 candidate-set cap, not an exhaustive global ambiguity proof. Projected prior matching can return without alternative-root enumeration. Earlier descriptions of complete search must not be generalized to this solver. Native Swift uses visibility/orientation priors; target IMU axes and physical timing remain unverified.

## K01846
Source `D0315`, original line17, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

## Explicit modulation preparation

## K01847
Source `D0315`, original line19, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

Phase300 source proves native25 writes IRmode20001080, with IRmode1 gating off and2 conditionally allowing pulses. This is separate from Frame operating mode1. Phase303 ledger and307 photometric identity matcher are isolated preparation only: two transport-tagged OFF/restore episodes, nominal250ms restore and500ms maximum, continuous actual frames, six lit brackets and two dark measurements. No I/O, no application identity proof, no integration, no hardware use. Restore must remain independent of optical exceptions and fresh target/mode/controller guards must be wired before launch. Pixel projection exceptions and timing uncertainty require review. Existing identity and IMU-start gates remain intact.

## K01848
Source `D0315`, original line21, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

One paid Gemini read-only audit timed out90s without a verdict; no retry, no quota failure, no sourced conclusion from that call.

## K01849
Source `D0315`, original line23, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

## Continuation

## K01850
Source `D0315`, original line25, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

Inspect actual signed cross-camera reprojection residuals and original active/factory calibration provenance. Determine whether disagreement originates in wrong point assignments or coordinate/calibration mapping. Keep305/306 undeployed. Only then finish explicit native25 application-level optical identity on the current firmware and integrate verified IMU axes/time/fusion. Multi-peer protected RAM, controller coexistence, enrollment/body roles and live end-to-end fullbody are still incomplete. User is away with devices powered; camera presence and fresh mode must not be assumed.

## K01851
Source `D0315`, original line27, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase304/native-contours.asm; outputs/phase305/native-derived-adaptive-shape/geometry.json; outputs/phase306/observation-equivalence.json; outputs/phase308/cross-camera.json and summary.json; outputs/phase303/led_mode_episode.py; outputs/phase307/led_mode_identity.py.

## K01852
Source `D0315`, original line29, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

## Phases309–312 addendum

## K01853
Source `D0315`, original line31, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

309 checks mono-derived same-LED assignments against both actual factory-bound rays, without fitting calibration. Stable30 frames: closest-ray separation median1.807mm,p954.021mm,max6.879mm. Signed right-camera observed-minus-projected displacement median[2.575,5.196]px versus left[-0.054,0.0002]px. This does not isolate a calibration fault; a weak monocular depth/orientation seed can produce this disagreement.

## K01854
Source `D0315`, original line33, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

310 derives ONE rigid seed per frame from triangulated actual common-LED points, then invokes the unchanged stereo refine_matches gates. Twenty of30 eligible stable pairs pass; degraded29 have no four common model assignments and are excluded. Residual median1.248px,p951.314px,max1.339px; estimated position sigma median1.688mm,max1.811mm; estimated rotation sigma median2.365deg,max2.540deg. Recorded position spans[0.134,0.320,0.589]mm,maximum consecutive retained position step0.592mm and rotation step1.132deg. These are consistency and estimated covariance, not independent real-world accuracy. No complete alternative-correspondence ambiguity check, deployment, identity or live source-age proof. Candidate is isolated and must not be published merely because a least-squares fit passes.

## K01855
Source `D0315`, original line35, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

311 independently solves each camera with distinct cache keys/no prior at the three worst baseline-disagreement timestamps23867.593023194,23866.126356527,23866.193023194. Right camera: no constellation,no constellation,ambiguous. Left: ambiguous,6-inlier fit,5-inlier fit. Fresh ambiguity rejection at the first frame contradicts trusting the prior-assisted mono fallback there. This identifies a correspondence/temporal-admission weakness but does not prove the remaining mono fits physically false or identify the tracker by pixels.

## K01856
Source `D0315`, original line37, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

Next: preserve stereo triangulated seed as isolated candidate, finish an explicit bounded ambiguity/admission policy using native visibility/orientation source and actual recorded evidence. Do not weaken identity, 48/61, covariance, fresh source age or IMU guards. Candidate303/307 native25 episodes still lack live wiring/restoration exception review; no RF launch is justified yet. Fullbody still needs verified IMU axes/time, multi-peer memory/runtime, controller coexistence and body roles.

## K01857
Source `D0315`, original line39, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

Additional evidence: outputs/phase309/epipolar.json,summary.json; outputs/phase310/geometry.json,summary.json; outputs/phase311/independent-mono.json. Scripts retained. No synthetic tests or new hardware runs.

## K01858
Source `D0315`, original line41, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

## Phases313–314 source/preparation closure

## K01859
Source `D0315`, original line43, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

313 reads the actual ELF PT_LOAD literal, binary SHA749b655313ac161ec0f8a7b081ec33bef8f3d5d04c271e9f11b066c87d30bf60. generateVisiblePoints4322c8 computes a normalized position-minus-centroid vector from object records stride400/offset+a0, compares its negative dot with a quaternion-rotated input vector at432624 against literalb52980 = -0.56, and skips <=threshold at432628. Input coordinate conventions, target LED record mapping and verified IMU orientation remain unresolved; no live visibility gate was installed from this finding.

## K01860
Source `D0315`, original line45, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

314 changes isolated303 restoration_due to count250ms from OFF publication, so delayed ACK cannot extend OFF time. Isolated307 now requires exact uint8 image shape, two exposures and rigid finite lit transforms; ValueError/linear-algebra projection failures reject optical evidence rather than escaping the matcher. Independent radio restoration wiring is STILL REQUIRED. Source parsed only; no synthetic tests, RF, OTA, deployment or application-level modulation proof. The earlier files are prototypes, not sealed runtime assets; their hashes changed and must be re-reviewed before packaging.

## K01861
Source `D0315`, original line47, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

One Cursor read-only review of the protected three-peer RAM excess was dispatched under user's explicit paid-bridge instruction. Its verdict is pending; no build/edits/hardware authorized to the helper and no result can be claimed until independently reviewed.

## K01862
Source `D0315`, original line49, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

## Phase315: uncertainty does not resolve correspondence ambiguity

## K01863
Source `D0315`, original line51, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

The deployed273 stereo pipeline assigns mono fallback a fixed estimated_position_std_m=.003 without deriving its pixel covariance. Actual retained305 poses315, conditional on greedy pose-projected assignments, have local pixel-Jacobian position sigma median5.241mm,p956.150mm,max6.188mm using the existing stereo0.5px noise floor. Rotation sigma median1.965deg,p952.752deg,max2.999deg. All59 local Jacobians have full rank and NONE breaches existing stereo3cm/5deg covariance thresholds, despite the independently recorded large alternative-fit disagreements. Therefore a covariance-only repair or smoother would not fix alternative correspondence/root identity. Local covariance assumes the chosen assignment; it is not a global pose-ambiguity test or true physical accuracy. Existing runtime remains unchanged; no new software tests/hardware.

## K01864
Source `D0315`, original line53, SHA256 `b3b8e113e0b54d9c2c2dcede253c30e15578571ce3ba210329e7938f7ed5472e`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase315/mono-covariance.json and summary.json. Continue native orientation/visibility mapping and explicit source-backed optical identity, with strict ambiguous-frame rejection and independent restoration ownership.

## K01865
Source `D0316`, original line1, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

# Phases316–320: three-peer code fits owned ranges, activation prohibited

## K01866
Source `D0316`, original line3, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

## Result and boundary

## K01867
Source `D0316`, original line5, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

Only source generation, remote disk compilation with the original phase210 compiler/flags, ELF retrieval and static review occurred. No MCU SRAM write, RF, OTA, new firmware, camera recording or hardware tracking test. Original admitted210 single-peer module remains the runtime. Existing hash/section/capability admission MUST reject this proposal.

## K01868
Source `D0316`, original line7, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

316 replaces a three-peer stage2 scan with the last selected peer plus invalid-index stop. Under all-zero initialization and host stage reset-only writes, only prepare_packet creates stage2 on the selected peer; the pending stage preempts further offers until promotion/reset. Multiple externally written stage2 records would violate this invariant; exclude them in admission and review native/host writers. Build2097bytes saves24 versus2642121, still52too large.317 outlines mailbox without changing C statements;2143bytes, rejected.

## K01869
Source `D0316`, original line9, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

318 relocates only request_initial40bytes and stop_radio56bytes to20010d30..20010d90 inside already reserved private DATA's DMA tail. Main section2029bytes at2001151c..20011d09 fits2045. Total code2125bytes spans two sections but extra96bytes overlaps an UNUSED portion of the existing DATA2048 reservation; unique allocated footprint4077bytes, or4080 after word padding, within existing2048+2045 owned ranges. No stock-adjacent gap or console buffer borrowed.320 verifies actual ELF entry2001151d, exactly these two allocated PROGBITS sections, addresses, hashes and island function identities.

## K01870
Source `D0316`, original line11, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

## DMA and MPU source basis

## K01871
Source `D0316`, original line13, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

Existing source DMA258bytes starts DATA+120=20010c90. ProfilePCNF0=40008:8bit LENGTH,4bit S1, no S0; PCNF1=01040096: MAXLEN150/staticadd-on0. Nordic RADIO specification states MAXLEN bounds payload/add-on in RAM and excludes header fields, with each nonzero header field occupying a byte. Thus this configured profile writes at most152bytes; island starts at DMA+160, leaving8bytes headroom. MCU software capture/copy bounds<=80bytes, mailboxTX<=58bytes,startup<=23bytes. This is a static configured-profile bound, NOT proof of all native RF transitions or anomalous ownership. Retained207FICR begins33280500, decoded part52833, not fresh hardware identity proof. Primary specification: https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/radio.html

## K01872
Source `D0316`, original line15, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

Current MPU executable region20011000..20012000 does NOT cover the island. Using unchanged MPU would fail.318 explicitly adds separate region3 at20010d00..20010e00 (256bytes,RBAR20010d13,RASR010b000f), static count4, preserving existing all-RAM XN and region2. This broadens execution permission only in an owned private-DATA subset but consumes another native MPU slot; dynamic-region budget/context switching must be proven. Do not disable MPU or silently broaden the old region. No runtime admission has been changed to allow this layout.

## K01873
Source `D0316`, original line17, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

## Exact artifacts

## K01874
Source `D0316`, original line19, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

318 ELF SHAcef9e90e28898a0c7fc661d39da6ba0e22afbe4ec9feef41d5c2c90168805d39. Main code SHA bf17a76f323e9ec84b75456240384b87c9ebfe663f08ae05016c733ee20c5728; island SHA eff951a3aa96ff423951826afd7a8d91d3e100754edb11fea634fd6978980d4d. Both section concatenation SHA d79feffe818410e43908a2c9fba28fd2171b2ef5bd03619bc79290f112baad6c is measurement provenance only, not a new admitted capability. Original guards/barriers/payload/ring/deadline/bounded wait/native pause preserved in source; selector and MPU changes require independent review.

## K01875
Source `D0316`, original line21, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

Evidence outputs/phase316,317,318/remote-size-report.json; phase318/proposal.json,candidate.c,candidate.elf; phase320/layout-proof.json,island.asm.319 statically locates native MPU-count literals at1f8cc,1fb38,20aa4,20ad0,20b34 in exact stock hash0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50. Linear windows include literal data; references are retrieval handles, not region-budget safety proof.

## K01876
Source `D0316`, original line23, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

## Required continuation before any activation

## K01877
Source `D0316`, original line25, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

1. Verify all module/host/native DMA writes and RADIO profile/PACKETPTR transitions; bound must hold whenever native radio can still point into this buffer. The old258byte buffer tolerated larger packets; tail code makes unsupported profile transitions dangerous.
2. Prove native MPU allocation/reconfiguration can preserve four static regions with adequate dynamic slots. Source209e0/20a94/20aa8/20ad8 are leads; confirm argument/return/bounds, not just byte literals. Preserve source-specific stock identity and region-count guards.
3. Implement isolated exact multisection ELF/image/hash admission, DATA initialization followed by island insertion/readback before invocation, main readback, initial sole-stage2 invariant and fixed hardware profile constraints. Existing production capability continues fail-closed.
4. Recovery checkpoint already savesDATA2048 including island area, but initialization would zero island unless inserted afterward. Restore originalDATA only after native MPSL closure/code inactivity; add explicit island-code closure checks, fourth region/static count restoration and readback. Existing loader expects reserved count3 and one main section; do not reuse it unchanged.
5. Only after review prepare one bounded purposeful activation with fresh target/mode/HMD/controllers admission and verified cleanup. No blind sweeps, guard deletion, firmware or repeated unchanged tests. Fitting bytes is NOT proof of multi-peer scheduling, controller coexistence, optical identity, IMU fusion or body roles.

## K01878
Source `D0316`, original line31, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

The paid Cursor read-only RAM review timed out210seconds without verdict, not a quota failure. No retry or unsourced helper finding was used. Optical findings remain separate: adaptive contours did not improve reliable stereo; stable triangulated seed20/30 is offline only. Real fullbody remains incomplete.

## K01879
Source `D0316`, original line33, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

## Phases321–324 source and isolated admission

## K01880
Source `D0316`, original line35, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

321–323 close a specific native direct dynamic-MPU path:209bc reads input object+9c, subtracts64;209c8 builds a64byte region;209d0 count1;209d2 tailbranches20aa8.20aa8 loads dynamic starting index from20010a26, uses209e0 whose limit is index7, then clears subsequent indices through7. With proposed static count4, this direct path uses region4 and clears5..7, preserving new static region3. Object role/stack-guard interpretation is inferred; computed/indirect paths and live context switching still unproven. No aligned literal pointers to the three helper entries found; absence is not proof of absence of indirect calls. Evidence outputs/phase323/native-mpu-budget.json/.asm.

## K01881
Source `D0316`, original line37, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

324 adds a NO-debugger/no-transport isolated artifact reader and initial-state builder. Exact318 ELF/ARM/entry/two sections/lengths/hashes and word padding verified on the actual artifact. Initial-state construction requires2048bytes/exact magic/session255/zero active counters/emptyDMA, then inserts96code bytes at DATA+280..376. Production loader UNCHANGED: actual318 elf_image call rejects with Module allocates outside reserved code buffer; no hardware. Future recovery address predicate covers both main/padding and island; both actual island function addresses are outside legacy CODE range and inside new predicate. This is necessary for shutdown checks, but does not prove session/stack/code inactivity. No live loader wired or enabled; deployment_allowed remains FALSE. Evidence phase324/old-loader-rejection.json,artifact-proof.json,scatter_artifact.py.

## K01882
Source `D0316`, original line39, SHA256 `2c4c425e3c239242c9b318912c2978bec6b7555647c0fb148cc0b0d4b7e78691`. Status: historical/unadjudicated narrative; English source prose.

The user explicitly requested continued work until real fullbody; persistent goal is ACTIVE with no token budget. Fullbody is incomplete. A text-only question about actual available tracker count/body placements is pending while independent source/recovery work continues. No HMD wearing or RF readiness question was issued. Goal creation is not hardware/OTA permission or completion evidence.

## K01883
Source `D0317`, original line1, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

# Phases328–332: three-peer state-island candidate, no DMA or MPU expansion

## K01884
Source `D0317`, original line3, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

## Current outcome and boundary

## K01885
Source `D0317`, original line5, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

This candidate supersedes318 as the preferred OFFLINE placement. It preserves all258 DMA bytes and original static MPU count3; no fourth region or native DMA MAXLEN/lifetime argument is needed for code placement. No MCU SRAM write, RF, OTA, new camera recording or hardware test occurred. Remote compilation writes disk artifacts only. Original admitted210 single-peer module remains unchanged. Goal active/fullbody incomplete.

## K01886
Source `D0317`, original line7, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

Actual328 ELF has main2043bytes at2001151c, exactly the existing2045 limit after2044-byte word padding. Additional50bytes overlap unused portions of existing DATA2048: request_initial instructions24 at DATA+1620=2001126c; request literals12 at+1652=2001128c; copy_bytes14 at+1704=200112c0, inside mailbox0 payload32..47. All lie within the original executable region20011000..20012000. Unique word-padded reservation4092bytes (2048+2044), within the old owned ranges. No stock gaps or console buffers borrowed.

## K01887
Source `D0317`, original line9, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

329 verifies exact four PROGBITS sections/entry2001151d, function identities, all three narrow literal-load target addresses and pool words. First328 link failed because cross-section LDR emitted wide instructions, code30bytes versus24 capacity. Failure log retained. Fixed-layout T1 literal loads encoded explicitly and verified against actual linked addresses; no linker-relaxation assumption. Build then passed. No repeated unchanged builds or synthetic tests.

## K01888
Source `D0317`, original line11, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

## Exact hashes and addresses

## K01889
Source `D0317`, original line13, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

ELF SHA4b638693cd58580a04d6de380dd76c699286f4c94cfd9f5b2ef2b0f90e0a2497.
- main2043: SHA0da2c56b814221a9c0ae2edb556b49132aadb36e2117270fa8de30b0118bbeb8.
- state_code24: fd6eebe722b0357bb991734961ebef45995502821a8c19b4f308e6e19ddd1a6b.
- literals12: f7ea13b703fcc270e0e7e04911644e48475587b3d929b6aeff6f9563b161de4a.
- mailbox0_code14:76899e190a7f67bf8b6a62bb626803e8845a92f5f1a73c56389b23d22cbd9b93.

## K01890
Source `D0317`, original line19, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

These are placement provenance, NOT admitted runtime hashes. Old single-section loader/hash capability must continue rejecting the artifact.

## K01891
Source `D0317`, original line21, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

## Commands and mailbox preservation

## K01892
Source `D0317`, original line23, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

MCU maximum payload is narrowed56->32, keeping exact8-byte native10 timed-reference shape and existing generation/meta/capacity/barrier guards. Frozen273 query_loader diagnostics use command1 body7e/schema/nonce16 =18body bytes; unchanged pack(record) adds4, total22. Native mode query22 and remote mode setter28 fit32; a29byte payload also fits. The setter is body7d02+nonce16+chip4+profile1+N1 (24body bytes) plus4 framing. Fresh mode/nonce/target/CRC rules remain. Large OTA packets are intentionally unsupported; user forbids OTA.

## K01893
Source `D0317`, original line25, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

330 uses a real retained273 hardware-CRC-valid target mode reply sequence14 to obtain its actual nonce and chip. The first reply was mode0 before the old session's verified transition; this is NOT current mode proof. Source-derived/reconstructed outgoing query22/set28 demonstrate exact encoder length and write intervals. They were not transmitted, and no fabricated nonce or synthetic fixture was used.

## K01894
Source `D0317`, original line27, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

Old MailboxWriter writes all56payload bytes and WOULD overwrite copy_bytes at payload32..45. Do not bind old writer. Isolated330 NO-I/O write_plan uses odd generation, meta+32payload bytes, reference48..55, then even generation; payload32..47 stays immutable. cycle_window rejects before any writes, as current timed module already does. Metadata/reference clearing semantics retained. Required mode/diagnostic/native10 packets fit, and source write intervals do not overlap code. This is not yet a live writer/loader binding or transport proof.

## K01895
Source `D0317`, original line29, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

## Native request ABI and data ownership

## K01896
Source `D0317`, original line31, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

331 reviews exact stock SHA0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50. Request preserves ordered volatile accesses: store8 DATA+88 type0; load32 DATA+1876; store request.a DATA+96; store request.b DATA+100=100000; load8 DATA+84 session; r1 points DATA+88. Same tail BX18949 preserves LR/SP and callee-saved registers. Native18948 saves r1 before session lookup10628, which overwrites r1/r2 and uses r0 only. Request validator10508 overwrites r3 before use. Static API-path review supports changed caller-saved register allocation; no hardware native API behavior exercised.

## K01897
Source `D0317`, original line33, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

Only exact328 compiled source can use these fields: other historical MCU sources wrote diagnostics at these offsets. Current328 direct D1644/D1648 writes are between state_code end1644 and literals start1652, not inside either. Peer records1504..1600, active counters1880..1920, ring/DMA bounds and generation writes retain their old ABI. Host initializes priority at1616 and mailbox head/ref fields; complete writers/recovery review still needed. Legacy loader reads1652/1656/1660/1628/1632/1640 as obsolete diagnostics; they would be CODE/LITERAL values now and must be suppressed or explicitly tagged unavailable. Never promote them to TX/IMU/timeout proof. Actual TX_END counter1908 and real CRC/native/IMU records remain separate evidence.

## K01898
Source `D0317`, original line35, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

## Next required implementation

## K01899
Source `D0317`, original line37, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

1. Exact four-section reader/state builder, initialization after zero state, hashes/readback of every island and main; pristine-state and original RTT/MPSL/MPU identity admission retained. Bind32byte island-preserving writer before any mailbox publication and reject OTA/legacy diagnostic misuse.
2. Review all selected host/MCU writers; enforce sole-stage2 selection invariant from316 and initial index0. No source from historical legacy modules may substitute for compiled328.
3. Recovery checkpoint already saves fullDATA2048, but code closure checks must cover DATA code islands too. Close native MPSL/entry hook and verify inactive code before restoring DATA. Keep original MPU/static-count3 lifecycle, no new region. Restore full DATA/main/RTT/trace with exact readback, retaining RAM if closure fails.
4. One sealed purposeful activation only after those reviews and fresh target/mode/controller/HMD admission. Initial validation must verify required commands/three-peer state and cleanup, not claim live body tracking from bytes fitting. No phase sweep, guard deletion, unchanged RF replay or firmware update.

## K01900
Source `D0317`, original line42, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

Optical/IMU work remains: adaptively increased mono fit counts were rejected as stereo improvement; stable triangulated seed20/30 is offline only; native explicit25 OFF/restore identity still preparation-only; verified axes/time/fusion/coexistence/body roles remain incomplete. User available tracker count/placements question is pending while independent work continues.

## K01901
Source `D0317`, original line44, SHA256 `70da9940202e6e58f8cc2aa1ffb51061ae3a638c4f7649df651542dcecb56230`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase328/proposal.json,candidate.c,candidate.elf,remote-size-report.json,first-wide-literal-build.log; phase329/layout-proof.json,islands.asm; phase330/island_mailbox_plan.py,recorded-mode-plan.json; phase331/abi-proof.json,native-request.asm.325–327 native radio pointer/profile investigation is retrieval-only and is no longer a placement blocker for328; retain it as source evidence, not a completed DMA-lifetime proof.

## K01902
Source `D0318`, original line1, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

# Phases 333–338: state-island host preparation and native body interface

## K01903
Source `D0318`, original line3, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

## Human-confirmed equipment
The user confirmed three physical PICO trackers on 2026-10-11: left leg, right leg and waist. Do not ask the count again. Only [TRACKER_SERIAL] / chip [TRACKER_ID] is enrolled and identified in retained registry273. The other two identities, factory calibration and assignment are NOT established. Intended placement does not establish a serial-to-role mapping.

## K01904
Source `D0318`, original line6, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

## Static implementation, not deployed
333 implements an exact four-section reader for the hash-bound328 ELF, pristine-state insertion, individual code readback plans, conservative code-range membership and original checkpoint restoration bytes. It does not access hardware. Recovery requires independent native session closure, hook clearing and inactive-code evidence; caller booleans are not themselves live proof. Actual retained258 checkpoint exists but cannot prove current closure.

## K01905
Source `D0318`, original line9, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

336 implements an isolated32-byte mailbox writer and binding helper. It reads and checks all four loaded code ranges and original static MPU reservation3 before publication. Odd generation / meta plus32 payload / separate reference8 / even generation ordering preserves mailbox0 payload32..47. Exact old56-byte writer must never be used with328. No production loader or admission changed; no caller is wired. Runtime initialization, all host/MCU writers, sole-stage2 invariant, obsolete diagnostic suppression and native shutdown/restoration still require closure before any activation. Compilation or this helper is not multi-tracker hardware proof.

## K01906
Source `D0318`, original line11, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

## Native body materials
334 inventories actual Android AArch64 libAlgSwiftBodyPose.so and1.0 exports/dependencies. Android, SNPE, bytenn, EGL/GLES and C++ ABI requirements prevent assuming these libraries run directly on Frame. No library execution occurred.

## K01907
Source `D0318`, original line14, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

335 locates SwiftBodyPosePredictor SetIMUInputMode334808, PredictBodyPose3320cc and GetBonesResult33362c in the retained PICO tracking library.337 resolves PLT21f190 to pico_sensor::AIContainer::getInstance. SetIMUInputMode calls it at3348b4 and dispatches vtable+0x40 at3348c4; this is not a logging-only stub. Prediction includes a map keyed by AlgTrackerRole containing six_dof_ai_input shared pointers. Numeric roles, field layouts, timestamp/axes conventions, concrete vtable implementations and body-output conversion remain unproven. Do not fabricate role mappings from symbol names or treat body inference as measured tracker poses.

## K01908
Source `D0318`, original line16, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

## Required continuation
1. Finish guarded multi-section loader preparation, initialization/writer/lifetime review and exact restoration. Candidate328 remains offline; original210/FWdiagnostic10 unchanged.
2. Trace native role-valued input construction, AIContainer concrete methods and bones output conventions using337 disassembly and factory library. Separate portable protocol/math from Android runtime dependencies.
3. Finish optical correspondence/causal native25 OFF-restore identity integration, then measured IMU axes/time/fusion. Stable310 stereo20/30 is recorded consistency only, not live identity or physical accuracy.
4. Identify/enroll two additional real devices, bind waist/left/right roles to confirmed identities, then verify three-peer tracking, occlusion and controller coexistence. One purposeful live run only when the actual candidate is sealed and fresh mode1/HMD/target evidence is available.

## K01909
Source `D0318`, original line22, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

No RF, MCU SRAM writes, OTA, new capture or fabricated tests occurred in333–338. Last hardware273 remains closed. True full body tracking is UNACHIEVED; goal remains active.

## K01910
Source `D0318`, original line24, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

## Static continuation339–340
339 follows actual AIContainer factory214978 to constructor20b68c and concrete vptr4e6c10 stored at20b7cc. ELF relocations resolve offsets40/48/60/68 to2126cc/212b98/2148a8/214950. Result method2148a8 checks container flag2e8 and object20, calls1eac48/1ecc88 and conditionally copies2040hex bytes into caller output; numeric skeleton meaning remains unproven. Offset68 reads object+2068 state and returns auxiliary status1 or2. Mode method queues the supplied integer converted to float; it is not sufficient to infer role enumeration. Exact evidence outputs/phase339/container-symbols.json,vtable-methods.json,factory.asm,constructor-prefix.asm,method-prefixes.asm.

## K01911
Source `D0318`, original line27, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

340 executes only the extracted pure old elf_image reader against actual328 ELF: it rejects with "Module allocates outside reserved code buffer" before any debugger construction. Source writer inventory records current host enrollment/reconnect writes and candidate selector references. This is a necessary compatibility check on an actual artifact, not a synthetic hardware test. Production admission remains unchanged. Full frozen selected-loader write/lifecycle audit remains pending; pc/radio sources alone are not execution provenance. Evidence outputs/phase340/source-audit.json.

## K01912
Source `D0318`, original line30, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

## Loader preparation341–342
Reviewed the actual273 frozen loader, whose invocation.json binds radio/ram/roy_ram_loader.py SHA5eef1a3599d9de9a6afd4f3bc0074787ecbe79e0a6becb9622f8eeb022ddd60e. It initializes DATA with magic314d5250/session255, copies peer state1504 onward, then writes fullDATA/main. The current workspace loader alone is not its provenance. Native callback detection and main-PC checks cover only CODE, not DATA code islands; these checks need expansion before activation. Auto-return writes22 saved register words atDATA1504, overwriting peers; it must not be combined with this native-shell three-peer layout. Actual273 used guarded native shell.

## K01913
Source `D0318`, original line33, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

333 readback and pristine reservation checks now cover full16-byte mailbox island, including two zero padding bytes. Exact actual328 ELF yields padded island SHA6e68d9e2a54934225883e3c3bfc30afd469a72bda27d777aa601cc9ce854edd6. Earlier14-byte code hash is unchanged; only full reservation checking was corrected.

## K01914
Source `D0318`, original line35, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

341 adds isolated placement_plan: validate exact artifact and pristine initialized state, insert all DATA islands, return DATA2048/main2044 ordered writes plus exact readback ranges. verify_placed checks actual bytes before invocation. require_inactive_halted_context rejects missing PC/LR/XPSR/CONTROL, handler/unprivileged context or current PC/LR inside any code/padding range. This checks only a coherently halted current context; it cannot establish all-thread/native callback closure. Caller must retain halt through restoration and separately prove session/hook closure. No caller wired and no activation admission changed. Source parsed, exact ELF readback plan evaluated; no synthetic state/debugger/hardware test.

## K01915
Source `D0318`, original line37, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

Next implement the selected frozen loader integration with exact admission, native lifecycle checks and obsolete diagnostics suppression; bind336 only after MCU reservation3 acknowledgment and before any mailbox publication. No fresh mode/HMD/peer evidence exists for a new run. No RF/OTA/SRAM writes since273; three physical trackers confirmed but only one identified. Fullbody still incomplete.

## K01916
Source `D0318`, original line40, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

## Review-only frozen-loader integration343–344
343 generates an exact273 loader source overlay with DATA island insertion, full code/padding readback before invocation, new mailbox binding after original MPU3 acknowledgment, callback-range expansion, coherent halted PC/LR check before full restoration, and suppression of obsolete diagnostic reads. TX fields are explicitly unavailable, not RF completion evidence. A hard refusal remains before debugger construction; this review source is not deployed and cannot activate. Package imports, exact admission, native entry/all-thread closure and MPU restoration ordering still need review. First generation stopped on a duplicated TX-read patch anchor (two actual reads); corrected by requiring exactly two source matches. No hardware or unchanged trial occurred.

## K01917
Source `D0318`, original line43, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

336 binding now rejects both runtime.ota_job and runtime.ota; the frozen runtime uses the latter. Existing freeze/mode/controller/age/identity gates were not changed.

## K01918
Source `D0318`, original line45, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

Actual273 query_loader.py ALSO needs a paired review: safe_record hardcodes profile0/chip[TRACKER_ID]; advance skips diagnostic processing for other chips; bootstrap_mode enables only[TRACKER_ID] with common8800 and Stock1/Frame2 before native N1 request; optical status hardcodes[TRACKER_ID]; prerequisite/producer output files use single-target names. It cannot simply be reused for three devices. New per-peer proof must bind profile slot, hardware CRC, source chip, actual enrolled product_sn, fresh mode/N/nonce, optical identity and producer age independently. Do not replace these checks by accepting all chips or equating transport ACK with application proof. Registry273 confirms exactly one slot0/product_sn; other two remain unknown despite physical count3.

## K01919
Source `D0318`, original line47, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

6000us slot scheduling and selected-stage2 preemption must be reconciled with per-peer native mode/N receive windows before a three-peer invocation. Keep original Native101 hash/paused-peers guard fail-closed until exact328 sampler admission is separately reviewed. No generic cadence change or RF sweep is authorized by this source overlay. Evidence outputs/phase343/loader.patch,roy_ram_loader.review.py,proposal.json and generation source. Goal remains active; no new RF/OTA/capture/SRAM writes.

## K01920
Source `D0318`, original line50, SHA256 `6bdb47d61b225806372290d1244194e05e264f50a86b578062ace62bd1fcba76`. Status: historical/unadjudicated narrative; English source prose.

## Recorded source and cadence closure345–348
345 compares only source-proposed6000us scheduling with direct3-slot versus existing source7-slot cycles; this is offline arithmetic, not an RF sweep or native timing proof. Existing frozen validate_remote_mode_grid rejects per-peer18000us: FrameN5 fits but no StockN fits. Per-peer42000us passes that host formula: StockN8 opens41754.5/closes42547.5us, margins245.5/547.5us; FrameN13 opens41207/closes42000us, margins793/0us. Zero Frame closing margin and stage2/N transition prevent treating this as a sealed live cadence. Native window branches/actual installed hooks must be reviewed before scheduling changes. Historical original host formula with3538 normal delay is distinct from3934.5 conservative alternate used by dual-mode validation; do not mix them.
