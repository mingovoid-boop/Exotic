#include "Clock.hpp"

namespace exotic {

Clock::Clock() : deltaTime_(0.0) {
    Reset();
}

void Clock::Reset() {
    startTime_ = std::chrono::steady_clock::now();
    lastTime_ = startTime_;
    deltaTime_ = 0.0;
}

void Clock::Tick() {
    auto now = std::chrono::steady_clock::now();
    deltaTime_ = std::chrono::duration<double>(now - lastTime_).count();
    lastTime_ = now;
}

double Clock::DeltaTime() const {
    return deltaTime_;
}

double Clock::ElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - startTime_).count();
}

}
