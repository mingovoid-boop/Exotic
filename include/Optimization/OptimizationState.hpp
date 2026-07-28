#pragma once

namespace exotic {

struct OptimizationState {
    double performanceScore = 1.0;
    double stabilityScore = 1.0;
    double efficiencyScore = 1.0;
    double batteryScore = 1.0;
    double thermalScore = 1.0;
    double networkScore = 1.0;
    double userExperienceScore = 1.0;

    double OverallScore() const {
        return (performanceScore + stabilityScore + efficiencyScore + batteryScore + thermalScore + networkScore + userExperienceScore) / 7.0;
    }
};

}
