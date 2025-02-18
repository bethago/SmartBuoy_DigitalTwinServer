#include <iostream>
#include "main.h"
#include "oneM2M.h"
#include "TwinDevice.h"
#include "UnrealEngine.h"
#include "config.h"

bool isRealTimeCommunication() {
    std::cout << "Select mode: [1] Real-time Communication, [2] Log-based Simulation: ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        return true;
    } else if (choice == 2) {
        std::cout << "Log-based simulation is not supported in this version. Exiting..." << std::endl;
        return false;
    } else {
        std::cout << "Invalid choice. Exiting..." << std::endl;
        return false;
    }
}

int main() {
    if (!isRealTimeCommunication()) {
        return 0;
    }

    // setting oneM2M
    OneM2MClient client(U("default_ri"), U("cb"));

    std::cout << "Verifying CSE..." << std::endl;
    client.verifyCSE().wait();

    // setting twinDevice
    std::vector<TwinDevice> devices;
    std::vector<std::string> clientNames = CLIENTS;
    for (int i = 0;i < CLIENT_COUNT; i++) {
        devices.emplace_back(i+1, clientNames[i]);
    }

    std::cout << "Fetching Group MID..." << std::endl;
    for (auto& device : devices) {
        device.cntUris = client.fetchGRPMid(utility::conversions::to_string_t(device.deviceName)).get();
        std::cout << device.deviceName << std::endl;
        for (const auto& mid : device.cntUris) {
            std::cout << "  - " << utility::conversions::to_utf8string(mid) << std::endl;
        }
        device.initializeSensorOnlineStatus();
    }
    std::cout << "Group MID fetched successfully:" << std::endl;

    // setting unrealEngine
    UnrealEngineClient ueClient(VS_SERVER_IP, VS_SERVER_PORT);

    while (true) {
        for (auto& device : devices) {
            for (const auto& uri : device.cntUris) {
                web::json::value body = client.discoveryCIN(utility::conversions::to_string_t(uri));
                auto parsedData = client.parseBody(body);
                device.updateSensorData(parsedData);
            }
            device.evaluateDangerLevel();
            device.printDeviceInfo();
            ueClient.sendData(device);
        }
        std::this_thread::sleep_for(std::chrono::seconds(UPDATE_INTERVAL));
    }

    return 0;
}