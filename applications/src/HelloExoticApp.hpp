#include "../include/Application.hpp"
#include <iostream>

namespace exotic {
class HelloExoticApp : public Application {
public:
bool Initialize(ExoticContext& context) override { std::cout << "Initializing Exotic..." << std::endl; return true; }
void Update(ExoticContext& context) override { auto id = context.memory.Store("Hello","Exotic"); context.observer.Observe("Application","Running"); context.execution.Execute(id); std::cout << "Exotic running." << std::endl; }
void Shutdown(ExoticContext& context) override { std::cout << "Shutting down Exotic." << std::endl; }
};
}
