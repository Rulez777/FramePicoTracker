# Cameras, geometry, clocks and causal optical identity

## Acquisition and calibration

Native/OCC camera recording and H264 packet metadata are primary inputs. Preserve packet-to-decoded-frame timestamps and dimensions; a decoder receive timestamp is not exposure time. Bind upper cameras by their actual local factory serial, not list ordinal. The recipient must acquire their own private sensor IDs and calibration; this package deliberately substitutes camera aliases.

Upper cameras were observed at1056x1024 and actual30fps/33.333ms. Some lower640 streams were black in the retained setup. A60fps request is not a60fps result. Actual native profile3 upper exposure was approximately782.266676us; the recent unmodified passive run instead had37.233334us. Profile changes can affect emitter behavior too, so exposure must be measured per invocation.

Use active flattened calibration corresponding to the exact recording/profile, not unflattened parent-relative factory extrinsics. Camera positions declare millimetres while CAD/head transforms use metres. Bind KB intrinsics and sourceCamera consistently; validate axis handedness, orthonormal rotation, determinant, dimensions and half-pixel convention. HEAD-from-camera, WORLD-from-head and camera-from-tracker must refer to the same actual exposure. Derive the eight-LED model from your own admitted tracker calibration; no factory coordinates are included.

## Time

RAW, MONOTONIC, BOOTTIME, Roy DWT and tracker sample counters are different domains. Require measured current-boot mapping, integer offset and uncertainty at most500us for camera admission. Exposure must be positive and below1ms; current source age must be0..250ms. A negative MONOTONIC comparison is not a fresh frame. Retained calibration/clock files from another boot never prove the current mapping.

LED phase uses the investigated16MHz timer and533333ticks/frame, not unrestricted24bit wrap. Preserve MCU retiming and source clock semantics across publication, actual TX and applied native command10. Do not infer physical pulse location from a transport ACK or a CPU-load correlation.

## Detection and geometry history

Blob detection, cluster association, P3P/root search, stereo triangulation, refinement and publication are distinct stages. The native-derived contrast/contour investigation found adaptive11x11 replicate background, contrast30 and state-dependent thresholds210/130/70; this is original explanatory research, not redistributed native code. External contours versus older alternatives matched on the tested120observations; unchanged repeats do not add evidence. A threshold140/1771 approximation is not the actual native contour pipeline.

Reported mono improvements did not automatically improve stereo. Native-derived detector305 yielded59accepted mono fits on selected60paired frames but the old retained-pose seeded stereo308 accepted none. Cross-view analysis and triangulated seed310 accepted20/30selected stable fits; helper364 accepted21/59actual retained pairs, all21within30selected stable pairs. These conditional denominators are not live success rates or physical accuracy estimates. Current mono hints cannot be treated as independently known correspondences.

Preserve at least4 common stereo-consistent physical LEDs, separated assignments, baseline>2cm, ray ranges0.1..2m, conditioning<200, noncollinear factory model, reprojection/covariance checks, ambiguity and hypothesis capacity. Selected later solver uses residual1.5px and cap128; exact release parameters must come from the admitted source, not a mixture of old RayPoseSolver2.5px defaults and later stereo gates. Local covariance does not resolve globally competing roots.

## Identity and freshness

Candidate geometry may be computed without publication. Causal target identity requires the actual commanded dark/lit episode on the current link, current camera frames and current source/boot mapping. Existing temporal admission uses two independently attributed dark events with six stable lit brackets; later native25 episode work additionally checks real decoded dark ROI frames, ordered tags/publication generations, per-exposure head poses and restoration. Missing/late/invalid frames never count as darkness. Preserve the separate48accepted-lit/48decoded-in61gate in phase admission.

Native25 OFF and ON need independent transport, application and observed optical restoration accounting. Do not replace an unacknowledged OFF, guess a sequence, or clear restore_required from an ON ACK alone. Failures must retain unresolved restoration state and close owned camera/RF safely.

Acquisition candidates can be retained for1s while identity publication requires250ms; filtering already stale candidates before costly geometry avoids wasted work without admitting stale data. Do not widen either gate to make a pilot appear to work. Reusing a current computed candidate must preserve exact source identity and decision provenance.

## Latest evidence and next discriminator

The900-pair passive result has no fits; sampled37us frames have too few detected markers. That result does not validate causal identity, predict profile3 success or establish absence of light. The prepared profile3 probe changes one justified condition, requires measured750..850us exposure and restores settings/power, without radio/LED/IMU/pose publication. Its completion is absent. A new human-ready bounded attempt is required; no profile change was made during handoff creation.

Benchmark vectorization preserves recorded decisions and numeric results but is not deployed. Ray memoization is off. Profile/cProfile overhead is not baseline latency. Compare entire selected pipeline on identical actual paired inputs before interpreting timing gains.
