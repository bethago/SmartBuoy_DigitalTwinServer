#include "TwinDevice.h"

TwinDevice::TwinDevice(int deviceId, std::string deviceName, int updateInterval)
    : deviceId(deviceId), deviceName(deviceName), updateInterval(updateInterval), dangerState(DangerLevel::SAFE) {
    sensorOnline["accelerometer"] = false;
    sensorOnline["gps"] = false;
    sensorOnline["ultrasonic"] = false;
    sensorOnline["temperature"] = false;
}

void TwinDevice::updateSensorData(std::pair<std::string, std::vector<double>> parsedData) {
    const std::string& sensorName = parsedData.first;
    const std::vector<double>& data = parsedData.second;

    if (sensorName == "accelerometer") {
        if (data.size() >= 3) {
            sensorData.acceleration = {data[0], data[1], data[2]};
        }
    } else if (sensorName == "gps") {
        if (data.size() >= 3) {
            sensorData.gps = {data[0], data[1], data[2]};
        }
    } else if (sensorName == "ultrasonic") {
        if (!data.empty()) {
            sensorData.ultrasonic = data[0];
        }
    } else if (sensorName == "temperature") {
        if (!data.empty()) {
            sensorData.temperature = data[0];
        }
    }
    sensorOnline[sensorName] = true;
}

double TwinDevice::calculateWavePeriod() {
    const std::vector<double>& z_values = sensorData.acceleration;
    double threshold = 10.0;
    std::vector<int> peaks;

    for (size_t i = 1; i < z_values.size() - 1; ++i) {
        if (z_values[i - 1] < z_values[i] && z_values[i] > z_values[i + 1] && z_values[i] > threshold) {
            peaks.push_back(i);
        }
    }

    if (peaks.size() >= 2) {
        return static_cast<double>(peaks.back() - peaks[peaks.size() - 2]);
    }
    return 0.0;
}

void TwinDevice::evaluateDangerLevel() {
    double wavePeriod = calculateWavePeriod();
    double waveHeight = sensorData.ultrasonic;

    if (waveHeight >= 4 && wavePeriod > 11.0) {
        dangerState = DangerLevel::DANGER;
    } else if (waveHeight >= 2 && wavePeriod >= 8.0 && wavePeriod <= 11.0) {
        dangerState = DangerLevel::ALERT;
    } else if (waveHeight > 2 && wavePeriod <= 8.0) {
        dangerState = DangerLevel::CAUTION;
    } else if (waveHeight <= 2 && wavePeriod > 8.0) {
        dangerState = DangerLevel::CAUTION;
    } else if (waveHeight < 2 && wavePeriod <= 8.0) {
        dangerState = DangerLevel::SAFE;
    } else {
        dangerState = DangerLevel::SAFE;
    }
}

void TwinDevice::initializeSensorOnlineStatus(const std::unordered_map<std::string, bool>& status) {
    for (const auto& entry : status) {
        if (sensorOnline.find(entry.first) != sensorOnline.end()) {
            sensorOnline[entry.first] = entry.second;
        }
    }
}

void TwinDevice::printDeviceInfo() {
    std::cout << "==============================================" << std::endl;
    std::cout << "Device ID: " << deviceId << std::endl;
    std::cout << "Device Name: " << deviceName << std::endl;
    std::cout << "Sensor Data:" << std::endl;
    if (sensorOnline["accelerometer"]) {
        std::cout << " - Accelerometer: [";
        for (const auto& value : sensorData.acceleration) {
            std::cout << value << " ";
        }
        std::cout << "]" << std::endl;
    }
    if (sensorOnline["gps"]) {
        std::cout << " - GPS: [";
        for (const auto& value : sensorData.gps) {
            std::cout << value << " ";
        }
        std::cout << "]" << std::endl;
    }
    if (sensorOnline["ultrasonic"]) {
        std::cout << " - Ultrasonic: [" << sensorData.ultrasonic << "]" << std::endl;
    }
    if (sensorOnline["temperature"]) {
        std::cout << " - Temperature: [" << sensorData.temperature << "]" << std::endl;
    }
    std::cout << "Danger Level: ";
    switch (dangerState) {
        case DangerLevel::SAFE: std::cout << "SAFE"; break;
        case DangerLevel::CAUTION: std::cout << "CAUTION"; break;
        case DangerLevel::ALERT: std::cout << "ALERT"; break;
        case DangerLevel::DANGER: std::cout << "DANGER"; break;
    }
    std::cout << std::endl;
}