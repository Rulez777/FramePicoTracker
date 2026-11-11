#pragma once
#include "device_config.hpp"
#include "phase5/orchestrator.hpp"
#include "openocd_process.hpp"
#include "live_session.hpp"
#include "radio_loop.hpp"
#include "query_peer.hpp"
#include "station_control.hpp"
#include "native25.hpp"
#include "native_identity.hpp"
#include "camera_preflight.hpp"
#include "optical_worker.hpp"
#include "live_policy.hpp"
#include "driver_origin.hpp"
#include "observer_process.hpp"
#include "durable_supervisor.hpp"
#include "live_startup.hpp"
#include "live_imu_routing.hpp"
#include "live_phase_routing.hpp"
#include "multi_peer_session.hpp"
#include "multi_led_schedule.hpp"
#include <memory>
#include <cstdlib>
#include "session_state_guard.hpp"
#include <utility>

namespace pico {
// Ordered components of the unified live/replay session. Files/Runtime are
// values so the profile's references never outlive caller-local adapter wrappers.
// Real OsAdapters callbacks retain their shared Linux owner; replay supplies the
// same interfaces. Worker/expiry and radio updates share one recursive mutex. Real
// adapters and durable supervision still block production live admission.
class LiveSessionOwner {
    mutable std::recursive_mutex shared_;
    std::function<void()> optics_stop_;
    phase5::Files files_;
    phase5::Runtime runtime_;
    phase5::ProfileOwner profile_;
    // Retain adapter state before dependent objects, including failed starts.
    std::shared_ptr<void> debugger_context_;
    std::unique_ptr<OpenOcdProcess> debugger_;
    std::unique_ptr<SramOwner> sram_;
    SramOwner* transaction_=nullptr;
#ifndef _WIN32
    SupervisedRoyOwner* supervisor_=nullptr;
#endif
    std::function<std::string(const std::string&)> supervised_command_;
    std::unique_ptr<DataPlane> plane_;
    std::unique_ptr<RadioLoop> radio_;
    std::unique_ptr<DebugMemory> query_debug_;
    std::unique_ptr<NativeIdentitySession> identity_;
    std::unique_ptr<QueryPeer> peer_;
    std::unique_ptr<Native25AckBridge> bridge_;
    std::unique_ptr<StationControl> station_;
    // multi4 (PICO_PEERS): peers 1..3, radio/IMU only; destroyed before the debugger memory and plane they use.
    const PeerTable& table_=PeerTable::configured();
    SecondaryPeers secondaries_;
    std::unique_ptr<LivePhaseRouting> phase_routing_;
    std::optional<OpticalPoseFrame> measured_pose_;
    LiveImuRouting imu_routing_;
    std::optional<int64_t> disconnected_since_;  // start of a tolerated query link loss
    bool live_imu_routing_=false;
    std::shared_ptr<camera::LiveCameraSource> camera_;
    std::unique_ptr<CameraAcquisition> camera_preflight_;
    std::shared_ptr<void> optical_context_;
    // Multi-tracker optics (PICO_PEERS with secondaries, PICO_MULTI_OPTICS unset or not 0): declared before the
    // worker that borrows it; the schedule borrows the secondaries and the optics.
    bool multi_optics_enabled_=false;
    std::unique_ptr<MultiChipOptics> multi_optics_;
    std::unique_ptr<MultiLedSchedule> led_schedule_;
    std::function<bool(const OpticalPoseFrame&,int64_t)> pose_send_;
    std::unique_ptr<OpticalWorker> worker_;
    std::unique_ptr<RoyClockPoll> clock_poll_;
    RoyClock* query_clock_=nullptr;
    std::unique_ptr<PublicationExpiry> publication_;
    std::unique_ptr<TrackingObserver> observer_;
#ifndef _WIN32
    std::shared_ptr<ObserverProcess> observer_process_;
#endif
    std::shared_ptr<UdpSender> imu_publisher_;
    std::shared_ptr<OpticalPosePublisher> pose_publisher_;
    uint64_t optical_epoch_=0;
    bool direct_camera_=false;
    bool live_exposure_active_=false,publication_failed_=false;
    bool led_recovery_only_=false;
    std::function<bool()> clock_ready_;
    bool worker_attempted_=false, worker_stopped_=false, worker_stopping_=false;
    bool clock_attempted_=false;
    std::function<int64_t()> camera_now_;
    std::function<std::string()> camera_boot_;
    std::function<CameraReadClocks()> camera_read_;
    bool camera_attempted_=false;
    bool peer_attempted_=false;
    bool debugger_started_=false,components_closed_=false;
    bool recovering_=false,observer_closed_logged_=false;
    bool observer_polling_=false;
    bool identity_attempted_=false;
    bool debugger_attempted_=false,radio_attempted_=false,entered_=false;
    LiveSessionSafety* safety_=nullptr;
public:
    // Borrowed references require this guard across their entire use.
    SessionStateGuard shared_state() const { return SessionStateGuard(shared_); }
    void optics_stop(std::function<void()> stop) {
        SessionStateGuard lock(shared_);optics_stop_=std::move(stop);
    }
    LiveSessionOwner(phase5::Files files,phase5::Runtime runtime,phase5::BoundInvocation invocation)
        :files_(std::move(files)),runtime_(std::move(runtime)),profile_(files_,runtime_,std::move(invocation)){}
    LiveSessionOwner(const LiveSessionOwner&)=delete;
    LiveSessionOwner& operator=(const LiveSessionOwner&)=delete;
    LiveSessionOwner(LiveSessionOwner&&)=delete;
    LiveSessionOwner& operator=(LiveSessionOwner&&)=delete;
    ~LiveSessionOwner(){
        // Join dispatch only; destruction never restores or releases Roy SRAM.
        try{stop_worker();}catch(const std::exception& e){std::cerr<<e.what()<<"; retaining SRAM ownership\n";}
    }
    void apply(){profile_.apply();}
    void start_observer(ObserverHooks hooks){
        if(observer_||safety_||components_closed_)throw std::runtime_error("Session observer dependency/order violation");
        auto read=hooks.read;
        if(read)hooks.read=[this,read](size_t n){
            SessionStateGuard::UnlockedIO unlocked(shared_);
            return read(n); // pipe read and proof-file flush, no session state
        };
        observer_=std::make_unique<TrackingObserver>(std::move(hooks));
    }
    void observer_poll(phase5::BenchTrackingGuard& guard){
        SessionStateGuard lock(shared_);
        // Runtime states log and continue: the read-only observer keeps reporting during
        // restoration; after close there is nothing to read. A missing observer before
        // close is a code-order violation and still throws.
        if(components_closed_){
            if(!observer_closed_logged_)std::cerr<<"Session observer closed; no tracking rows polled\n";
            observer_closed_logged_=true;return;
        }
        if(!observer_)throw std::runtime_error("Session observer unavailable");
        // A pipe read can release shared_. Preserve parser ordering if the other
        // thread polls during that read; it uses the last ingested history.
        if(observer_polling_)return;
        observer_polling_=true;
        struct Reset { bool& busy; ~Reset(){busy=false;} } reset{observer_polling_};
        observer_->poll(guard);
    }
    bool observer_alive() const {
        SessionStateGuard lock(shared_);
        return observer_&&!components_closed_&&observer_->alive();
    }
#ifndef _WIN32
    void launch_observer(const std::filesystem::path& binary,const std::filesystem::path& proof,
        const std::string& reviewed_sha,std::function<bool()> cancelled,bool controller_guard=false){
        if(observer_||observer_process_||safety_||components_closed_)
            throw std::runtime_error("Session observer launch dependency/order violation");
        observer_process_=std::make_shared<ObserverProcess>(binary,proof,reviewed_sha,controller_guard);
        start_observer(observer_process_->hooks(std::move(cancelled)));
    }
#endif
    void verify_active(){profile_.verify_active();}
    void own_camera(std::function<void()> close){profile_.own_camera(std::move(close));}
    void launch_begin(){profile_.launch_begin();}
    void attach_child(phase5::Child& child){profile_.attach_child(child);}
    void start_debugger(OpenOcdHooks hooks,std::shared_ptr<void> context){
        if(debugger_attempted_||components_closed_||!context)throw std::runtime_error("Session debugger transaction consumed or context missing");
        debugger_attempted_=true;
        debugger_context_=std::move(context);
        debugger_=std::make_unique<OpenOcdProcess>(std::move(hooks));
        debugger_->start();debugger_started_=true;
    }
    std::string debugger_command(const std::string& command){
        SessionStateGuard::UnlockedIO unlocked(shared_);
        if(!debugger_started_||components_closed_)throw std::runtime_error("Session debugger unavailable");
        if(supervised_command_)return supervised_command_(command);
    return debugger_->command(command);
    }
#ifndef _WIN32
    // Borrow the one durable transaction; never construct a second SRAM owner.
    // Called in the detached child after profile/debugger construction.
    void bind_supervisor(SupervisedRoyOwner& supervisor,OwnerIO io){
        if(debugger_attempted_||sram_||transaction_||components_closed_)
            throw std::runtime_error("Session supervisor binding consumed");
        debugger_attempted_=true;
        supervisor_=&supervisor;
        supervised_command_=[&supervisor](const std::string& text){return supervisor.command(text);};
        transaction_=&supervisor.prepare_sram(std::move(io));
        debugger_started_=true;
    }
#endif
    SramOwner& prepare_sram(OwnerIO io,std::function<void(const Checkpoint&)> persist){
        if(!debugger_started_||transaction_||components_closed_||!io.write)throw std::runtime_error("Session SRAM dependency/order violation");
        auto write=io.write;
        io.write=[this,write](uint32_t address,const Bytes& bytes){debugger_->before_sram_mutation();write(address,bytes);};
        sram_=std::make_unique<SramOwner>(std::move(io),std::move(persist));
        transaction_=sram_.get();
        return *sram_;
    }
    void install_sram(LiveSessionSafety& safety,const Bytes& stock,const Bytes& elf,const Bytes& state){
        if(!transaction_||safety_||recovering_)throw std::runtime_error("Session SRAM install consumed or missing");
        table_.require_stock_timing(); // configuration refusal before any SRAM installation/entry
        safety_=&safety;
        safety.install_sram(*transaction_,stock,elf,state);
    }
    void enter_sram(){
        if(!safety_||entered_||recovering_)throw std::runtime_error("Session SRAM entry consumed or missing");
        safety_->enter_sram();entered_=true;
    }
    CleanupBarriers restore_sram(LiveSessionSafety& safety){
        stop_worker();
        SessionStateGuard lock(shared_);
        if(supervised_command_)throw std::runtime_error("Only supervisor may restore/report its transaction");
        if(!sram_||safety_!=&safety)throw std::runtime_error("Session SRAM not owned by this safety envelope");
        recovering_=true;
        // No decode/solve/publication may outlive the first restoration command.
        // A reentrant stop throws before identity invalidation or MCU access,
        // retaining the debugger/profile for an explicit recovery retry.
        if(identity_)identity_->invalidate();
        auto barriers=safety.restore_sram();
        if(barriers.complete())debugger_->restored(barriers.session_closed_verified,barriers.ram_restored_verified,
                            barriers.mpu_restored,barriers.trace_controls_restored_verified);
        else std::cerr<<"SRAM restoration barriers unresolved; debugger owner retained\n";
        return barriers;
    }
    DataPlane& start_radio(OwnerIO io,Hooks plane_hooks,LoopHooks loop_hooks){
        if(!transaction_||!entered_||!transaction_->unresolved()||radio_attempted_||recovering_||components_closed_||!loop_hooks.poll)
            throw std::runtime_error("Session radio dependency/order violation");
        radio_attempted_=true;
        auto read=io.read; auto write=io.write; auto sleep=io.sleep;
        if(read)io.read=[this,read](uint32_t a,size_t n){SessionStateGuard::UnlockedIO unlocked(shared_);return read(a,n);};
        if(write)io.write=[this,write](uint32_t a,const Bytes& b){SessionStateGuard::UnlockedIO unlocked(shared_);write(a,b);};
        if(sleep)io.sleep=[this,sleep](int64_t ns){SessionStateGuard::UnlockedIO unlocked(shared_);sleep(ns);};
        auto canonical=plane_hooks.canonical;
        plane_hooks.canonical=[this,canonical](const Record& r,const std::vector<Command>& c,const std::optional<AckContext>& a){
            if(station_) {
                query_record(r);
                if(live_imu_routing_)imu_routing_.canonical(*plane_,*peer_,c);
            }
            if(canonical)canonical(r,c,a);
        };
        auto poll=loop_hooks.poll;
        loop_hooks.poll=[this,poll](const Bytes& header){poll(header);if(station_)query_poll();
        if(secondaries_.size())secondary_poll();};
        if(table_.multi)plane_hooks.divert=[this](const Record& r,const Stamps& s){return secondaries_.divert(r,s);};
        plane_=std::make_unique<DataPlane>(std::move(plane_hooks));
        if(table_.multi)plane_->peer_chips=table_.chips();
        radio_=std::make_unique<RadioLoop>(std::move(io),*plane_,std::move(loop_hooks));return *plane_;
    }
    NativeIdentitySession& prepare_identity(LedModel model,NativeOpticalApi api){
        if(!radio_||peer_attempted_||identity_attempted_||recovering_||components_closed_)
            throw std::runtime_error("Session identity dependency/order violation");
        identity_attempted_=true;
        try{identity_=std::make_unique<NativeIdentitySession>(pico::devices::chip(),std::move(model),"live",api);return *identity_;}
        catch(...){recovering_=true;throw;}
    }
    NativeIdentitySession& identity(){
        SessionStateGuard lock(shared_);
        if(!identity_||recovering_||components_closed_)throw std::runtime_error("Session identity unavailable");
    return *identity_;
    }
    QueryPeer& start_query(RoyClock& clock,
        std::function<double()> now,std::function<Bytes()> nonce,QueryHooks hooks,
        std::function<void(const std::string&,unsigned,uint8_t,const Bytes&)> recovery,
        std::function<int64_t()> source_now){
        if(!radio_||!identity_||peer_attempted_||recovering_||components_closed_||!recovery||!hooks.fresh_guard)
            throw std::runtime_error("Session query dependency/order violation");
        peer_attempted_=true;
        query_clock_=&clock;
        try{
        query_debug_=std::make_unique<DebugMemory>([this](const std::string& command){return debugger_command(command);});
        // Guard remains the same owner-thread envelope, including command19.
        auto fresh=hooks.fresh_guard;
        hooks.fresh_guard=[this,fresh]{safety_->fresh_tracking();fresh();};
        hooks.native_radio_tick=[this,source_now]{
            safety_->fresh_tracking();
            if (peer_->stock_mode_requested) return false;
            if (led_recovery_only_) {
                identity_->recovery_tick(*peer_,"live",source_now());
                plane_->exclusive = true;
                return true; // no unrelated command after the final ON return
            }
            const bool exclusive=identity_->drive_radio(*peer_,"live",source_now());
            plane_->exclusive=exclusive;
            return exclusive;
        };
        auto action=hooks.action;
        if(!action)throw std::runtime_error("Session query action evidence absent");
        hooks.action=[this,action](uint8_t command,const Bytes& body){
            action(command,body);
            imu_routing_.action(command,body);
        };
        auto secondary_now=now;auto secondary_nonce=nonce;
        peer_=std::make_unique<QueryPeer>(pico::devices::chip_le(),0,std::move(now),std::move(nonce),std::move(hooks));
        peer_->breadcrumb_capable=table_.peers[0].profile=="release-c2";
        if(!table_.multi){
            configure_dual_mode(*peer_,1,2,true);
            // Single path unchanged unless explicitly asked for (same release-plan restart as multi).
            const char* restart=std::getenv("PICO_RESTART_PLAN");
            peer_->restart_plan_after_reconnect=restart&&std::string(restart)=="1";
        }
        else{  // multi4: every tracker polled every K x 6000 us, Frame N = 2K-1 written and read back
            configure_frame_peer(*peer_,int(table_.tracker_n()),table_.poll_period_us(),true);
            peer_->bootstrap_n=uint8_t(table_.tracker_n());
            peer_->n_readback=n_readback_logger(first_target_chip);
            peer_->restart_plan_after_reconnect=true;  // IMU must come back after a body-worn link loss
        }
        peer_->recovery_event=std::move(recovery);
        bridge_=std::make_unique<Native25AckBridge>(identity_->owner,*peer_,clock,"live",source_now);
        StationControlHooks sh;
        sh.optics_poll=[this,source_now]{
            if(!peer_->stock_mode_requested && phase_routing_ && !led_recovery_only_)phase_routing_->poll(*peer_,source_now());
        };
        sh.reset_optics=[this](uint32_t){
            if(phase_routing_)phase_routing_->reset();
            measured_pose_.reset();
            imu_routing_.reset();
            ++optical_epoch_;
            identity_->restart();  // was invalidate(): that ended identity for the whole session
            if(identity_->owner.restore_required())std::cerr<<"Query link lost with native restoration unresolved; continuing\n";};
        sh.mailbox=[this,source_now](int slot,PeerEngine& e,MailboxWriter& w){
            if(peer_->stock_mode_requested || !phase_routing_){bridge_->publish(w,slot,e.mailbox());return;}
            phase_routing_->mailbox(*peer_,source_now(),[&](const Mailbox& box,
                std::optional<PhaseReference> reference,std::optional<CycleWindow> window){
                return bridge_->publish(w,slot,box,reference,window);
            });
        };
        sh.acknowledge=[this,source_now](int,std::optional<uint8_t> command,uint32_t cycles,
                                       std::optional<uint16_t> generation){
            if(phase_routing_)phase_routing_->acknowledge(command,cycles,generation,source_now());
        };
        sh.advance=[](PeerEngine& e){e.advance();};
        sh.ack=[](PeerEngine& e,uint16_t tag){return e.ack_tag(tag);};
        sh.receive=[](PeerEngine& e,const Record& r){return e.receive(Bytes(r.dma.begin()+2,r.dma.end()),r.dma[1],uint16_t(r.generation),r.hardware_crc);};
        station_=std::make_unique<StationControl>(*query_debug_,*plane_,*peer_,std::move(sh));
        plane_->feedback_enabled=false;
        auto feedback=plane_->hooks.feedback;
        plane_->hooks.feedback=[this,feedback](uint8_t counter){
            station_->feedback(counter);
            if(feedback)feedback(counter);
        };
        auto mailbox=plane_->hooks.mailbox;
        plane_->hooks.mailbox=[this,mailbox]{
            station_->mailbox();
            if(mailbox)mailbox();
        };
        if(table_.multi){
            const char* optics=std::getenv("PICO_MULTI_OPTICS");
            multi_optics_enabled_=table_.enrolled()>1&&!(optics&&std::string(optics)=="0");
            secondaries_.start(table_,*query_debug_,*plane_,std::move(secondary_now),std::move(secondary_nonce),
                               multi_optics_enabled_);
            if(imu_publisher_) {secondaries_.set_send(secondary_send());secondaries_.set_sensor_send(plane_->hooks.sensor);}
        }
        return *peer_;
        }catch(...){identity_->invalidate();recovering_=true;throw;}
    }
    void query_poll(){
        SessionStateGuard lock(shared_);
        if(!station_||recovering_||components_closed_)throw std::runtime_error("Session query dispatch revoked");
        try{
            safety_->guard();
            // QueryPeer's application-proof wait must not hide the link watchdog.
            if(peer_->last_reply&&peer_->now()-*peer_->last_reply>=6)
                std::cerr<<"Query link watchdog expired at 6s; continuing\n";
            const auto previous=peer_->state;
            station_->poll();
            if(identity_ && identity_->owner.relight){  // LEDs back ON: light-sync must be re-sent
                identity_->owner.relight=false;
                ++optical_epoch_;
                if(phase_routing_)phase_routing_->reset();
            }
            // A tracker may briefly reconnect (seen after command 25 on hardware, 2026-10-29). Wait up to 5 s for
            // the link to come back instead of ending the session on the first Reconnecting/Absent poll.
            const bool lost=peer_->state==PeerState::Reconnecting||
                (peer_->state==PeerState::Absent&&(previous!=PeerState::Absent||disconnected_since_));
            if(!lost)disconnected_since_.reset();
            else{
                const auto now=nearest_even(peer_->now()*1e9);
                if(!disconnected_since_)disconnected_since_=now;
                if(now-*disconnected_since_>5000000000LL)
                    std::cerr<<"Query peer disconnected for more than 5 s; continuing\n";
            }
        }catch(const std::exception& e){log_live_runtime_error(e);}
    }
    // multi4 peers 1..3: same owner thread and lock as query_poll; a failing peer never stops the others.
    void secondary_poll(){
        SessionStateGuard lock(shared_);
        if(recovering_||components_closed_)return;
        try{safety_->guard();secondaries_.poll();if(led_schedule_)led_schedule_->poll();}
        catch(const std::exception& e){log_live_runtime_error(e);}
    }
    // PTI1 of a secondary chip: UDP only (no optical prior: the worker solves the first target).
    std::function<bool(const std::array<uint8_t,32>&)> secondary_send(){
        return [this](const std::array<uint8_t,32>& bytes){
            if(recovering_||components_closed_||worker_stopped_||!imu_publisher_)return false;
            if(multi_optics_)multi_optics_->prior_imu(bytes);
            if(!imu_publisher_->send(bytes)){std::cerr<<"Session PTI1 transport failed; dropping packet\n";return false;}
            return true;
        };
    }
    SecondaryPeers& secondary_peers(){SessionStateGuard lock(shared_);return secondaries_;}
    const Counters& radio_counters(){
        SessionStateGuard lock(shared_);
        if(!plane_||components_closed_)throw std::runtime_error("Session radio counters unavailable");
        return plane_->counters;
    }
    void query_record(const Record& r){
        SessionStateGuard lock(shared_);
        if(!station_||recovering_||components_closed_)throw std::runtime_error("Session query dispatch revoked");
        try{safety_->guard();
            if(peer_->last_reply&&peer_->now()-*peer_->last_reply>=6)
                std::cerr<<"Query link watchdog expired at 6s; continuing\n";
            bridge_->record(r,[&]{station_->canonical(r);});}
        catch(const std::exception& e){log_live_runtime_error(e);}
    }
    Native25AckBridge& query_bridge(){
        SessionStateGuard lock(shared_);if(!bridge_||recovering_||components_closed_)throw std::runtime_error("Session query bridge unavailable");
    return *bridge_;}
    void radio_step(){
        SessionStateGuard lock(shared_);
        if(!radio_||recovering_||components_closed_)throw std::runtime_error("Session radio dispatch revoked");
        try{
            if(publication_failed_)std::cerr<<"Session PTI1 transport failed; continuing\n";
            safety_->guard();
            if(clock_ready_)clock_poll(clock_ready_());
            radio_->step();
            if(publication_failed_)std::cerr<<"Session PTI1 transport failed; continuing\n";
        }catch(const std::exception& e){log_live_runtime_error(e);}
    }
    // Transport installation never enables feedback, enrolls a peer or sends19.
    // Canonical admission, exclusive mode, reorder and clock freshness stay in DataPlane.
    void start_udp_publishers(uint16_t destination_port = 39571){
        SessionStateGuard lock(shared_);
        if(!plane_||imu_publisher_||recovering_||components_closed_||worker_attempted_)
            throw std::runtime_error("Session publisher dependency/order violation");
        if(query_clock_!=&plane_->clock)throw std::runtime_error("PTI1 requires shared data-plane Roy clock");
        imu_publisher_=std::make_shared<UdpSender>(destination_port);
        pose_publisher_=std::make_shared<OpticalPosePublisher>(destination_port);
        plane_->hooks.send=[this](const std::array<uint8_t,32>& bytes){
            if(recovering_||components_closed_||worker_stopped_)return false;
            if(worker_)worker_->prior_imu(bytes);
            if(multi_optics_)multi_optics_->prior_imu(bytes);
            if(!imu_publisher_->send(bytes)){std::cerr<<"Session PTI1 transport failed; dropping packet\n";return false;}
            return true;
        };
        plane_->hooks.sensor=[this](const pico_sensors::Wire &w) {
            if(recovering_||components_closed_||worker_stopped_)return false;
            return imu_publisher_->send_sensor(w);
        };
        secondaries_.set_sensor_send(plane_->hooks.sensor);
        secondaries_.set_send(secondary_send());
    }
    void start_live_imu_routing(){
        SessionStateGuard lock(shared_);
        if(!station_||!imu_publisher_||live_imu_routing_||recovering_||components_closed_)
            throw std::runtime_error("Live IMU routing dependency/order violation");
        live_imu_routing_=true;
        plane_->enrolled=false;
        plane_->feedback_enabled=false;
    }
    RoyClock& radio_clock(){
        SessionStateGuard lock(shared_);
        if(!plane_||recovering_||components_closed_)throw std::runtime_error("Session radio clock unavailable");
        return plane_->clock;
    }
    void start_phase_routing(OpticalExposureSchedule& schedule,OpticalPhaseVerification& phase,
                             int ticks,bool windows){
        SessionStateGuard lock(shared_);
        if(!station_||!worker_||phase_routing_||recovering_||components_closed_)
            throw std::runtime_error("Live phase routing dependency/order violation");
        phase_routing_=std::make_unique<LivePhaseRouting>(schedule,phase,plane_->clock,ticks,windows);
    }
    BenchImuEvidence measured_bench(const OpticalPhaseVerification& phase,int64_t now){
        SessionStateGuard lock(shared_);
        if(!safety_||!identity_||recovering_||components_closed_)
            throw std::runtime_error("Measured bench unavailable");
        safety_->fresh_tracking();
        BenchImuEvidence evidence;
        evidence.forbid_controllers=true;
        evidence.min_controllers=0;
        evidence.guard_mode="strict";
        evidence.temporal_identity_verified=identity_->bindings_snapshot();
        evidence.phase=phase.state()==IdentityPhaseWindow::State::Locked?"locked":"unlocked";
        if(measured_pose_ && now>=measured_pose_->stamp && now-measured_pose_->stamp<=250000000){
            (void)measured_pose_->encode(now);
            evidence.pose_valid=measured_pose_->valid;
        }
        return evidence;
    }
    std::function<bool(const OpticalPoseFrame&,int64_t)> udp_pose_sink(){
        SessionStateGuard lock(shared_);
        if(!pose_publisher_||recovering_||components_closed_)throw std::runtime_error("Session UDP publisher unavailable");
        return [this](const OpticalPoseFrame& frame,int64_t now){
            SessionStateGuard lock(shared_);
            if(!pose_publisher_||components_closed_||worker_stopped_)
                throw std::runtime_error("Session UDP publisher dispatch revoked");
            return pose_publisher_->send(frame,now);
        };
    }
    void start_camera(const std::filesystem::path& root,const std::filesystem::path& archive,
        camera::CameraHooks hooks,std::function<std::string()> boot,std::function<CameraReadClocks()> read,
        const std::string& calibration,bool as_root=false,uint64_t budget=2ULL*1024*1024*1024,
        std::function<std::string(const std::filesystem::path&)> active_calibration={}){
        if(!station_||camera_attempted_||recovering_||components_closed_||!boot||!read||!hooks.now||calibration.empty())
            throw std::runtime_error("Session camera dependency/order violation");
        camera_attempted_=true;
        try{
            safety_->fresh_tracking();camera_now_=hooks.now;camera_boot_=std::move(boot);camera_read_=std::move(read);
            const auto current_boot=camera_boot_();
            if(current_boot.empty())throw std::runtime_error("Camera current boot missing");
            const auto prior_close=hooks.before_close;
            hooks.before_close=[this,prior_close] {
                if(transaction_&&transaction_->unresolved())
                    std::cerr<<"Camera cleanup: SRAM barriers unresolved; retaining SRAM owner\n";
                if(prior_close)prior_close();
            };
            camera_=std::make_shared<camera::LiveCameraSource>(root,archive,std::move(hooks),as_root,budget);
            // Register before start, retaining failed/ambiguous recording ownership.
            auto close=[source=camera_]{source->close();};
#ifndef _WIN32
            if(supervisor_)supervisor_->own_camera(std::move(close));
            else
#endif
                profile_.own_camera(std::move(close));
            const auto dataset=camera_->start();
            const auto provenance=active_calibration?active_calibration(dataset):calibration;
            if(provenance.empty())throw std::runtime_error("Active camera calibration provenance missing");
            camera_preflight_=std::make_unique<CameraAcquisition>(camera_now_(),current_boot,dataset.string(),provenance);
        }catch(...){identity_->invalidate();recovering_=true;throw;}
    }
    void camera_health(){
        SessionStateGuard lock(shared_);if(camera_)camera_->poll();}
    bool camera_poll(){
        SessionStateGuard lock(shared_);
        if(!camera_||!camera_preflight_||recovering_||components_closed_)
            throw std::runtime_error("Session camera dispatch revoked");
        try{
            safety_->fresh_tracking();camera_->poll();
            for(const auto& row:camera_->take_metadata()){
                const auto clocks=camera_read_();
                camera_preflight_->observe(row,camera_now_(),clocks);
            }
            return camera_preflight_->poll(camera_now_(),camera_boot_());
        }catch(const std::exception& e){log_live_runtime_error(e);return false;}
    }
    const CameraAcquisitionProof& camera_proof()const{
        SessionStateGuard lock(shared_);
        if(!camera_preflight_||recovering_||components_closed_)throw std::runtime_error("Session camera proof unavailable");
    return camera_preflight_->proof();
    }
    // Frames remain owned snapshots. Acquisition must finish before dispatch;
    // startup frames can be discarded without treating them as live evidence.
    std::deque<camera::GrayFrame> take_camera_frames(){
        SessionStateGuard lock(shared_);
        if(!camera_||recovering_||components_closed_)
            throw std::runtime_error("Session camera frame dispatch revoked");
        return camera_->take_frames();
    }
    OpticalWorker& start_worker(LedModel model,NativeOpticalApi api,
        const OpticalCameraClock& camera,OpticalExposureSchedule& schedule,OpticalPhaseVerification& phase,
        std::function<int64_t()> now,std::function<bool(const OpticalPoseFrame&,int64_t)> send,
        std::shared_ptr<void> context){
        SessionStateGuard lock(shared_);
        if(!station_||worker_attempted_||recovering_||components_closed_||!context)
            throw std::runtime_error("Session worker dependency/order violation");
        worker_attempted_=true;optical_context_=std::move(context);
        try{
            pose_send_=send;  // multi-tracker optics: the other chips' PTP2 go to the same sink
            publication_=std::make_unique<PublicationExpiry>(std::move(send));
            worker_=std::make_unique<OpticalWorker>(pico::devices::chip(),std::move(model),api,"live",camera,schedule,phase,*identity_,
                std::move(now),[this](const OpticalPoseFrame& frame,int64_t stamp){
                    auto output=frame;
                    if(live_exposure_active_)output=driver_pose(frame,direct_camera_?
                        safety_->camera_origin_cached(frame.stamp):safety_->driver_origin(frame.stamp));
                    publication_->optical(output,stamp);
                    measured_pose_=frame;
                    return true;});
            if(const char* path=std::getenv("PICO_PRIOR_CALIBRATION"))worker_->prior_calibration(path);
            worker_->session_compute(shared_,[this]{return optical_epoch_;},[this]{
                return !worker_stopped_ && !worker_stopping_ && !recovering_ && !components_closed_;
            });
            return *worker_;
        }catch(...){identity_->invalidate();recovering_=true;throw;}
    }
    // Multi-tracker optics: secondaries with a factory LED model get identity episodes and PTP2 poses of their own
    // chip. Requires the worker (it borrows the optics) and the secondaries started with dark LEDs.
    void start_multi_optics(std::vector<MultiChipModel> models,const LedModel& primary_model,NativeOpticalApi api,
                            OpticalExposureSchedule* schedule,std::function<int64_t()> now){
        SessionStateGuard lock(shared_);
        if(!worker_||multi_optics_||recovering_||components_closed_)
            throw std::runtime_error("Multi-tracker optics dependency/order violation");
        if(!multi_optics_enabled_||!secondaries_.size()){
            std::ofstream("/tmp/pico-diag.log",std::ios::app)<<"multi_optics: not enabled (PICO_MULTI_OPTICS=0 or no secondary)"<<std::endl;
            return;
        }
        std::vector<MultiChipModel> enrolled;
        for(auto& m:models)for(size_t i=0;i<secondaries_.size();++i)if(secondaries_.at(i).entry.chip==m.chip)enrolled.push_back(m);
        multi_optics_=std::make_unique<MultiChipOptics>(api,MultiChipModel{first_target_chip,pico::devices::serial(),primary_model},
                                                        std::move(enrolled));
        multi_optics_->send=[this](const OpticalPoseFrame& frame,int64_t stamp){
            // The worker calls this with the session lock held, right after the first target's own publication.
            if(!pose_send_||worker_stopped_||components_closed_)return false;
            auto output=frame;
            if(live_exposure_active_)output=driver_pose(frame,direct_camera_?
                safety_->camera_origin_cached(frame.stamp):safety_->driver_origin(frame.stamp));
            return pose_send_(output,stamp);
        };
        led_schedule_=std::make_unique<MultiLedSchedule>(*multi_optics_,std::move(now));
        led_schedule_->exposure_schedule(schedule);
        led_schedule_->attach(secondaries_);
        worker_->optical_peers(multi_optics_.get());
    }
    MultiChipOptics* multi_optics(){SessionStateGuard lock(shared_);return multi_optics_.get();}
    MultiLedSchedule* led_schedule(){SessionStateGuard lock(shared_);return led_schedule_.get();}
    // Live pixels always use the supervised observer history for EACH exposure.
    // Replay calls the same owned worker with its original recorded head proof.
    bool live_exposure(IdentityPair pair,const std::array<CameraMetadata,2>& metadata,bool dark=false){
        SessionStateGuard lock(shared_);
        if(peer_ && peer_->stock_mode_requested) return false;
        if(!worker_||worker_stopped_||worker_stopping_||recovering_||components_closed_)
            {log_optical_drop("Session worker dispatch revoked");return false;}
        try{
            if(!direct_camera_)(void)camera_proof();
            for(auto& frame:pair.frames){
                frame.world_from_head=safety_->head_pose(frame.stamp);
                (void)safety_->driver_origin(frame.stamp);
            }
            live_exposure_active_=true;
            const bool result=worker_->exposure(std::move(pair),metadata,dark);
            live_exposure_active_=false;
            return result;
        }catch(const std::exception& e){live_exposure_active_=false;log_live_runtime_error(e);return false;}
    }
    void direct_camera(bool enabled){
        SessionStateGuard lock(shared_);direct_camera_=enabled;}
    bool measured_exposure(IdentityPair pair,const std::array<CameraMetadata,2>& metadata,bool dark=false){
        SessionStateGuard lock(shared_);
        if(peer_ && peer_->stock_mode_requested) return false;
        if(!worker_||worker_stopped_||worker_stopping_||recovering_||components_closed_)
            {log_optical_drop("Session measured worker dispatch revoked");return false;}
        try{
            if(!direct_camera_)(void)camera_proof();
            else safety_->camera_tracking(); // prepare observer history before solve
            live_exposure_active_=true;
            const bool result=worker_->try_measured_exposure(std::move(pair),metadata,[this](int64_t stamp)->std::optional<Rigid>{
                if(direct_camera_)return safety_->camera_head_cached(stamp);
                (void)safety_->driver_origin(stamp);
                return safety_->head_pose(stamp);
            },dark);
            live_exposure_active_=false;
            return result;
        }catch(const std::exception& e){live_exposure_active_=false;log_live_runtime_error(e);return false;}
    }
    void start_clock_poll(OwnerIO io,RoyClock& clock,std::function<int64_t()> raw,
        std::function<void(const ClockProbeEvent&)> log){
        SessionStateGuard lock(shared_);
        if(!worker_||query_clock_!=&clock||clock_attempted_||recovering_||components_closed_)
            throw std::runtime_error("Session clock dependency/order violation");
        clock_attempted_=true;
        auto read=io.read;
        if(read)io.read=[this,read](uint32_t a,size_t n){
            SessionStateGuard::UnlockedIO unlocked(shared_);return read(a,n);
        };
        try{clock_poll_=std::make_unique<RoyClockPoll>(std::move(io),clock,std::move(raw),std::move(log));}
        catch(...){identity_->invalidate();recovering_=true;throw;}
    }
    void clock_poll(bool ready){
        SessionStateGuard lock(shared_);
        if(!clock_poll_||recovering_||components_closed_)throw std::runtime_error("Session clock dispatch revoked");
        try{clock_poll_->poll(ready);}catch(const std::exception& e){log_live_runtime_error(e);}
    }
    // Install after the query peer has bound the SAME DataPlane clock. Reads go
    // through this owner's attributed debugger, never a caller's unrelated IO.
    void start_continuous_clock(std::function<int64_t()> mono,std::function<int64_t()> raw,
        std::function<bool()> ready,std::function<void(const ClockProbeEvent&)> log){
        SessionStateGuard lock(shared_);
        if(!plane_||query_clock_!=&plane_->clock||!ready)
            throw std::runtime_error("Continuous Roy poll requires shared data-plane clock");
        OwnerIO io;io.now=std::move(mono);
        io.read=[this](uint32_t address,size_t count){return query_debug_->read(address,count);};
        start_clock_poll(std::move(io),plane_->clock,std::move(raw),std::move(log));
        clock_ready_=std::move(ready);
    }
#ifndef _WIN32
    void start_linux_clock(std::function<void(const ClockProbeEvent&)> log){
        auto clock=[](clockid_t id){timespec t{};
            if(clock_gettime(id,&t))throw std::runtime_error("Measured Linux Roy host clock failed");
            return int64_t(t.tv_sec)*1000000000+t.tv_nsec;};
        start_continuous_clock([clock]{return clock(CLOCK_MONOTONIC);},
            [clock]{return clock(CLOCK_MONOTONIC_RAW);},
            [this]{return peer_&&peer_->diagnostics_ready()&&peer_->native_ir31_applied;},std::move(log));
    }
#endif
    void publication_poll(int64_t now,bool connected,bool allowed){
        SessionStateGuard lock(shared_);
        if(!publication_)return;
        if(worker_stopped_||components_closed_)throw std::runtime_error("Session publication dispatch revoked");
        try{publication_->poll(now,connected,allowed);}catch(const std::exception& e){log_live_runtime_error(e);}
    }
    void stop_worker(){
        std::function<void()> stop;
        {SessionStateGuard lock(shared_);worker_stopping_=true;++optical_epoch_;stop=optics_stop_;}
        {SessionStateGuard::UnlockedIO unlocked(shared_);
            if(stop)stop();} // join without holding shared_
        SessionStateGuard lock(shared_);
        if(worker_&&!worker_stopped_){worker_->stop_join();worker_stopped_=true;}}
    bool led_restore_required() const {
        SessionStateGuard lock(shared_);
        return identity_ && identity_->owner.restore_required();
    }
    void begin_led_recovery() {
        SessionStateGuard lock(shared_);
        if (!led_restore_required()) return;
        if (!worker_ || worker_stopped_ || recovering_ || components_closed_)
            {std::cerr<<"Retain supervisor: measured LED recovery unavailable; continuing cleanup\n";return;}
        // Reuse the original measured camera/episode; never fabricate lit return
        // or restart an episode. All existing safety checks stay active.
        ++optical_epoch_;
        led_recovery_only_ = true;
        worker_->begin_led_recovery();
        measured_pose_.reset();
        plane_->feedback_enabled = false;
    }
    void stop_camera(){
        // Recording/decoder ownership follows profile_owner.py: only close
        // after all MCU barriers, even when worker startup failed.
        if(transaction_ && transaction_->unresolved())
            std::cerr<<"Camera cleanup: SRAM barriers unresolved; retaining SRAM owner\n";
        if(camera_)camera_->close();
    }
    void stop_supervised_dispatch(){
        if(!supervised_command_)throw std::runtime_error("Session is not supervised");
        stop_worker();
        SessionStateGuard lock(shared_);
        // A ledger is cleared only by measured lit return and fresh mode.
        // Invalidation alone must never authorize physical SRAM restoration.
        if(identity_&&identity_->owner.restore_required())
            std::cerr<<"Retain supervisor: native LED restoration unresolved; continuing\n";
        recovering_=true;
        if(identity_)identity_->invalidate();
    }
    void close_components(){
        if(transaction_&&transaction_->unresolved()){std::cerr<<"Session retained: SRAM barriers unresolved\n";return;}
        stop_worker(); // strictly before publisher, camera, debugger and profile release
        SessionStateGuard lock(shared_);
        // Once barriers are proven, an independent camera-close error must not
        // leave the tracking observer child running. Keep failed state for retry.
        stop_camera_worker([this] {
            if(camera_)camera_->close();
        },[this] {
#ifndef _WIN32
            if(observer_process_)observer_process_->close();
#endif
        });
        if(debugger_)debugger_->close();
        components_closed_=true;debugger_started_=false;safety_=nullptr;
        transaction_=nullptr;
        supervised_command_={};
#ifndef _WIN32
        supervisor_=nullptr;
#endif
        phase_routing_.reset();
        measured_pose_.reset();
        worker_.reset();led_schedule_.reset();multi_optics_.reset();pose_send_={};publication_.reset();clock_poll_.reset();clock_ready_={};optical_context_.reset();observer_.reset();
        secondaries_.clear();station_.reset();bridge_.reset();peer_.reset();identity_.reset();query_debug_.reset();radio_.reset();plane_.reset();sram_.reset();debugger_.reset();debugger_context_.reset();
        camera_.reset();camera_preflight_.reset();
        pose_publisher_.reset();imu_publisher_.reset();
#ifndef _WIN32
        observer_process_.reset();
#endif
    }
    // Explicit only. Destruction must never restore/release unresolved SRAM.
    phase5::Cleanup cleanup(){
        if(debugger_||supervised_command_){phase5::Cleanup result;result.deferred=true;
            result.errors.push_back("Session debugger/components must close before profile release");return result;}
        return profile_.cleanup();
    }
};
}

#include "live_implementation_registry.hpp"
PICO_LIVE_IMPLEMENTATION(pico_impl_led_recovery, "led-recovery");
PICO_LIVE_IMPLEMENTATION(pico_impl_fusion_disabled, "fusion-disabled");
