#pragma once
#include "device_config.hpp"
#include "query_peer.hpp"
#include "sensors.hpp"

namespace pico {
// Called only inside the canonical DataPlane callback, after QueryPeer receive.
// Command19 credit is issued by the installed action hook after LaterImuStart
// admission and durable action evidence, never by receipt of an IMU packet.
class LiveImuRouting {
    bool product_ = false, admitted_ = false;
  public:
    void reset() {
        product_ = false;
        admitted_ = false;
    }
    void action(uint8_t command, const Bytes& body) {
        if (command == 19) {
            if (body != Bytes{0})
                throw std::runtime_error("Unexpected admitted command19 body");
            admitted_ = true;
        }
    }
    void canonical(DataPlane& plane, const QueryPeer& peer,
                   const std::vector<Command>& commands) {
        plane.sensor_format=sensor_format_for_revision(peer.firmware_revision,peer.stock_firmware);
        if(peer.sensor_status.mask&1) plane.imu_id=peer.sensor_status.raw[1];
        for (const auto& command : commands) {
            const auto& b = command.body;
            if (command.command != 1 || b.size() != 35 || b[0] != 0x82)
                continue;
            const unsigned firmware = unsigned(b[1]) | (unsigned(b[2]) << 8);
            const std::string serial(b.begin() + 8, b.begin() + 25);
            if (firmware < 1 || firmware > 9999 || serial != pico::devices::serial())
                continue;  // another product: not routed, never a run abort
            product_ = true;
        }
        const bool connected = peer.state != PeerState::Absent &&
            peer.state != PeerState::Reconnecting && peer.state != PeerState::Stale;
        const bool allowed = product_ && admitted_ && connected &&
            peer.chip == pico::devices::chip_le() && peer.slot == 0 &&
            peer.mode_verified() && peer.sample_feedback_enabled;
        plane.enrolled = allowed;
        plane.feedback_enabled = allowed;
    }
};
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_imu_routing, "imu-routing");
