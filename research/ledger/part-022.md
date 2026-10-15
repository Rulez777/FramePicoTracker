# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K01681
Source `D0305`, original line11, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Exclusive150s surface-supported capture ended normally. All four cleanup flagsTRUE, power300 restored5, RF closed. Actual6668IMU/11799countergaps/1591native11feedback,2587opticalpublications. During87.62s first-to-last measured hold, fresh2006179/8432=73.28%; visual200+20199.68%; maximumfreshgap18.277s. Do not present visual retention as smooth tracking.

## K01682
Source `D0305`, original line13, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Static analyzer finds6one-second quiet windows but onlyONE distinct gravity direction. New tilted configurations lost usable >=5-point monocular optical coverage. Olddynamicfit rejects:409pairs, RMS0.2211rad/s, scales2.055/.776/.108; axes/time/fusionunverified. No new OTA or parameter scan.

## K01683
Source `D0305`, original line15, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

## Actual recorded stereo evidence246/247

## K01684
Source `D0305`, original line17, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Pre-identity candidates contain6same-exposure pairs with4blobs in EACH uppercamera, both rejected by the oldmonocular minimum5. These are unassigned geometry observations; do not infer temporalidentity from their presence.

## K01685
Source `D0305`, original line19, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Factory-calibrated stereo prototype recovers6/6, reprojectionresidual0.197..0.253px, physicalLEDindices0/5/6/7,8rays; estimatedpositionstd~1.05mm androtationstd~2.44deg (conservative0.5pxobservation floor), ambiguity checked. Source metadata exposure differences172..669ns. Uses exact active KB calibration/extrinsics and serial-bound factory8LEDmodel, no pinhole defaults, inertialpose or inventedposition.

## K01686
Source `D0305`, original line21, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Completed native OCC245 exported539MiB. Decode bounded RAW-time16975..17040 segment from real IDR,132actual1Hz camera samples, then1974actual15Hz samples. First extractor exhausted100000record budget before reachingsegment: corrected to bounded chunk iteration until completedEOF; zero-output attempt was not used as data evidence. Decoded framecounts match access-unit metadata exactly. No new camera or RF operation.

## K01687
Source `D0305`, original line23, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

## Dense actual replay248

## K01688
Source `D0305`, original line25, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

987same-exposure pairs (<=2us):762stereo geometry accepted;477accepted while at leastonecamera had<5blobs.753use an ACTUAL freshpast<=250ms world-pose association hint, then independentcurrent joint ray refinement. Prior can never be emitted as a new measured pose. Remaining bootstrap exhaustive<=128hypotheses; exceeds capacity =>refuse, never accept an incompletely checked ambiguous search.

## K01689
Source `D0305`, original line27, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Runtime p50/p95/max6.03/13.31/132.28ms.178pairs lacked required4LEDstereo/bounded8blobcapacity,27insufficientconsistenttriangulatedpoints,19no4LEDfit,1ambiguous. Same-exposure comparison with actual oldmono publications:543/987hadoriginalmono,284additionalstereoexposures,65oldmonoexposures lackedstereo. Combinedgeometry827/987=83.79% versus55.02% originalmono in this segment (+28.77percentagepoints). This is offline geometry coverage, NOT liveSteamVRavailability or fullbodyproof. Production249retains separatelylabelledgenuine>=5-inliermono fallback.

## K01690
Source `D0305`, original line29, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

HistoricalRAW->MONO mapping supported by2921actualrecordedsource-stamp/metadata pairs; maximumfiterror55.88us. HMD transforms use bracketing actuallyvalid recordedStandingposes with<=25msgap. New stereo gravity replay finds8quietwindows andTWOdistinctgravitydirections, heldoutgravityerror0.3044deg,scale0.98190..0.98382; explicitstatic analyzer requires3directions so stillREJECTED. No axis/time/fusion activation from this provisionalmatrix.

## K01691
Source `D0305`, original line31, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

## Explicit249 candidate prepared; hardware not yet started

## K01692
Source `D0305`, original line33, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Files outputs/phase249/optical-pass/release/optical/{measured_stereo_solver.py,measured_stereo_pipeline.py,station_optics.py}. Defaultmonocularmethodunchanged; config explicitly selects calibrated_stereo_v1. Source allhashsealed in attempt/invocation.json. Base245pipelinecontains prior228standing-origin fix/229world-seed fix/232velocityfilter/233visualheartbeat; do not substitute stalecanonical measured_pipeline.py.

## K01693
Source `D0305`, original line35, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Stereo pairs factoryserial-bound uppercameras, actualpacket/exposuremetadata, <=2usexposuredifference. Waiting forsecondcamera is NOT a darkframe or a publication. Joint4physicalLED/8ray solution mustpass residual<=2.5px, covarianceestimatedpositionstd<=3cm,rotationstd<=5deg, geometry/properrotation/ambiguity. Current>=5-inliermono can seedjointstereo or remain actualmonofallback. No new inliers fabricated: logs separate physicalLEDcount/raycount/stereo_verified. Phaseadmission accepts the independentlyverifiedstereo solution as an alternative to monocular5inliers; actualnative two-commanded-dark/six-lit temporal identity remains unchanged. All publicationage250ms, source/time/identity/jump gates and inertialpropagation25ms/covariance guards remain.

## K01694
Source `D0305`, original line37, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

New pipeline actual-image replay verifies: incompletepairpublishesnothing;4+4geometrycandidatepublishesnothing; temporalidentityfalsecannotpublish; verifiedidentity admitsONEcurrentstereopose; source250ms+1refusesnewpublication; IMUaxes/time remainfalse. Real first4+4fixture was geometricallyambiguous and correctlyrefused; positivefixture chosen from independentlyaccepted unseededrealgeometry, not altered pixels or forcedacceptance. Evidence outputs/phase249/real-frame-contract.json. Offlineclock/publisherfixtures neverconstitute livehardwareproof.

## K01695
Source `D0305`, original line39, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

User asked readiness for one150s stereo pilot with surface-supported baseline andtwo arbitrarydistincttilts. Directionlabels notneeded; measure actualangles/coverage. Await current readiness; freshmode1 andactualHMD/controlleroffpreflight mandatory. Sourcepipeline reset/reconnect mustneverinheritphysicaltime. No RFactive at this preparation stage, noOTA/no phase scan.

## K01696
Source `D0305`, original line41, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Fullbody remains unfinished: live stereo validation/verifiedaxes+physicalgyrotime+usefulfusion, healthytwo-controller coexistence andadditionaltrackers/bodyroles. Continueworking; user explicitly forbids stopping at a partial pilot.

## K01697
Source `D0305`, original line43, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

## Pre-launch owner review and sensor range closure

## K01698
Source `D0305`, original line45, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

249 owner review corrected one preparation defect before RF: a pending-pair `continue` would have bypassed the common status/expiry/calibration tail and left cached radio optical status stale if one camera disappeared. Pair processing is now conditional, commontail alwaysruns; finalunconsumedstation source and invocation resealed. No hardware attempt had begun. Pipeline/actual-image contract unchanged; no redundant rerun.

## K01699
Source `D0305`, original line47, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Same-exposuremono/stereo comparison478pairs: positiondifference p50/p95/max7.05/9.59/13.34mm; orientationdifference2.36/4.84/5.89deg. This is estimatoragreement, not externalgroundtruth; don't force stereo to replicate monocular depthbias.

## K01700
Source `D0305`, original line49, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Exact sensor-mode path B162..B188 passes `r4 & 0x0f` to AB20/AADC. Thus confirmed sensor07 selects byte0x07, upperfull-scale bitszero at both0x4F/0x50.245actualnonce/chip-bound diagnostic snapshots show mode128/ID0x44 thenmode7/ID0x44 with advancingcounter137->95->130. TDK documented range therefore±4000dps and±32g/200Hz; older±2000dps conversion was wrong for this selectedbranch. Current0.0021305289rad/s/count matches4000*pi/(180*32768). Currentaccelscale uses9.81m/s²,0.034%above9.80665-basedvalue; this is far below the3%admission tolerance and does not explain observed~1.8%normoffset. Static source/rangeprovenance is not registerreadback, dynamicgyro-axis orphysicaltimeverification. Other sensorclasses/devicesremainunverified.

## K01701
Source `D0306`, original line1, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

# Phase250-252: actual stereo recording and timing diagnosis

## K01702
Source `D0306`, original line3, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

## Current disposition

## K01703
Source `D0306`, original line5, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

Phase249 is CLOSED, not active. Its150-second bound terminated after50.1 seconds because fixed267620 phase verification yielded11lit/52decoded exposures. No scan occurred and guarded command19 never started: zero IMU samples. All four cleanup checks passed and screen timeout returned to5. No OTA, no new firmware, no RF after249. Do not reuse the consumed249 attempt.

## K01704
Source `D0306`, original line7, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

Hardware evidence: fresh mode1/N1/target connection;35timed10 ACKs; two commanded-dark identity confirmations atframes101/128;264 optical publication log records (262 in final station snapshot due observation timing). These are not proof of smooth tracking or body tracking. User can remove HMD during offline work.

## K01705
Source `D0306`, original line9, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

## Offline geometry changes

## K01706
Source `D0306`, original line11, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

Phase250 decoded1238 actual upper-camera images from249 native recording, sampled at15Hz without interpolation. Phase251 tested619 same-exposure pairs against exact active camera calibration and tracker factory geometry. It accepted298, including20 three-LED continuations and278 four-LED measurements. Runtime p50/p95/max3.915/10.283/71.210ms.

## K01707
Source `D0306`, original line13, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

Three-LED continuation requires a fresh actual measured pose solely as association hint, noncollinear known factory geometry, separated LED image assignments in both cameras, joint reprojection/covariance gates and bounded correction. Bootstrap still requires four physical LEDs/eight rays. Bootstrap vectorizes model triangle distances and rejects beyond128 hypotheses immediately rather than performing an incomplete ambiguity search. No production activation or RF validation of251 occurred. Preserve measured source age and original IMU gates.

## K01708
Source `D0306`, original line15, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

## Actual brightness and clock evidence

## K01709
Source `D0306`, original line17, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

Phase252 redecoded original images to retain maxima and threshold counts. In the two-second fixed verification interval,13 of30 sampled pairs had zero detected LED blobs in both cameras. Multiple such pairs have maximum intensity3/255, zero pixels above60 and180: this is real near-black image content, not just correspondence rejection. Later frames gradually recover brightness and LED count. Similar losses recur around native10 ACKs approximately one second apart. ACK correlation does not establish LED command application time or root cause.

## K01710
Source `D0306`, original line19, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

The early candidate frame-index/source-time fit is33.332383ms/index with maximum residual2.802us; no evidence of doubled stereo frame counting in that span. Offline RAW-to-MONO mapping uses364 actual candidate/publication timestamps, maximum residual6.256us. It is a replay correspondence mapping, not a replacement for production clock admission or proof of microsecond phase accuracy.

## K01711
Source `D0306`, original line21, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

Roy SWD midpoint fit admits approximately280us uncertainty, substantially wider than37.233us camera exposure. A changed midpoint bias under optical computation load remains a hypothesis, not an established defect. Native10 timer correction and host/MCU time conversion need exact source review before choosing one justified repair. Lowering brightness thresholds cannot recover frames atmax3; three-point continuation alone cannot fix missing photons. Do not weaken48/61 phase admission, introduce blind phase sweeps, or start another unchanged hardware attempt.

## K01712
Source `D0306`, original line23, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

## Evidence and continuation

## K01713
Source `D0306`, original line25, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

- `outputs/phase251-dense-replay.json`: actual619-pair replay result.
- `outputs/phase251_stereo_solver.py`: offline-only conservative three-point/vectorized solver.
- `outputs/phase252/timing-diagnosis.json`: frame-index fit, replay bridge, decoded brightness, nearby native10 ACKs.
- `outputs/phase252/dense-brightness.json`: actual1238 decoded image statistics.
- `outputs/phase252_timing_diagnosis.py`, `phase252_brightness.py`: reproducible offline analyzers.
- Remote249 attempt retains original nativeDEV recording, clocks, radio proof and cleanup.

## K01714
Source `D0306`, original line32, SHA256 `bd52a0554d511853d0ac0c12a0ff44be9254f1f9114b2da710488c9a6a612f4e`. Status: historical/unadjudicated narrative; English source prose.

Next: correlate repeated brightness recovery with exact native10 timer branch and MCU retiming; establish one source-supported timing correction before another bounded live capture. Then obtain three distinct stationary gravity directions and dynamic axes/physical time validation, integrate guarded IMU fusion, validate both controllers, and enroll/assign additional body trackers. Existing static245 stereo replay identifies only TWO directions; fusion/body tracking remain unverified. User requires continuation until actual body tracking, without empty tests or repeated unchanged runs.

## K01715
Source `D0307`, original line1, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

# Phase253-258: source review and one computed optical candidate

## K01716
Source `D0307`, original line3, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Status:258 PREPARED/UPLOADED, NO RF. Uploaded148release/8additional files and runner hashes verified. Await fresh physical readiness. No RF after249, no OTA.249 cleanup all four true, power5. Grok/Cursor read-only review timed out without an answer; no independent-review approval is claimed.

## K01717
Source `D0307`, original line5, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

## Findings from existing recordings

## K01718
Source `D0307`, original line7, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Phase253 compared recorded Roy fits:245 read-width p5/p50/p95=542.859/692.558/2573.098us;249=543.214/752.113/4162.970us. Last10s ACK fit prediction spread relative to each final fit:24520.521us,2499.555us. These do not prove a stereo-load-induced offset or a hardware clock defect.

## K01719
Source `D0307`, original line9, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Exact installed image SHA040b304d verified.78f4..7968,79b8..7a78,8df0..8e12 are byte-identical to stock.785a..785d is the known diagnostic compare-hook branch to191f4, not unchanged stock. The exact installed helper preserves registers/APSR/PRIMASK, writes the original gate, records counters, restores state, executes original load and returns785e. No timing-hook corruption is established. Use M-class Capstone mode to decode PRIMASK instructions. Literal pools are data.

## K01720
Source `D0307`, original line11, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

IMPORTANT provenance:249 release contains legacy three-peer C source SHA0e318f..., which is NOT the source compiled into executed protected RAM ELF. Executed210 source SHA3db4e36..., ELF d8328717..., code63d319d4...2043bytes. The actual210 source supports peer0 only and phase-reference bit16, not cycle-window bit17. Its formula adds unsigned DWT difference/4 to reference ticks modulo533333 before byte emission/checksum. Do not infer executed capabilities from copied release C.

## K01721
Source `D0307`, original line13, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Phase254 decoded actual native images again and summed intensities>=20 as a brightness proxy. Latest consecutive sync intervals1020/1044 show first10percent normalized brightness420.613/420.624ms and first90percent753.937/753.948ms after recorded ACK. These are sampled15Hz, quantized/encoded images and received ACK times, not calibrated irradiance or exact LED application times. ACK intervals999.959/965.952ms. See `outputs/phase254/ack-brightness-profile.json`.

## K01722
Source `D0307`, original line15, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Phase255 fits actual sync-timer ticks against unwrapped IMU counters between command10 events (interior>=80ms,>=15samples,>=300ms span,maxresidual100ticks).239/242/245 admit47/61/77segments; median relative to historical counter-period seed is+50.466/+41.682/+15.085ppm. This measures a ratio to the sensor-counter cadence; it cannot distinguish current sensor ODR from absolute timer frequency and does not certify physical sample time. No fusion gate changed.

## K01723
Source `D0307`, original line17, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Phase256 receives217 successive native23 body01 reports, tags advance modulo64 without omissions. New tags13/14 and19/20 occur during both-camera near-black intervals. Exact native77F6..7828 report path follows the LED gate and permission checks. Thus the allowed pulse-report branch continues while decoded images lose LEDs. It does not prove electrical LED output or absolute phase. Arrival-time report regression is confounded by radio latency and native timer corrections; its32.825ms inferred five-period quotient MUST NOT be called measured physical timer period.

## K01724
Source `D0307`, original line19, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

## Same-recording geometry comparison

## K01725
Source `D0307`, original line21, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Exact old248/249 solver and new251 solver replay the same619 actual249 pairs with the same historical input/HMD/clock evidence. Old278 accepted; new298, including20 three-LED continuations (+7.19percent accepted measurements). Old compute p50/p95/max4.670/14.739/133.605ms; new3.915/10.283/71.210ms. P95 reduces30.23percent, max46.70percent in these offline runs. Hardware timing and body tracking remain unverified. Baseline remote `phase257/dense-summary.json`; new `phase251/dense-summary.json`.

## K01726
Source `D0307`, original line23, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

## One inferred phase correction, not a search

## K01727
Source `D0307`, original line25, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

The actual brightness ramp motivates a single centering candidate. Model assumptions: fixed37.233us exposure, approximately rectangular shared LED pulse, constant relative phase drift within an approximately1s sync interval, stable gain; threshold20 intensity is only a proxy. With observed10/90 times a=.4206,b=.7539s, ramp duration `(b-a)/.8=.4167s`. A rectangular pulse of widthw gives driftv=w/.4167. Brightness centre is `(a+b)/2 + exposure/(2v)`. To move it to the interval midpoint, positive phase correction is approximately `exposure/2 + v*((a+b)/2 - interval/2)`.

## K01728
Source `D0307`, original line27, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Historical234 observed full-light samples at267492 and267748 (at least16us apart), with fixed exposure37.233us. If that full-light plateau and pulse model still apply, w<=21.233us and correction is approximately18.6..23.5us. This is an INFERRED bound, not a calibrated physical pulse-width measurement: sparse phase samples, drift, gain and encoder behavior can invalidate it. Choose22us =>352ticks => ONE fixed267972. Native79b8 sign shows increased desired phase advances timer phase. No new sweep, trial list or guaranteed benefit is claimed. Older234268004 gave only7/120 lit; the candidate is based on current249 brightness timing and must not inherit old optical admission.

## K01729
Source `D0307`, original line29, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

## Prepared258 and required outcome

## K01730
Source `D0307`, original line31, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

258 copies the exact consumed249 release/build into a new attempt, replaces only the stereo solver with251, sets computed fixedphase267972, and records first actual timed10 mailbox reference publication (generation/reference cycles/ticks/target/fit). The owner pending-pair fix is retained. Exact invocation hashes resealed; no prior output/phase result copied.

## K01731
Source `D0307`, original line33, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Three-LED continuation needs a fresh<=250ms actual measured association seed, known noncollinear factory geometry (second singular value>=3mm), separated image assignments by>=2px, independent joint calibrated ray fit, original residual/covariance gates, correction<=5cm/0.3rad. Bootstrap still requires four physical LEDs/eight rays and exhaustive<=128 hypotheses; excessive triangulation/hypothesis capacity rejects. No prior pose itself is published.

## K01732
Source `D0307`, original line35, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Actual new-path publication lifecycle replay passed using untouched native4+3 images:3physicalLEDs/6rays/residual0.171421px. Actual prior ray-fit age133.329ms, current measured position differs1.521mm from that seed. Waiting for second camera, geometry candidate, unverified temporal identity and250ms+1stale source publish nothing; verified identity publishes the current three-point measurement once; axes/time/IMU remain unverified. First fixture incorrectly assumed a past live publication existed; it did not. Correct fixture uses the actual past independently fitted four-point geometry from the recorded replay, which is a permitted association seed without publication authorization. No generated pixels or altered raw observations. Evidence `outputs/phase258/real-frame-contract.json`. This is an offline boundary check, not live tracking proof.

## K01733
Source `D0307`, original line37, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Unchanged: native mode1/N1 fresh proof, producer/gate proof before19, temporal two-dark/six-lit identity, fixed verification>=48decoded AND>=48lit in61frames, source age, IMU axes/time calibration and propagation limits, protected210 RAM/FW189,6000/900RF, both controllers off, timeout restoration and cleanup. No weakening of missing-data/phase/calibration gates.

## K01734
Source `D0307`, original line39, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Before RF: fresh physical HMD/PICO readiness; runtime preflight and exact loaded code/hash checks. One<=150s run only; if fixed phase fails, stop beforeIMU19, retain data and close all four cleanup checks. If IMU begins, capture untouched baseline and two distinct arbitrary supported tilts10s each; no left/right labels needed. Then evaluate actual three-direction coverage, driver freshness and physical time/dynamic axes. No automatic repeated run.

## K01735
Source `D0307`, original line41, SHA256 `4a5ee423f8589554db17ff08c8cdb4061ef424fa3798a000583844f4de462e13`. Status: historical/unadjudicated narrative; English source prose.

Full-body remains unfinished: current exact RAM capability is single-peer; dynamic fusion, occlusion behavior, both-controller coexistence, multiple tracker enrollment/runtime and body roles require separate measured completion. Holding a last known model as result201 is not fresh body tracking.

## K01736
Source `D0308`, original line1, SHA256 `b9884eae4f46c5b6accd3697b90af0b610eb6423318e4445fd97a8e7363f50d1`. Status: historical/unadjudicated narrative; English source prose.

# Phase261: recorded photon closure, 2026-10-11

## K01737
Source `D0308`, original line3, SHA256 `b9884eae4f46c5b6accd3697b90af0b610eb6423318e4445fd97a8e7363f50d1`. Status: historical/unadjudicated narrative; English source prose.

## Hardware closure
Phase258 stopped after 49.8 seconds. Fixed phase267972 (+22us hypothesis) failed:6lit/55decoded, temporal identity false. Command19 never started; IMU samples0, optical publications0. All four restoration checks passed; RF closed and screen timeout restored5. The consumed attempt and sealed source remain unchanged. No OTA occurred.

## K01738
Source `D0308`, original line6, SHA256 `b9884eae4f46c5b6accd3697b90af0b610eb6423318e4445fd97a8e7363f50d1`. Status: historical/unadjudicated narrative; English source prose.

## Recorded evidence
Phase260 reprocessed640 actual native-video stereo pairs from258:12 geometry acceptances, including7 actual-prior continuations. Compute p50/p95/max3.997/15.501/81.902ms. These offline solutions do not authorize live pose publication or fusion. Photon counts below use full video sampled15Hz, rather than the retained raw-camera subset biased toward bright frames. Different recording intervals and physical arrangements prevent causal A/B claims.

## K01739
Source `D0308`, original line9, SHA256 `b9884eae4f46c5b6accd3697b90af0b610eb6423318e4445fd97a8e7363f50d1`. Status: historical/unadjudicated narrative; English source prose.

{
  "249": {
    "actual_decoded_pairs": 619,
    "both_maximum_below_20": 222,
    "both_zero_detected_blobs": 237,
    "both_at_least_three_blobs": 379,
    "both_at_least_four_blobs": 278,
    "blob_pair_histogram": {
      "(0, 0)": 237,
      "(6, 4)": 244,
      "(3, 3)": 83,
      "(4, 3)": 13,
      "(5, 4)": 30,
      "(4, 4)": 4,
      "(5, 3)": 5,
      "(3, 1)": 1,
      "(3, 2)": 1,
      "(2, 1)": 1
    },
    "scope": "Actual native-video frames sampled at 15 Hz. Different recordings, not controlled causal A/B."
  },
  "258": {
    "actual_decoded_pairs": 640,
    "both_maximum_below_20": 275,
    "both_zero_detected_blobs": 503,
    "both_at_least_three_blobs": 133,
    "both_at_least_four_blobs": 17,
    "blob_pair_histogram": {
      "(0, 0)": 503,
      "(3, 3)": 51,
      "(4, 3)": 10,
      "(5, 3)": 27,
      "(6, 3)": 6,
      "(7, 4)": 8,
      "(0, 1)": 2,
      "(5, 4)": 2,
      "(7, 3)": 22,
      "(6, 4)": 7,
      "(3, 1)": 1,
      "(3, 2)": 1
    },
    "scope": "Actual native-video frames sampled at 15 Hz. Different recordings, not controlled causal A/B."
  }
}

## K01740
Source `D0308`, original line54, SHA256 `b9884eae4f46c5b6accd3697b90af0b610eb6423318e4445fd97a8e7363f50d1`. Status: historical/unadjudicated narrative; English source prose.

## Next work and completion boundary
Use existing phase_plan_published/reference cycles/ticks and actual native10 ACKs to reconcile command timing with recorded photon coverage. Do not guess another phase or repeat unchanged RF. Preserve identity, age, covariance, controller and IMU guards. Stable optical tracking, measured axes and physical time, dynamic fusion, controller coexistence, multiple-peer RAM/runtime and body roles remain required. The executed210 module is single-peer; legacy three-peer source copied into releases is not executed-module evidence.

## K01741
Source `D0309`, original line1, SHA256 `75dfd92c64aeca1070dfc3612dee68f6b630aabefa5bd656d20f4e8bb4b437cc`. Status: historical/unadjudicated narrative; English source prose.

# Phase262–268 continuation, 2026-10-11

## K01742
Source `D0309`, original line3, SHA256 `75dfd92c64aeca1070dfc3612dee68f6b630aabefa5bd656d20f4e8bb4b437cc`. Status: historical/unadjudicated narrative; English source prose.

## Hardware and human state
No RF or OTA since failed258. All four258 cleanup checks independently true; screen timeout restored5. Original consumed attempt unchanged. Frame initially unavailable by SSH; user powered it, SSH subsequently available. User reports PICO removed from strap, on USB charge before HMD. Fresh mode1/target proof is required in any next hardware attempt. User need not wear HMD during offline work. Full body goal remains unmet; do not promise completion today.

## K01743
Source `D0309`, original line6, SHA256 `75dfd92c64aeca1070dfc3612dee68f6b630aabefa5bd656d20f4e8bb4b437cc`. Status: historical/unadjudicated narrative; English source prose.

## Clock closure
35 actual258 timed10 mailbox references matched to ACKs. Reference-epoch fit revisions p5/p50/p95 are−0.931/0/+2.419us, while midpoint-fit uncertainties are278..325us. This does not certify physical LED phase or explain all dark images. Native mode2 compare interval calculation can select128 or288ticks (nominal8/18us), conditional on unmeasured F9A/F76; neither is a measured electrical pulse width. Cursor/Grok narrow review timed out after210seconds; no independent verdict or quota failure was received, no automatic retry.

## K01744
Source `D0309`, original line9, SHA256 `75dfd92c64aeca1070dfc3612dee68f6b630aabefa5bd656d20f4e8bb4b437cc`. Status: historical/unadjudicated narrative; English source prose.

Native77F7..781C increments a uint8 LED-report counter, then checks modulo5. Counter255 reports; counter0 reports one pulse later. There are52 reports per256 pulses. Offline source-constrained52-origin fit on217 original249 reports reduces receive residual RMS40.346→10.670ms and gives effective33.339791ms pulse period. Arrival latency and native10 corrections remain confounders; do not call this an absolute physical clock or use it for a speculative phase correction. Evidence outputs/phase262 andphase263.

## K01745
Source `D0309`, original line11, SHA256 `75dfd92c64aeca1070dfc3612dee68f6b630aabefa5bd656d20f4e8bb4b437cc`. Status: historical/unadjudicated narrative; English source prose.

## Multi-peer RAM work
Exact original210 compiler/flags reproduced baseline code2043bytes and original code/ELF hashes. Three-peer candidate retains mailbox seqlocks/timed-reference validation, CRC/ring bounds, fixed slot deadline, bounded waits, controller phase exclusion, per-peer pause and global diagnostic pause; handshake stage2 priority restored. It measures2121bytes, exceeding2045 by76. Parameter-passing compaction measured2125, rejected. Neither candidate loaded or approved. Enlarged4096-byte linker is strictly size measurement, never deployment. Exact candidate metadata inoutputs/phase264/remote-size-report.json andphase265. Local Zig attempt failed due toolchain option differences; original remote compiler used for quantitative results. Before any deployment: fit, exact linker, native audit, capabilities/runtime/host binding, fresh admission and restoration proof.

## K01746
Source `D0309`, original line14, SHA256 `75dfd92c64aeca1070dfc3612dee68f6b630aabefa5bd656d20f4e8bb4b437cc`. Status: historical/unadjudicated narrative; English source prose.

## Native camera controls and completed-frame inspection
Copied actual running native binaries tooutputs/phase266. ELF PT_LOAD-aware string/code xrefs locate ControllerExposureSlider atc5d6b0, direct settercabeb0→e1cd40→capture source vtable slotd8. Slider range nominal0.01..20ms. UI setter is a direct live call; JSONtrackingCameras initialExposure is not proven same controller IR path. Do not edit it as a guessed fix. vrcmd help returned unknown command; no mutation/RF occurred. Need a supported way to invoke the actual IR exposure control or a source-supported photon phase repair.

## K01747
Source `D0309`, original line17, SHA256 `75dfd92c64aeca1070dfc3612dee68f6b630aabefa5bd656d20f4e8bb4b437cc`. Status: historical/unadjudicated narrative; English source prose.

Actual completed258 recording contains1780frames perstream. Controller/IR kind38 upper cameras:37.233334us exposures,33.333333ms periods. Same serials in normal kind32:8362.799644us exposures, same period, timestamp offset approximately16.63ms. Offline bounded segment decoded1280 normal images at15Hz; default detector finds24..583 ambient blobs. A native normal-image preview was inspected, showing room/object detail. These are not verified PICO LED correspondences or a drop-in optical solution; do not loosen bootstrap capacity/identity/age guards. Original IR-only stereo replay12/640 excludes mono fallback and is not a full-pipeline accepted count.

## K01748
Source `D0309`, original line19, SHA256 `75dfd92c64aeca1070dfc3612dee68f6b630aabefa5bd656d20f4e8bb4b437cc`. Status: historical/unadjudicated narrative; English source prose.

## Next action and actual completion criteria
Resolve actual short-frame exposure control or the physical synchronization error with existing source/evidence; do not guess another fixed phase. Preserve timed-reference, identity, covariance, age and IMU gates. Then one sealed purposeful capture with fresh mode1, measured optical coverage and explicit distinguishing result. Calibration still lacks three distinct stationary gravity directions and verified dynamic axes/physical time. Multi-peer RAM/runtime, simultaneous Frame-controller health, body roles and actual in-headset body motion remain required. Persistent201 last-known rendering is not fresh tracking.

## K01749
Source `D0310`, original line1, SHA256 `0065385201ece546102b054852effc250b3951fe66bbacdaec946a9d41dee063`. Status: historical/unadjudicated narrative; English source prose.

# Phase269–274: native camera profile and recorded geometry

## K01750
Source `D0310`, original line3, SHA256 `0065385201ece546102b054852effc250b3951fe66bbacdaec946a9d41dee063`. Status: historical/unadjudicated narrative; English source prose.

## Hardware evidence
One sealed273 attempt used the existing258 host release and protected210 RAM, fixed267972 phase, controllers off, tracker USB charging without strap. Installed diagnostic10 firmware unchanged; no OTA. Native captureSessionSettings/trackingCameras/streamingMode=3 was derived from actual XRService code and confirmed before radio by six upper-camera frames each at782.266676us. Previous exposure37.233us:21.01x increase. Mode3 is Controller30HzRetroreflector.75ms and changes native emitter behavior as well as exposure; this is not a clean exposure-only experiment.

## K01751
Source `D0310`, original line6, SHA256 `0065385201ece546102b054852effc250b3951fe66bbacdaec946a9d41dee063`. Status: historical/unadjudicated narrative; English source prose.

Finite optical verification failed and stopped the child: command19 never started, zero IMU samples and zero optical publications. Early43.1s observation contained11 KB inversion domain errors and8 computation-age failures, with a candidate computation820.593ms. These are an intermediate snapshot, not final totals or a proved sole cause. All four MCU restoration checks true. Owned camera closed; original XRService settings restored; screen timeout5 restored. Managed XRService PID38729 observed after restoration. Wrapper SIGINT was issued after the child had already failed and entered cleanup; it interrupted the final restart wait. It did not stop an active RF run. Preserve original logs and profile-cleanup plus this independent restart observation.

## K01752
Source `D0310`, original line8, SHA256 `0065385201ece546102b054852effc250b3951fe66bbacdaec946a9d41dee063`. Status: historical/unadjudicated narrative; English source prose.

## Static evidence
DeckardCaptureSource vtable3171640 slotd0 targetsf62650; enum3 stores gain1/exposure.75ms and enables control. JSON captureSessionSettings/trackingCameras/streamingMode path derived frome1e494 and UI persistence. Enum labels at316af88, outputs/phase272/mode-labels.json. Direct manual exposure setter remains separate slotd8. Do not write initialExposure as an IR setter. Multi-peer unrolled selection270 measures2125bytes, worse than264's2121 and above2045 capacity; rejected, never loaded.

## K01753
Source `D0310`, original line11, SHA256 `0065385201ece546102b054852effc250b3951fe66bbacdaec946a9d41dee063`. Status: historical/unadjudicated narrative; English source prose.

## Recorded evidence and limits
Native273 upper-camera recordings preserved remotely. Each extracted stream has1713 frames. Best-brightness raw PNGs show five apparent tracker LEDs in each view; they are not necessarily simultaneous and do not establish identity or valid SteamVR pose. raw_pixel_evidence.save_frame overwrites a PNG only when brightness score improves. Do not associate the PNG with the first pixels.jsonl record.

## K01754
Source `D0310`, original line14, SHA256 `0065385201ece546102b054852effc250b3951fe66bbacdaec946a9d41dee063`. Status: historical/unadjudicated narrative; English source prose.

Actual right PNG detector finds nine blobs; one background observation near322.5,997 is outside factory KB inversion's monotonic domain. Existing stereo solves the whole pixel array and this one invalid ray can reject other valid observations. Five apparent tracker LEDs individually have valid factory rays. Proposed correction rejects only KB-invalid observations before solving while preserving calibration, bootstrap ambiguity capacity, inlier/covariance/age/identity guards. This is offline only, not yet deployed or proved sufficient.

## K01755
Source `D0310`, original line16, SHA256 `0065385201ece546102b054852effc250b3951fe66bbacdaec946a9d41dee063`. Status: historical/unadjudicated narrative; English source prose.

Initial1Hz replay58 actual stereo pairs: original0 accepted, individually filtered0 accepted. Filtered reasons42 fewer-than-four stereo-consistent points,16 bootstrap capacity exceeded. It may alias IR flashing; dense15Hz replay of the same completed recording is in progress. Never claim the sparse replay exhausts the recording. No new radio/phase sweep is needed for this analysis.

## K01756
Source `D0310`, original line18, SHA256 `0065385201ece546102b054852effc250b3951fe66bbacdaec946a9d41dee063`. Status: historical/unadjudicated narrative; English source prose.

## Continue
Finish dense replay, quantify actual geometry and computation improvement before preparing a changed host release. Preserve full ambiguity and optical identity guards; do not truncate candidates to force success. If a purposeful next live capture is prepared, request fresh HMD wear readiness, verify actual mode1 again and confirm exposure before RF. No unchanged retry or new firmware. Remaining fullbody work: reliable live optical coverage, actual IMU axes and physical time, validated fusion and occlusion behavior, fitted protected multi-peer RAM/runtime, Frame-controller coexistence, enrolled trackers/body roles and actual body motion in SteamVR. Persistent last-known rendering is not fresh body tracking.

## K01757
Source `D0311`, original line3, SHA256 `95e8ccaaeaeeca24bb9f3769901437b689fb96dc867899c4f5e97826f98e2864`. Status: historical/unadjudicated narrative; English source prose.

## Expected-dark closure285 (offline, 2026-10-11)

## K01758
Source `D0311`, original line5, SHA256 `95e8ccaaeaeeca24bb9f3769901437b689fb96dc867899c4f5e97826f98e2864`. Status: historical/unadjudicated narrative; English source prose.

Actual273 ACK32 expected-dark predictions:18 match preserved15Hz recorded pairs within500us;15 of these contain accepted factory tracker geometry (9 stereo,6 mono fallback).14 predicted frames are not present at this recording sampling parity and cannot be classified. The accepted geometry contradicts a universal dark-pulse assumption; it is not verified chip identity/live publication. Index fit maximum residual32.284us. See outputs/phase285/expected-dark-frames.json; raw blob counts alone were deliberately not treated as target light.

## K01759
Source `D0311`, original line7, SHA256 `95e8ccaaeaeeca24bb9f3769901437b689fb96dc867899c4f5e97826f98e2864`. Status: historical/unadjudicated narrative; English source prose.

Static installed native79b8..7a56: actual correction stored through literal7a74 at RAM0x20000f78; firmware image file base1000 must be subtracted when reading native addresses. Only negative correction below-8 takes7a32/760c and sets skip latch20000f98 at7a42. Positive correction/no-op does not take that branch. Existing diagnostic10 firmware already exposes this latch via schema5 byte29 (outputs/phase176/build_observer.py); NEW OTA is unnecessary for that observation. Snapshot latch0 after a pulse clears it cannot disprove an earlier skip; freshnonce/chip plus atomic command ordering/timing must be established before using a future measurement. Retained273 observer-page counts:{'2': 4, '3': 4, '4': 4, '9': 3, '5': 1, '6': 4, '7': 3, '8': 6, '10': 3, '11': 3}; actual saved schema5 rows:1. No new RF or tests.

## K01760
Source `D0311`, original line9, SHA256 `95e8ccaaeaeeca24bb9f3769901437b689fb96dc867899c4f5e97826f98e2864`. Status: historical/unadjudicated narrative; English source prose.

Next concrete task: establish source-backed native10/query5 ordering and reply/slot budget on the current protected module, or extract sufficient existing snapshots, before proposing one bounded causal measurement. Do not infer the native correction from planned reference_ticks/adjust_ticks. Preserve all existing temporal identity,48/61,age,covariance,IMU and controller guards. Accelerated host279 is still isolated/undeployed. Fullbody remains unachieved.
