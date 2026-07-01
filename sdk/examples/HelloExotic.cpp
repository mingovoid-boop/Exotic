#include "../include/ExoticContext.hpp"
#include <iostream>

int main() {
exotic::ExoticContext exotic;
auto memoryId = exotic.memory.Store("Exotic Canon", "Universal graph-backed intelligence SDK.");
auto observationId = exotic.observer.Observe("UserInput", "Build Exotic as one connected system.");
exotic.relationship.Connect(observationId, memoryId);
auto prediction = exotic.prediction.Predict(memoryId);
auto alignment = exotic.alignment.Evaluate(memoryId);
auto result = exotic.execution.Execute(memoryId);
exotic.learning.Learn(memoryId, result.success);
std::cout << "Prediction: " << prediction.description << std::endl;
std::cout << "Alignment: " << alignment.overall << std::endl;
std::cout << "Execution: " << result.message << std::endl;
return 0;
}
