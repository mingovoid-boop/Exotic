#pragma once

#include "Clock.hpp"
#include "EventBus.hpp"
#include "Scheduler.hpp"

namespace exotic {

class Runtime {
public:
    Runtime();

    void Initialize();
    void Start();
    void Update();
    void Shutdown();

    Clock& GetClock();
    Scheduler& GetScheduler();
    EventBus& GetEventBus();

    bool IsRunning() const;

private:
    Clock clock_;
    Scheduler scheduler_;
    EventBus eventBus_;
    bool running_;
};

}

