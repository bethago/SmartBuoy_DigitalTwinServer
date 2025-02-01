#ifndef TWINDEVICE_H
#define TWINDEVICE_H

#include <string>
#include <unordered_map>

class TwinDevice {
private:
    std::unordered_map<std::string, double> sensor_data;

public:
    // 센서 값 업데이트
    void updateSensor(const std::string& sensor_id, double value);

    // 센서 값 가져오기
    double getSensorValue(const std::string& sensor_id) const;

    // 모든 센서 값 출력
    void printAllSensors() const;
};

#endif // TWINDEVICE_H
