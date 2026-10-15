
## Included source supplement — 2026-10-15

The user subsequently authorized inclusion of project-authored implementation source. Read [source supplement](../source/README.md) and its exact [file/exclusion manifest](../verification/SOURCE_SUPPLEMENT.json). This supersedes earlier blanket statements that no implementation is bundled. Some private inputs, captured fixtures and third-party dependencies remain excluded; no clean-machine full build is claimed. Work in a separate development checkout, not this integrity-checked folder.

# Source map, environment and reproducible build procedure

The live private project's active instructions are AGENTS.md→docs/CURRENT_WORK.md. Its historical append-only AGENTS diary was preserved at docs/history/context-reset/AGENTS.before-compact-2026-10-11.md. This package updates the current summary from later saved optical results without changing that private chronology.

| Component | Rights-cleared source to obtain | Contract |
|---|---|---|
|Native dashboard|pc/frame_app/|C++17/OpenVR/Vulkan/Cairo/libcurl; Game/Service and truthful sensor/pose state.|
|Diagnostic agent/API|pc/radio/frame_debug_agent.py and client|Owner-bound stream, loopback18741, bounded operations, sealed profiles and no arbitrary command endpoint.|
|Roy loader/runtime|pc/radio/ram/ and exact frozen273/210 sources|Image-specific SRAM layout, native calls, exclusive owner, four actual teardown barriers.|
|Tracker protocol|pc/radio/swift_*, tracker_* and packet code|CRC/sum8, identity/nonce, sequences, mode lease, OTA separation, current producer proof.|
|Camera/timing|pc/optical/live_source.py, live_decode.py, measurement_clock.py, frame_camera_rig.py|Actual packet/exposure clocks, serial-bound calibration, per-boot mapping.|
|Geometry/identity|pc/optical/measured_pipeline.py, station_optics.py, temporal_identity.py; frozen pilot helpers|Actual associations, unchanged rejection gates, causal identity, freshness, uncertainty.|
|Fusion/output|pc/optical/fusion_ekf.py, imu_calibration.py; pc/driver/pose_frame.py and native driver|Measured axes/time/bias only, explicit invalidation, PTP2/OpenVR.|
|Tests/replays|pc/tests/, phase-specific offline scripts|Reproduce a specific defect; distinguish real recorded input from explicit simulation.|

## Host setup

Use Python3.12+ and a separate environment. Core historical packages include numpy, openvr, python-osc and pytest; optional offline/camera work uses OpenCV, Pillow, Capstone, pyelftools, ffmpeg and Brotli. Pin the actual versions from the received source environment/lockfile rather than guessing from the current latest release. Windows and Frame ARM64 environments are separate; compiled wheels/ABIs must match.

Current local test interpreter reference was outputs/phase386/test-venv, with PYTHONPATH pointing to pc. Older .venv and phase291/deps are historical/optional and are not shipped. Use an explicit interpreter path on Windows: the unqualified python alias can open Microsoft Store rather than execute Python. Set UTF-8 explicitly for JSON/Markdown; locale-default decoding can create false Cyrillic/language failures.

Run only relevant offline checks after reproducing the actual old failure. A green synthetic test does not prove device timing, valid pose, controller navigation or publication. No complete test suite was rerun during handoff packaging. Source aliases and hashes let a recipient select exactly the tested version rather than a newly copied similarly named file.

## Native dashboard and dependencies

Build requires a C++17 compiler, pkg-config, Cairo, libcurl, Vulkan headers and installed SteamVR LinuxARM64/OpenVR runtime. Existing build scripts in the publication source are scripts/build_frame_app.sh and scripts/prepare_overlay_dependency.py. Obtain frame-mic-tuner separately from `https://github.com/sasaken1102r/frame-mic-tuner` at recorded commit`d1e8c830b00928ab94a44e1d5323613af95263c6` and retain its upstream MIT notice. Obtain official OpenVR headers separately from ValveSoftware/openvr; do not bundle their runtime here.

Publishing a native executable is separate from installing a device service, enabling radio or validating tracking. Do not auto-enable persistent startup before measured power-cycle reconnect, controller coexistence and pose quality. All actual deployment/execution requires the exact target/source/config/invocation and current authorization.

## Research tooling and assistants

Helpers must read project instructions, receive bounded evidence paths and return evidence-labelled findings. Their timeout is not a verdict. Grok is selected through Cursor with a non-fast model; Gemini3.8High calls should be bounded. During this packaging task the user explicitly approved translating anonymized historical prose with Gemini. Every numeric value/hash/address/path/identifier was replaced with an opaque placeholder before translation and restored locally; the private map was not sent. Exact placeholder/block coverage is checked. A translated paragraph is still historical evidence, not a newly verified experiment.

Do not copy local account credentials/MCP settings into a project handoff. Corporate/personal AI-account separation is environment policy, not a project secret to publish. Windows sandbox CryptUnprotectData failure is a host execution issue; it is not evidence about the tracker or firmware.
