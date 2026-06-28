#include "../include/ApplicationRunner.hpp"
#include "HelloExoticApp.hpp"

int main() {
exotic::HelloExoticApp app;
exotic::ApplicationRunner runner;
return runner.Run(app);
}
