#include "Runtime.hpp"

namespace exotic {

Runtime::Runtime() : running_(false) {}

void Runtime::Initialize() {
    clock_.Reset();
    eventBus_.Publish(Event("RuntimeInitialized", EventType::Runtime));
}

void Runtime::Start() {
    running_ = true;
    eventBus_.Publish(Event("RuntimeStarted", EventType::Runtime));
}

void Runtime::Update() {
    if (!running_) return;

    clock_.Tick();
    scheduler_.Execute();
    eventBus_.Publish(Event("RuntimeUpdated", EventType::Runtime));
}

void Runtime::Shutdown() {
    running_ = false;
    scheduler_.Clear();
    eventBus_.Publish(Event("RuntimeShutdown", EventType::Runtime));
}

Clock& Runtime::GetClock() {
    return clock_;
}

Scheduler& Runtime::GetScheduler() {
    return scheduler_;
}

EventBus& Runtime::GetEventBus() {
    return eventBus_;
}

bool Runtime::IsRunning() const {
    return running_;
}

}
