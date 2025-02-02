#ifndef ONEM2M_H
#define ONEM2M_H

#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <unordered_map>
#include <string>
#include <thread>
#include <chrono>
#include <future>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <utility>
#include "config.h"

class OneM2MClient {
public:
    OneM2MClient(const utility::string_t &ri, const utility::string_t &content_type);

    void setContentType(const utility::string_t &content_type);
    void setRI(const utility::string_t &ri);

    pplx::task<void> verifyCSE();
    pplx::task<std::vector<utility::string_t>> fetchGRPMid();
    pplx::task<void> ensureSubscription(const utility::string_t &subName);
    web::json::value discoveryCIN(const utility::string_t &cntName);
    std::pair<std::string, std::vector<double>> parseBody(const web::json::value &body);
    void startNotificationServer(std::promise<void>& serverStarted);

    pplx::task<web::http::http_response> sendPostRequest(const utility::string_t &url, const web::json::value &payload);
    pplx::task<web::http::http_response> sendGetRequest(const utility::string_t &url);

    utility::string_t ri;
    web::http::http_headers headers;

    static int getContentType(const std::string &content_type);
};

#endif // ONEM2M_H
