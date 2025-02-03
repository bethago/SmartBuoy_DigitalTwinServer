#ifndef UNREAL_ENGINE_H
#define UNREAL_ENGINE_H

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include "TwinDevice.h"

class UnrealEngineClient {
public:
    UnrealEngineClient(const std::string& serverIp, int serverPort);
    bool isServerAvailable();
    void sendData(const TwinDevice& device);

    std::string serverIp;
    int serverPort;
};

#endif // UNREAL_ENGINE_H
