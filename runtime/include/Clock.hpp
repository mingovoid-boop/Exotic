#pragma once

#include <chrono>

namespace exotic {

class Clock {
public:
    Clock();

    void Reset();
    void Tick();

    double DeltaTime() const;
    double ElapsedTime() const;

private:
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point lastTime_;
    double deltaTime_;
};

}
