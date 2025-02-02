#include <iostream>
#include "main.h"
#include "OneM2M.h"
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

    try {
        OneM2MClient client("default_ri", "cb");

        std::cout << "Verifying CSE..." << std::endl;
        client.verifyCSE().wait();

        std::cout << "Fetching Group MID..." << std::endl;
        std::string grpMid = client.fetchGRPMid().get();
        std::cout << "Group MID: " << grpMid << std::endl;

        std::cout << "Extracting container URIs and ensuring subscription..." << std::endl;
        std::istringstream uriStream(grpMid);
        std::string containerURI;
        while (std::getline(uriStream, containerURI, ';')) {
            std::cout << "Processing container URI: " << containerURI << std::endl;
            client.ensureSubscription(containerURI).wait();
        }

        std::cout << "Starting notification server..." << std::endl;
        client.startNotificationServer(SUB_NOTIFY_URL);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}