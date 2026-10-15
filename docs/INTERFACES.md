# Reimplementation contracts and versioned interfaces

This is an explanatory specification derived from the inspected first-party interfaces. It is not a copy of their implementation or an assertion that a new implementation is already admitted. Obtain the matching source and confirm the version before integration.

## PTP2 measured-pose loopback

The current inspected Python/C++ pair uses a packed88byte little-endian record over UDP localhost39571. Time is Linux CLOCK_MONOTONIC nanoseconds ON FRAME, not a remote PC clock. The producer must first transform calibrated optical measurements into OpenVR standing space.

| Offset | Size | Field |
|---|---|---|
|0|4|ASCII magicPTP2|
|4|4|uint32 chip identity, nonzero|
|8|4|uint32 flags:bit0connected,bit1pose_valid,bit2verified battery present; all other bits rejected|
|12|8|uint64 sequence|
|20|8|uint64 source_time_ns|
|28|12|float32 positionXYZ in OpenVR world metres|
|40|16|float32 quaternionXYZW; consumer normalizes admitted norm|
|56|12|float32 linear velocityXYZ|
|68|12|float32 angular velocityXYZ|
|80|4|float32 battery percent0..100 if verified, otherwise exactly−1|
|84|4|float32 quality0..1|

Admission requires correct magic/size/flags, finite values with absolute value≤1e6, nonzero source time not in the future and age≤250000000ns, quaternion squared norm0.81..1.21, and positive quality plus connected when pose_valid is true. Unknown battery stays absent/−1, never guessed. Field-size/packing checks matter because sequence/source are unaligned in this packed layout.

The driver sets poseTimeOffset from the actual source age. Connected-but-out-of-range and a fresh measured valid pose are different states. Invalidation after missing/stale/uncertain data is mandatory. A sequence accepted by the transport does not authorize the producer to claim a measured pose.

## Camera binding and transforms

Each recording frame must identify the source camera and match exactly one local factory serial and active calibration entry. The inspected KB schema requires cameraModelkb, supported zero pre-rotation, positive focal lengths, finitefx/fy/cx/cy/k1..k4, exactwidth/height and monotonic inversion in the admitted pixel domain. Crop/thermal/half-pixel conventions must be established by the caller, not inferred from image size.

Camera rays are unit rays in the camera frame with positiveZ forward. No OpenVR/world transformation is implicit in pixel-to-ray conversion. Compose measured camera-to-tracker with actual same-exposure head/world pose and the appropriate HEAD/CAD/reference/active-camera chain. Validate units and rigid axes. Do not catch an out-of-domain KB inversion as a valid ray.

## Native mode and notifications

The authored FW189 interface uses schema/nonce/identity-bound queries and switches; active mode, pending mode andN are separate. Preserve per-command6bit sequence,32bit/actual transport tag width and current peer link attribution. Native16 transport ACK can trigger a fresh query, but only an exact native reply can complete applied-mode proof. Mode proof expires on its finite lease and is revoked by disconnect/target change.

Timed optical10 uses its exact current8byte command packet within16byte timing metadata in the selected path. Sample feedback11 is a separate independent command; it must not be concatenated into that packet. Native19 admission requires the actual current producer state, upstream gates and optical/clock/mode proofs, not an ACK or generated fixture.

## Historical SDK-PC bridge, not the standalone path

The separate SDK3.4.0/Unity2022.3.22f1 prototype used TCP39500 with12byte little-endian header: uint32magic0x31425450(PTB1),uint16version1,uint16type,uint32payload length. Type2Hello is UTF-8JSON; type1Frame carries headset monotonic seconds, frame counter, HMD7floats and variable tracker records. Pose uses Unity left-handed+Yup/+Zforward metres andXYZW quaternion after SDK conversion; raw SDK velocities need an explicit frame/unit audit. PC output used OSC/VMT localhost39570.

These fields document an alternate architecture only. Do not feed its PC clock/Unity poses into PTP2 without explicit conversion and Frame-time/world calibration, and do not claim it meets the onboard-Roy standalone objective.
