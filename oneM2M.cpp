#include "oneM2M.h"
#include <iostream>

// 네임스페이스 설정
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;

// Constructor
OneM2M::OneM2M(const std::string& cseUrl, const std::string& notificationUrl)
    : cseUrl(cseUrl), notificationUrl(notificationUrl) {}

// Function to create a subscription
void OneM2M::createSubscription(const std::string& resourceName, const std::string& subscriptionName) {
    try {
        // Construct the full subscription URL
        uri subscriptionUri(U(cseUrl + "/~/in-cse/in-name/" + resourceName));

        // HTTP client for sending the POST request
        http_client client(subscriptionUri);

        // JSON payload for subscription
        json::value subPayload;
        subPayload[U("m2m:sub")][U("rn")] = json::value::string(U(subscriptionName));  // Subscription name
        subPayload[U("m2m:sub")][U("nu")] = json::value::array();                      // Notification URI
        subPayload[U("m2m:sub")][U("nu")][0] = json::value::string(U(notificationUrl));
        subPayload[U("m2m:sub")][U("nct")] = json::value::number(2);                   // Notification Content Type

        // Send the POST request
        client.request(methods::POST, U(""), subPayload.serialize(), U("application/json"))
            .then([](http_response response) {
                if (response.status_code() == status_codes::Created) {
                    std::cout << "Subscription created successfully!" << std::endl;
                } else {
                    std::cerr << "Failed to create subscription. Status: " << response.status_code() << std::endl;
                }
            }).wait();
    } catch (const std::exception& ex) {
        std::cerr << "Error creating subscription: " << ex.what() << std::endl;
    }
}

// Function to start a notification server
void OneM2M::startNotificationServer() {
    try {
        // Create HTTP listener for notifications
        http_listener listener(U(notificationUrl));

        // Handle POST requests
        listener.support(methods::POST, [](http_request request) {
            // Extract and print the notification data
            request.extract_json().then([](json::value body) {
                std::wcout << L"Notification received: " << body.serialize() << std::endl;
            }).wait();

            // Reply to the CSE server
            request.reply(status_codes::OK, U("Notification received."));
        });

        // Start the server
        listener.open().wait();
        std::cout << "Notification server started at: " << notificationUrl << std::endl;

        // Keep the server running
        std::string stopSignal;
        std::cout << "Press ENTER to stop the server..." << std::endl;
        std::getline(std::cin, stopSignal);

        // Stop the server
        listener.close().wait();
    } catch (const std::exception& ex) {
        std::cerr << "Error starting notification server: " << ex.what() << std::endl;
    }
}
