#pragma once

namespace exotic {

struct NetworkState {
    double latencyMs = 0.0;
    double jitterMs = 0.0;
    double packetLoss = 0.0;
    double bandwidthMbps = 0.0;
    double signalQuality = 1.0;
    bool connected = true;

    double StabilityScore() const {
        double latencyPenalty = latencyMs > 120.0 ? 0.65 : 1.0;
        double jitterPenalty = jitterMs > 30.0 ? 0.75 : 1.0;
        double lossPenalty = packetLoss > 0.03 ? 0.60 : 1.0;
        double signalPenalty = signalQuality < 0.50 ? 0.70 : 1.0;
        return latencyPenalty * jitterPenalty * lossPenalty * signalPenalty;
    }
};

}
