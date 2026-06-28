#include "Runtime.hpp"

#include <iostream>

int main() {
    exotic::Runtime runtime;

    runtime.GetEventBus().Subscribe(exotic::EventType::Runtime, [](const exotic::Event& event) {
        std::cout << "[event] " << event.Name() << std::endl;
    });

    runtime.Initialize();
    runtime.Start();

    runtime.GetScheduler().Queue(exotic::Task("HelloTask", []() {
        std::cout << "Hello from Exotic Runtime." << std::endl;
    }));

    runtime.Update();
    runtime.Shutdown();

    return 0;
}
