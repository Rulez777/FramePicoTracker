# Shared control-library dependency list. Source from either build entry point.
# Unit names deliberately omit extensions so native/build.sh can build archives.
PICO_CONTROL_UNITS='openocd_process roy_owner tcl_transport owner_lifecycle image_admission radio_loop peer_engine station_control query_peer native25 native_identity multi_optics'
pico_control_sources() {
    for pico_unit in $PICO_CONTROL_UNITS; do
        printf '%s%s.cpp ' "$1" "$pico_unit"
    done
}
# All library/adapter translation units used by the native executables. Audit
# these together so lazy archive extraction cannot conceal an ODR violation.
pico_library_sources() {
    pico_control_sources "$1"
    for pico_unit in data_plane camera_transport live_camera decoder_process recording_replay optical_timing pose_publication optical_worker replay_calibration openocd_os producer_optical_fixture live_policy live_session phase5/tracking_guard phase5/orchestrator phase5/sha256 phase5/os_adapters; do
        printf '%s%s.cpp ' "$1" "$pico_unit"
    done
}
