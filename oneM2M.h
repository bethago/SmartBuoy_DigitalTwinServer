#ifndef ONEM2M_H
#define ONEM2M_H

#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <string>

class OneM2M {
public:
    // Constructor
    OneM2M(const std::string& cseUrl, const std::string& notificationUrl);

    // Function to create a subscription
    void createSubscription(const std::string& resourceName, const std::string& subscriptionName);

    // Function to start a notification server
    void startNotificationServer();

private:
    std::string cseUrl;            // Base URL of the CSE server
    std::string notificationUrl;   // URL of the local notification server
};

#endif // ONEM2M_H
