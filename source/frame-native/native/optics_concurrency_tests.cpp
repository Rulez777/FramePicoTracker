#include "session_state_guard.hpp"
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace {
void require(bool b){if(!b)throw std::runtime_error("optics concurrency invariant");}
// Synthetic identity/phase/clock/publication state, using the exact owner lock
// and synchronous I/O release mechanism. No hardware or invented pose inputs.
struct State {
    std::recursive_mutex shared;
    unsigned identity=0,phase=0,clock=0,published=0;
    void update(){
        pico::SessionStateGuard nested(shared);
        require(identity==phase&&phase==clock&&clock==published);
        ++identity;std::this_thread::yield();++phase;++clock;++published;
    }
    void measured_exposure(){pico::SessionStateGuard lock(shared);update();}
    void radio_update(){pico::SessionStateGuard lock(shared);update();}
};
void hammer(){
    State state;std::atomic<bool> go{false};std::exception_ptr optics_error,radio_error;
    auto optics=std::thread([&]{try{while(!go)std::this_thread::yield();
    for(int i=0;i<20000;++i)state.measured_exposure();}catch(...){optics_error=std::current_exception();}});
    auto radio=std::thread([&]{try{go=true;for(int i=0;i<20000;++i)state.radio_update();}catch(...){radio_error=std::current_exception();}});
    optics.join();radio.join();
    if(optics_error)std::rethrow_exception(optics_error);
    if(radio_error)std::rethrow_exception(radio_error);
    require(state.published==40000);
}
void overlap(bool fail_io){
    State state;std::promise<void> enter_io,solved;
    auto io=enter_io.get_future();auto done=solved.get_future();
    auto optics=std::thread([&]{io.wait();state.measured_exposure();solved.set_value();});
    bool progressed=false,caught=false;
    {
        pico::SessionStateGuard radio(state.shared);
        pico::SessionStateGuard query(state.shared);
        try{
            pico::SessionStateGuard::UnlockedIO unlocked(state.shared);
            pico::SessionStateGuard::UnlockedIO nested_transport(state.shared);
            enter_io.set_value();
            progressed=done.wait_for(std::chrono::seconds(2))==std::future_status::ready;
            if(fail_io)throw std::runtime_error("synthetic SWD failure");
        }catch(const std::runtime_error&){caught=true;}
        // Both recursive acquisitions must be restored even on an exception.
        state.update();
    }
    optics.join();require(progressed&&caught==fail_io&&state.published==2);
}
}
int main(){try{hammer();overlap(false);overlap(true);std::cout<<"optics concurrency PASS (synthetic state, I/O overlap and unwind)\n";return 0;}
catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
