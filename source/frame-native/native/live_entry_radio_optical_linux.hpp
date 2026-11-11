// Included inside pico::live_fixture after State; dedicated Linux fixture only.
inline LedModel fixture_model(bool corrupt=false) {
    LedModel model{};
    for (size_t i = 0; i < 8; ++i) {
        model[3 * i] = (double(i % 4) - 1.5) * .02;
        model[3 * i + 1] = (double(i / 4) - .5) * .02;
    }
    std::vector<uint8_t> bytes(660);bytes[204]=corrupt?2:1;bytes[208]=60;
    for(size_t i=0;i<24;++i){float value=float(model[i]);std::memcpy(bytes.data()+212+4*i,&value,4);}
    const std::string alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string encoded;
    for(size_t i=0;i<bytes.size();i+=3){const uint32_t word=(uint32_t(bytes[i])<<16)|(uint32_t(bytes[i+1])<<8)|bytes[i+2];
        for(int shift=18;shift>=0;shift-=6)encoded+=alphabet[(word>>shift)&63];}
    phase5::Value root;root.type='{';phase5::Value row;row.type='{';
    row.object["sn"]=string_value("PCTESTTRACKER0001");row.object["calibration"]=string_value(encoded);
    root.object["calibration_data"].type='[';root.object["calibration_data"].array.push_back(row);
    return load_replay_leds(root,"PCTESTTRACKER0001");
}
inline IdentityView fixture_view(const std::string& serial,bool corrupt=false) {
    const std::string axes=R"({"plus_x":[1,0,0],"plus_z":[0,0,1],"position":[0,0,0]})";
    std::string factory=R"({"cameras":[)",active=R"({"cameras":[)";
    for(int side=0;side<2;++side){
        if(side){factory+=",";active+=",";}
        const std::string name=side?"right":"left",sensor=side?"CAMUPPERRIGHT":"CAMUPPERLEFT";
        auto f=phase5::parse(R"({"intrinsics":[{"cameraModel":"kb","cx":528,"cy":512}]})");
        f.object["serial"]=string_value(sensor);f.object["sourceCamera"]=string_value(name);
        factory+=phase5::encode(f);
        auto a=phase5::parse(R"({"cameraModel":"kb","width":1056,"height":1024,"intrinsics":{"fx":200,"fy":200,"cx":528,"cy":512,"k1":0,"k2":0,"k3":0,"k4":0}})");
        a.object["sourceCamera"]=string_value(name);a.object["extrinsics"]=phase5::parse(axes);
        a.object["extrinsics"].object["__units"]=string_value(corrupt?"unknown":"mm");
        active+=phase5::encode(a);
    }
    auto device=phase5::parse("{}");device.object["head"]=phase5::parse(axes);
    device.object["cv"].type='{';device.object["cv"].object["cad_from_cal"]=phase5::parse(axes);
    ReplayRig rig{phase5::parse(factory+"]}"),phase5::parse(active+"]}"),device};
    return rig.bind(serial,1056,1024);
}
inline int fixture_project(const OpticalCamera* camera, const double* points, size_t count, double* pixels) {
    for (size_t i = 0; i < count; ++i) {
        if (points[3 * i + 2] <= 0) return 1;
        pixels[2 * i] = camera->cx + camera->fx * points[3 * i] / points[3 * i + 2];
        pixels[2 * i + 1] = camera->cy + camera->fy * points[3 * i + 1] / points[3 * i + 2];
    }
    return 0;
}
inline int fixture_detect(const uint8_t* image, int height, int width, int, int, int, double* out, int capacity) {
    need(height == 1024 && width == 1056 && capacity >= 4, "Fixture detector dimensions");
    if (!image[0]) return 0;
    for (int i = 0; i < 4; ++i) {
        out[5 * i] = (image[0]==254?1020:520) + i * 5;
        out[5 * i + 1] = 500 + i * 3;
        out[5 * i + 2] = 1;
        out[5 * i + 3] = 255;
        out[5 * i + 4] = 2;
    }
    return 4;
}
inline int fixture_solve(const double*, const OpticalView* views, const double*, OpticalFit* fit) {
    need(views[0].count == 4 && views[1].count == 4, "Fixture stereo points");
    auto pose = identity_rigid();
    pose[11] = 1;
    std::copy(pose.begin(), pose.end(), fit->pose);
    fit->count = 4;
    fit->residual = 0;
    fit->position_std = views[0].pixels[0]>1000 ? .04 : 0;
    fit->rotation_std_deg = 0;
    fit->condition = 0;
    return 0;
}
inline NativeOpticalApi fixture_api() {
    return {fixture_project, fixture_solve, fixture_detect};
}
class RadioOptical {
    std::shared_ptr<State> state_;
    LiveSessionOwner& owner_;
    LiveSessionSafety& safety_;
    QueryPeer& peer_;
    OpticalPhaseVerification& phase_;
    DebugMemory memory_;
    OpticalCameraClock camera_{{1, "fixture-boot", "CLOCK_MONOTONIC", "synthetic", true, 0, 0}, "fixture-boot"};
    OpticalExposureSchedule schedule_;
    OpticalWorker* worker_ = nullptr;
    uint32_t sequence_ = 0;
    unsigned timers_ = 0, orders_ = 0;
    int64_t clock_origin_ = 0;
public:
    RadioOptical(std::shared_ptr<State> state, LiveSessionOwner& owner, LiveSessionSafety& safety,
                 QueryPeer& peer, OpticalPhaseVerification& phase)
        : state_(std::move(state)), owner_(owner), safety_(safety), peer_(peer), phase_(phase),
          memory_([this](const std::string& command) { return owner_.debugger_command(command); }),
          schedule_(camera_, owner.radio_clock(), 261700), clock_origin_(state_->now-6000000000LL) {
        for(int i=0;i<=120;++i) {
            const auto t=clock_origin_+int64_t(i)*50000000;
            const auto c=nearest_even(double(t-clock_origin_)/15.625);
            // A self-consistent MONOTONIC fit must not conceal a bad measured
            // crystal/RAW relationship. Only the private fixture perturbs RAW.
            const auto raw=t+(state_->fault=="radio-clock-raw" ? (t-clock_origin_)/1000 : 0);
            owner_.radio_clock().observe(c,t,t,raw,raw);
        }
        need(bool(owner_.radio_clock().fit),"Synthetic RAW probe fit failed");
    }
    uint32_t cycles() const {
        return uint32_t(nearest_even(double(state_->now - clock_origin_) / 15.625));
    }
    static Bytes packet(const Bytes& body) {
        Bytes result{0, 1, static_cast<uint8_t>(body.size())};
        result.insert(result.end(), body.begin(), body.end());
        unsigned sum = 0;
        for (auto byte : result) sum += byte;
        result.push_back(static_cast<uint8_t>(sum));
        return result;
    }
    void time(int64_t stamp) {
        need(stamp >= state_->now, "Fixture clock reversal");
        state_->now = stamp;
        need(owner_.radio_clock().observe(cycles(),stamp,stamp,stamp,stamp),"Synthetic RAW probe rejected");
    }
    // Only DebugMemory/TclSocket writes touch the fake MCU. The socket server
    // remains the sole accessor of its memory map; no cross-thread map writes.
    void ring(const Bytes& payload, uint32_t generation = 0) {
        const auto header = memory_.read(roy_data, 120);
        const auto write = word(header, 112);
        need(write == word(header, 116), "Fixture ring not drained");
        Bytes bytes(112);
        auto set = [&](size_t offset, uint32_t value) {
            const auto encoded = le32(value);
            std::copy(encoded.begin(), encoded.end(), bytes.begin() + offset);
        };
        set(0, ++sequence_);
        set(4, generation << 16);
        set(12, 2);
        set(16, uint32_t(payload.size() + 2) | ((sequence_ % 30000) << 17));
        bytes[20] = static_cast<uint8_t>(payload.size());
        bytes[21] = 2;
        std::copy(payload.begin(), payload.end(), bytes.begin() + 22);
        set(100, 0xA1B2C3D4);
        // A received packet precedes its host dispatch. Keep it behind now so
        // cycle quantization cannot turn the synthetic source into future data.
        set(104, owner_.radio_clock().cycles_at(state_->now - 1000));
        // Keep product discovery valid; corrupt subsequent canonical evidence
        // at the real ring boundary, before parse_record/DataPlane admission.
        if (sequence_ > 1) {
            if (state_->fault == "radio-source-chip") set(100, 0xA1B2C3D5);
            if (state_->fault == "radio-source-channel") set(12, 3);
            if (state_->fault == "radio-partial-crc") set(16, uint32_t(payload.size() + 2) | 0x10000 | ((sequence_ % 30000) << 17));
            if (state_->fault == "radio-payload-checksum") bytes[21 + payload.size()] ^= 1;
        }
        memory_.write(roy_data + 380 + (write % 10) * 112, bytes);
        memory_.write(roy_data + 112, le32(write + 1));
        memory_.write(roy_data + 1504 + 12, le32(3));
        owner_.radio_step();
        need(word(memory_.read(roy_data + 116, 4)) == write + 1, "Canonical ring not consumed");
    }
    Bytes reply(unsigned schema, const Bytes& nonce) {
        Bytes result{0xfe, static_cast<uint8_t>(schema)};
        result.insert(result.end(), nonce.begin(), nonce.end());
        const auto chip = le32(0xA1B2C3D4);
        result.insert(result.end(), chip.begin(), chip.end());
        result.resize(schema == 2 ? 26 : schema == 4 ? 25 : 32);
        auto set = [&](size_t offset, uint32_t value) {
            const auto bytes = le32(value);
            std::copy(bytes.begin(), bytes.end(), result.begin() + offset);
        };
        if (schema == 2) {
            result[22] = 1;
            result[23] = 7;
            result[24] = peer_.active_query() ? 1 : 2;
            if (peer_.active_query() && state_->fault == "radio-native-n") result[24] = 3;
            if (peer_.active_query() && state_->fault == "radio-native-pending") {
                result[24] = 2;
                result[25] = 1;
            }
        }
        if (schema == 3) {
            result[22] = 7;
            result[24] = 1;
            result[26] = static_cast<uint8_t>(sequence_);
            result[29] = 1;
            if (peer_.diagnostic_index >= 22 && state_->fault == "optical-producer-disabled") result[24] = 0;
        }
        if (schema == 4) result[22] = result[23] = result[24] = 1;
        if (schema == 6) {
            result[23] = 3;
            result[25] = static_cast<uint8_t>(timers_++ % 2);
            set(26, 500 + timers_);
            result[30] = 4;
            result[31] = 2;
        }
        if (schema == 7) {
            set(22, 100);
            set(26, 533333);
        }
        if (schema == 8) {
            set(22, 0x40001104);
            set(26, 0x40009048);
            result[30] = 8;
        }
        if (schema == 9) {
            set(22, 10 + 10 * orders_++);
            set(26, 3);
        }
        if (schema == 10) {
            set(22, 2);
            set(26, 1);
        }
        if (schema == 11) {
            set(22, 4);
            set(26, 5);
            result[31] = 1;
        }
        return result;
    }
    // Answer the session's current request, never a wall-clock reply script.
    // Even without an application query a tracker returns canonical link
    // traffic. Native25 tags remain explicitly acknowledged by episode().
    void poll() {
        Bytes payload{0, 0, 0, 0};
        uint32_t generation = 0;
        if (peer_.pending) {
            const auto pending = *peer_.pending;
            if (pending.command != 25 || !peer_.native25_exclusive) generation = pending.generation;
            if (pending.command == 1 && pending.body.size() == 18 && pending.body[0] == 0x7e) {
                const bool missing = peer_.mode_nonce &&
                    (state_->fault == "radio-recovery-exhaust" ||
                     (state_->fault == "radio-recovery" &&
                      (!peer_.initial_recovery || peer_.initial_recovery->attempts < 3)));
                if (!missing) {
                    auto nonce = Bytes(pending.body.begin() + 2, pending.body.end());
                    if (state_->fault == "radio-wrong-nonce") nonce[0] ^= 1;
                    payload = packet(reply(pending.body[1], nonce));
                }
            }
        }
        // Do not replace a requested diagnostic page with a sample.
        if (payload == Bytes{0, 0, 0, 0} && peer_.sample_feedback_enabled) {
            payload = {0, 2, 18};
            payload.resize(21);
            payload[20] = static_cast<uint8_t>(sequence_);
            unsigned sum = 0;
            for (auto byte : payload) sum += byte;
            payload.push_back(static_cast<uint8_t>(sum));
        }
        ring(payload, generation);
    }
    void startup() {
        state_->event("radio-startup");
        Bytes product(35);
        product[0] = 0x82;
        product[1] = 10;
        const std::string serial = "PCTESTTRACKER0001";
        std::copy(serial.begin(), serial.end(), product.begin() + 8);
        ring(packet(product));
        for (unsigned i = 0; i < 700 && !(peer_.diagnostic_index == 22 && !peer_.pending); ++i) {
            time(state_->now + 100000000);
            if (state_->fault == "radio-heartbeat") {
                memory_.write(roy_data + 4, le32(1));
                owner_.radio_step();
                throw std::runtime_error("Expired MCU heartbeat accepted");
            }
            if (state_->fault == "radio-ring-loss") {
                const auto read = word(memory_.read(roy_data + 116, 4));
                memory_.write(roy_data + 112, le32(read + 11));
                owner_.radio_step();
                throw std::runtime_error("Ring overflow accepted");
            }
            poll();
        }
        need(peer_.diagnostic_index == 22 && !peer_.pending && peer_.native_ir31_applied &&
             peer_.continuity_lease, "Fixture startup did not reach measured prerequisites");
        state_->event("query-prerequisites-observed");
        if (state_->fault == "radio-recovery") {
            need(peer_.initial_recovery && peer_.initial_recovery->completed &&
                 peer_.initial_recovery->attempts == 3, "Three-attempt recovery not observed");
            state_->event("three-attempt-recovery-observed");
        }
    }
    void frame(int index, int64_t start, bool dark = false, bool radio = true, bool causal_dark = false) {
        const auto raw = double(start + int64_t(index) * 33333333) / 1e9;
        const auto stamp = nearest_even(raw * 1e9);
        time(stamp + 12000000);
        safety_.fresh_tracking();
        if (radio) poll();
        IdentityPair pair;
        pair.timestamp_matched = true;
        for (size_t i = 0; i < 2; ++i) {
            auto& frame = pair.frames[i];
            frame.serial = i ? "CAMUPPERRIGHT" : "CAMUPPERLEFT";
            frame.metadata = "fixture-exposure-" + std::to_string(stamp) + "-" + std::to_string(i);
            frame.timestamp_matched = true;
            frame.stamp = stamp;
            frame.exposure_us = 800;
            frame.world_from_head=safety_.head_pose(stamp);
            (void)safety_.driver_origin(stamp);
            frame.view=fixture_view(frame.serial,state_->fault=="optical-factory-rig");
            frame.pixels.resize(1056u * 1024u);
            frame.pixels[0] = dark ? 0 : state_->fault=="optical-covariance"?254:255;
        }
        // A missing detection in phase verification counts against 48-of-61.
        // Only a deliberate native25 OFF frame is excluded from that window.
        std::array<CameraMetadata, 2> metadata{CameraMetadata{raw, .0008}, CameraMetadata{raw, .0008}};
        // Inject before the first OFF: rejection must leave no LED application
        // outstanding, so ordinary supervisor restoration can be verified.
        if (state_->fault == "optical-skew") metadata[1].raw += .000003;
        if (state_->fault == "optical-exposure") metadata[0].exposure = .001;
        if (state_->fault == "optical-source-age") time(stamp + 250000001);
        if (state_->fault == "optical-image-size") pair.frames[0].pixels.pop_back();
        if (state_->fault == "optical-sensor") pair.frames[0].serial = "WRONG-SENSOR";
        if (state_->fault == "optical-metadata") ++pair.frames[0].exposure_us;
        const bool published=worker_->exposure(std::move(pair), metadata, causal_dark);
        if(state_->fault=="optical-covariance") {
            need(!published,"Covariance fault published a pose");
            throw std::runtime_error("Covariance candidate rejected");
        }
    }
    void episode(int64_t start) {
        // Poll before collecting the three pre-OFF frames. During the causal
        // bracket the explicit OFF/ON polls below own transport advancement;
        // an extra tick can retire ON and start the next episode prematurely.
        for (int i = 0; i < 3; ++i) frame(i, start, false, i == 0);
        time(start + 80000000);
        ring({0, 0, 0, 0});
        need(peer_.pending && peer_.pending->command == 25 && peer_.pending->body == Bytes{1}, "OFF not allocated");
        // Source minus clock uncertainty must follow mailbox publication.
        time(start + 81000000);
        ring({0, 0, 0, 0}, peer_.pending->generation);
        need(owner_.led_restore_required(), "OFF did not retain restoration ledger");
        for (int i = 3; i <= 9; ++i) frame(i, start, i >= 4, false, i >= 4);
        time(start + 330000000);
        ring({0, 0, 0, 0});
        need(peer_.pending && peer_.pending->command == 25 && peer_.pending->body == Bytes{2}, "ON not allocated");
        time(start + 331000000);
        ring({0, 0, 0, 0}, peer_.pending->generation);
        if(state_->fault=="optical-led-restore") {
            bool refused=false;
            try {owner_.identity().owner.optical_return(peer_,"live",false);}
            catch(const std::exception&){refused=true;}
            need(refused && owner_.led_restore_required() && peer_.native25_exclusive,
                 "Unmeasured LED return released exclusive owner");
            state_->event("unmeasured-led-return-refused");
        }
        for (int i = 10; i <= 14; ++i) frame(i, start, i == 10, false, i == 10);
        need(owner_.led_restore_required(), "Optical thread retired LED owner");
        time(state_->now + 1);
        owner_.identity().radio_tick(peer_, "live", state_->now);
        need(!owner_.led_restore_required() && !peer_.native25_exclusive, "Measured ON return did not retire LED owner");
        state_->event("native25-episode-observed");
    }
    void run(const std::shared_ptr<RadioOptical>& context) {
        worker_ = &owner_.start_worker(fixture_model(), fixture_api(), camera_, schedule_, phase_,
            [state = state_] { return state->now; }, owner_.udp_pose_sink(), context);
        OwnerIO probe;
        probe.now = [state = state_] { return state->now; };
        probe.read = [this](uint32_t, size_t count) {
            need(count == 4, "Fixture DWT read width");
            return le32(cycles());
        };
        owner_.start_clock_poll(probe, owner_.radio_clock(), probe.now, [](const ClockProbeEvent&) {});
        time(state_->now);
        startup();
        if (state_->fault.rfind("radio-", 0) == 0) return;
        if(state_->fault=="optical-clock-horizon") (void)owner_.radio_clock().cycles_at(state_->now+250000001);
        phase_.ack(0xA1B2C3D4, 261700);
        const auto start = state_->now + 10000000;
        episode(start);
        if(state_->fault=="optical-led-restore")throw std::runtime_error("Unmeasured LED return refused; measured recovery completed");
        need(!owner_.identity().bindings_snapshot(), "One episode granted identity");
        episode(start + 1000000000);
        need(owner_.identity().bindings_snapshot(), "Two episodes failed identity");
        state_->event("two-episode-identity-observed");
        // A canonical sample marks the radio peer streaming; absent command19,
        // LiveImuRouting must still suppress every PTI1 datagram.
        Bytes sample{0, 2, 18};
        sample.resize(21);
        unsigned checksum = 0;
        for (auto byte : sample) checksum += byte;
        sample.push_back(static_cast<uint8_t>(checksum));
        ring(sample);
        // Keep actual canonical link evidence continuous across the deliberate
        // frame-index gap; a time jump must not bypass QueryPeer's watchdog.
        while (state_->now + 1000000000 < start + 900 * 33333333LL) {
            time(state_->now + 1000000000);
            poll();
        }
        for (int i = 900; i <= 960; ++i) {
            frame(i, start, state_->fault == "optical-phase-exhausted" && i < 914);
            safety_.guard();
        }
        need(phase_.state() == IdentityPhaseWindow::State::Locked, "48-of-61 phase not locked");
        state_->event("phase-lock-observed");
        frame(961, start);
        state_->event("optical-publication-complete");
        // Keep fresh optics while LaterImuStart earns its three-second link
        // stability and two measured producer pages. No peer/gate state writes.
        for (int i = 962; i < 1162; ++i) {
            frame(i, start);
        }
        need(peer_.diagnostic_index == 26 && peer_.sample_feedback_enabled, "Command19 admission not observed");
        state_->event("command19-admission-observed");
        if(state_->fault=="optical-publication-expiry") {
            time(state_->now+250000001);
            owner_.publication_poll(state_->now,true,true);
            state_->event("expired-publication-invalidated");
            throw std::runtime_error("Publication expired; invalid status emitted");
        }
    }
};
