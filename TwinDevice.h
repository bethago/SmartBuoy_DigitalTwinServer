#ifndef TWIN_DEVICE_H
#define TWIN_DEVICE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

class TwinDevice {
public:
    enum class DangerLevel {
        SAFE,
        CAUTION,
        ALERT,
        DANGER
    };

    struct SensorData {
        std::vector<double> acceleration = {0.0, 0.0, 0.0};
        std::vector<double> gps = {0.0, 0.0, 0.0};
        double ultrasonic = 0.0;
        double temperature = 0.0;
    };

    TwinDevice(int deviceId, std::string deviceName, int updateInterval);

    void updateSensorData(std::pair<std::string, std::vector<double>> parsedData);
    void initializeSensorOnlineStatus(const std::unordered_map<std::string, bool>& status);
    void evaluateDangerLevel();
    double calculateWavePeriod();
    void printDeviceInfo();

    int deviceId;
    std::string deviceName;
    SensorData sensorData;
    std::unordered_map<std::string, bool> sensorOnline;
    DangerLevel dangerState;
    int updateInterval;    // seconds
};

#endif
