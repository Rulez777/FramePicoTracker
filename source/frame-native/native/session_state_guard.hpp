#pragma once
#include <mutex>
#include <vector>

namespace pico {
// Track recursive acquisitions so synchronous radio I/O can temporarily release
// ALL acquisitions on this thread. No state may be accessed inside UnlockedIO.
class SessionStateGuard {
    std::recursive_mutex& mutex_;
    SessionStateGuard* previous_;
    bool held_=true;
    inline static thread_local SessionStateGuard* current_=nullptr;
public:
    explicit SessionStateGuard(std::recursive_mutex& mutex):mutex_(mutex),previous_(current_){
        mutex_.lock();current_=this;
    }
    ~SessionStateGuard(){current_=previous_;mutex_.unlock();}
    SessionStateGuard(const SessionStateGuard&)=delete;
    class UnlockedIO {
        std::recursive_mutex& mutex_;
        std::vector<SessionStateGuard*> released_;
    public:
        explicit UnlockedIO(std::recursive_mutex& mutex):mutex_(mutex){
            for(auto* p=current_;p;p=p->previous_)
                if(&p->mutex_==&mutex_&&p->held_)released_.push_back(p);
            for(auto* p:released_){p->held_=false;mutex_.unlock();}
        }
        ~UnlockedIO(){for(auto* p:released_){mutex_.lock();p->held_=true;}}
        UnlockedIO(const UnlockedIO&)=delete;
    };
};
}
