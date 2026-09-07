#include "exotic/core/runtime.hpp"

#include <iostream>

int main() {
    exotic::core::Runtime runtime;
    const exotic::core::ActionRequest request{
        "Demonstrate one governed verified action",
        "core.execute.echo",
        "EXOTIC Core v0.1",
        true
    };

    const auto result = runtime.run(request);
    std::cout << "executed=" << result.executed
              << " verified=" << result.verification.passed
              << " committed=" << result.committed
              << " events=" << runtime.events().all().size()
              << " output=\"" << result.output << "\"\n";

    return result.committed ? 0 : 1;
}
