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
            {"deviceId", device.deviceId},
            {"deviceName", device.deviceName},
            {"dangerState", static_cast<int>(device.dangerState)},
            {"gps", {
                {"latitude", device.sensorData.gps[0]},
                {"longitude", device.sensorData.gps[1]},
                {"altitude", device.sensorData.gps[2]}
            }}
        };

        std::string jsonString = jsonData.dump();

        write(socket, buffer(jsonString));
        std::cout << "Sent data to Unreal: " << jsonString << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Failed to send data to Unreal: " << e.what() << std::endl;
    }
}
