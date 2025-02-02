#ifndef ONEM2M_H
#define ONEM2M_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <unordered_map>
#include <string>
#include "config.h"

class OneM2MClient {
public:
    OneM2MClient(const std::string& ri, const std::string& content_type);

    void setContentType(const std::string& content_type);
    void setRI(const std::string& ri);

    pplx::task<void> verifyCSE();
    pplx::task<std::string> fetchGRPMid();
    pplx::task<void> ensureSubscription(const std::string& subName);

// void 말고 적절하게 수정 필요
    void parseBody(const web::json::value& body);
    void startNotificationServer(const std::string& listenURL);

    pplx::task<web::http::http_response> sendPostRequest(const std::string& url, const web::json::value& payload);
    pplx::task<web::http::http_response> sendGetRequest(const std::string& url);

    std::string ri;
    web::http::http_headers headers;

    static int getContentType(const std::string& content_type);
};

#endif // ONEM2M_H
