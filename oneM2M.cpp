#include "oneM2M.h"
#include <stdexcept>
#include <iostream>
#include <cpprest/http_listener.h>

OneM2MClient::OneM2MClient(const std::string& ri, const std::string& content_type) : ri(ri) {
    headers.add(U("Accept"), U("application/json"));
    headers.add(U("X-M2M-Origin"), U(X_M2M_ORIGIN));
    headers.add(U("X-M2M-RVI"), U(X_M2M_RVI));
    headers.add(U("X-M2M-RI"), U(ri));
    setContentType(content_type);
}

void OneM2MClient::setContentType(const std::string& content_type) {
    int ty = getContentType(content_type);
    if (ty == -1) {
        throw std::invalid_argument("Invalid content type: " + content_type);
    }

    headers[U("Content-Type")] = U("application/json;ty=" + std::to_string(ty));
}

void OneM2MClient::setRI(const std::string& ri) {
    this->ri = ri;
    headers[U("X-M2M-RI")] = U(ri);
}

int OneM2MClient::getContentType(const std::string& content_type) {
    static const std::unordered_map<std::string, int> types = {
        {"acp", 1},
        {"ae", 2},
        {"cnt", 3},
        {"cin", 4},
        {"cb", 5},
        {"grp", 9},
        {"sub", 23}
    };

    auto it = types.find(content_type);
    return (it != types.end()) ? it->second : -1;
}

pplx::task<void> OneM2MClient::verifyCSE() {
    setRI("retrieve_cse_base");
    setContentType("cb");
    return sendGetRequest(std::string(CSE_URL) + "/" + CSE_BASE)
        .then([](web::http::http_response response) {
            if (response.status_code() == 200) {
                std::wcout << L"CSE is accessible." << std::endl;
            } else {
                std::wcerr << L"Failed to access CSE: " << response.status_code() << std::endl;
            }
        });
}

pplx::task<std::string> OneM2MClient::fetchGRPMid() {
    setRI("retrieve_grp");
    setContentType("grp");
    return sendGetRequest(std::string(CSE_URL) + "/" + CSE_BASE + "/" + GRP_RI)
        .then([](web::http::http_response response) -> std::string {
            if (response.status_code() != 200) {
                throw std::runtime_error("GRP not found");
            }
            auto body = response.extract_json().get();
            return utility::conversions::to_utf8string(body[U("m2m:grp")][U("mid")].as_string());
        });
}

pplx::task<void> OneM2MClient::ensureSubscription(const std::string& subName) {
    setRI("retrieve_sub");
    setContentType("sub");
    return sendGetRequest(std::string(CSE_URL) + "/" + CSE_BASE + "/" + subName)
        .then([this, subName](web::http::http_response response) {
            if (response.status_code() == 200) {
                std::wcout << L"Subscription already exists: " << subName.c_str() << std::endl;
                return pplx::task_from_result();
            } else if (response.status_code() == 404) {
                std::wcout << L"Subscription does not exist. Creating: " << subName.c_str() << std::endl;

                web::json::value payload;
                payload[U("m2m:sub")][U("rn")] = web::json::value::string(U(subName));
                payload[U("m2m:sub")][U("nu")] = web::json::value::array({ web::json::value::string(U(SUB_NOTIFY_URL)) });
                payload[U("m2m:sub")][U("nct")] = web::json::value::number(2);
                payload[U("m2m:sub")][U("enc")] = web::json::value::object({ { U("net"), web::json::value::array({ web::json::value::number(3), web::json::value::number(4) }) } });

                setRI("create_sub");
                return sendPostRequest(std::string(CSE_URL) + "/" + CSE_BASE, payload)
                    .then([](web::http::http_response response) {
                        if (response.status_code() == 201) {
                            std::wcout << L"Subscription created successfully." << std::endl;
                        } else {
                            std::wcerr << L"Failed to create subscription: " << response.status_code() << std::endl;
                        }
                    });
            } else {
                throw std::runtime_error("Failed to check subscription: HTTP " + std::to_string(response.status_code()));
            }
        });
}

void OneM2MClient::parseBody(const web::json::value& body) {
    std::wcout << L"Parsed OneM2M Body: " << body.serialize() << std::endl;
}

void OneM2MClient::startNotificationServer(const std::string& listenURL) {
    web::http::experimental::listener::http_listener listener(U(listenURL));

    listener.support(web::http::methods::POST, [this](web::http::http_request request) {
        request.extract_json()
            .then([this](web::json::value body) {
                std::wcout << L"Notification received:" << std::endl;
                parseBody(body);
            })
            .catch([](const std::exception& e) {
                std::wcerr << L"Error handling notification: " << e.what() << std::endl;
            });
    });

    try {
        listener.open()
            .then([&listenURL]() { std::wcout << L"Notification server started at: " << listenURL.c_str() << std::endl; })
            .wait();

        while (true) {
            std::this_thread::yield();
        }
    } catch (const std::exception& e) {
        std::wcerr << L"Failed to start notification server: " << e.what() << std::endl;
    }
}

pplx::task<web::http::http_response> OneM2MClient::sendPostRequest(const std::string& url, const web::json::value& payload) {
    web::http::client::http_client client(U(url));
    web::http::http_request request(web::http::methods::POST);
    request.headers() = headers;
    request.set_body(payload.serialize());

    return client.request(request);
}

pplx::task<web::http::http_response> OneM2MClient::sendGetRequest(const std::string& url) {
    web::http::client::http_client client(U(url));
    web::http::http_request request(web::http::methods::GET);
    request.headers() = headers;

    return client.request(request);
}
