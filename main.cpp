#include <iostream>
#include "main.h"
#include "oneM2M.h"
#include "TwinDevice.h"
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

    std::cout << "Fetching Group MID..." << std::endl;
    auto grpMidList = client.fetchGRPMid().get();
    std::cout << "Group MID fetched successfully:" << std::endl;
    for (const auto& mid : grpMidList) {
        std::cout << "  - " << utility::conversions::to_utf8string(mid) << std::endl;
    }

    // setting twinDevice
    TwinDevice device(1, CLIENT, 1);
    device.initializeSensorOnlineStatus({{"accelerometer", true}, {"gps", true}, {"ultrasonic", true}, {"temperature", false}});

    while (true) {
        for (const auto& mid : grpMidList) {
            web::json::value body = client.discoveryCIN(utility::conversions::to_string_t(mid));
            auto parsedData = client.parseBody(body);
            device.updateSensorData(parsedData);
        }
        device.evaluateDangerLevel();
        device.printDeviceInfo();
        std::this_thread::sleep_for(std::chrono::seconds(device.updateInterval));

        // send to ue5
    }



    return 0;


    /* don't know how to work sub function in TinyIoT... so just use discovery instead
    std::cout << "Starting notification server..." << std::endl;
    std::promise<void> serverStarted;
    std::future<void> serverReady = serverStarted.get_future();

    std::thread notificationThread([&client, &serverStarted]() {
        client.startNotificationServer(serverStarted);
    });
    notificationThread.detach();

    std::cout << "Waiting for notification server to start..." << std::endl;
    serverReady.wait();
    std::cout << "Notification server started successfully!" << std::endl;

    int wait;
    std::cin >> wait;

    int subID = 1;
    for (const auto& mid : grpMidList) {
        std::cout << "Ensuring subscription for: " << utility::conversions::to_utf8string(mid) << std::endl;
        utility::string_t subName = utility::string_t(U(CLIENT)) + U("_sub") + utility::conversions::to_string_t(std::to_string(subID));
        client.ensureSubscription(subName).wait();
        ++subID;
    }
    */
}