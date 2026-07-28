#pragma once
#include <string>

namespace exotic {

struct DeviceState {
    std::string deviceName = "Unknown;
 std::string deviceType = Desktop;
 double cpuLoad = 0.0;
 double memoryLoad = 0.0;
 double batteryLevel = 1.0;
 double temperatureC = 35.0;
 double networkLatencyMs = 0.0;
 double networkJitterMs = 0.0;
 double packetLoss = 0.0;
 bool charging = false;
 bool safeMode = true;
 double QualityScore() const {
 double heatPenalty = temperatureC > 80.0 ? 0.5 : 1.0;
 double batteryPenalty = batteryLevel < 0.20 ? 0.6 : 1.0;
 double networkPenalty = packetLoss > 0.05 ? 0.7 : 1.0;
 return heatPenalty * batteryPenalty * networkPenalty;
 }
};

}
