#include "UnrealEngine.h"

using json = nlohmann::json;

UnrealEngineClient::UnrealEngineClient(const std::string& serverIp, int serverPort) : serverIp(serverIp), serverPort(serverPort) {}

bool UnrealEngineClient::isServerAvailable() {
    using namespace boost::asio;
    io_service ioService;
    ip::tcp::socket socket(ioService);

    try {
        socket.connect(ip::tcp::endpoint(ip::address::from_string(serverIp), serverPort));
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Unreal Engine server is not available: " << e.what() << std::endl;
        return false;
    }
}

void UnrealEngineClient::sendData(const TwinDevice& device) {
    if (!isServerAvailable()) {
        std::cerr << "Skipping data transmission: Unreal Engine server is not running." << std::endl;
        return;
    }

    using namespace boost::asio;
    io_service ioService;
    ip::tcp::socket socket(ioService);

    try
    {
        socket.connect(ip::tcp::endpoint(ip::address::from_string(serverIp), serverPort));

        json jsonData = {
            {"dn", device.deviceName},  // deviceName
            {"ds", static_cast<int>(device.dangerState)},   // dangerState
            {"gps", {
                {"lat", device.sensorData.gps[0]}, // latitude
                {"lon", device.sensorData.gps[1]},    // longitude
                {"alt", device.sensorData.gps[2]}   // altitude
            }}
        };

        std::string jsonString = jsonData.dump();

        write(socket, buffer(jsonString));
        std::cout << "Sent data to Unreal: " << jsonString << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Failed to send data to Unreal: " << e.what() << std::endl;
    }
}
