#include "oneM2M.h"

OneM2MClient::OneM2MClient(const utility::string_t &ri, const utility::string_t &content_type) : ri(ri) {
    headers.add(U("Accept"), U("application/json"));
    headers.add(U("X-M2M-Origin"), U(X_M2M_ORIGIN));
    headers.add(U("X-M2M-RVI"), U(X_M2M_RVI));
    headers.add(U("X-M2M-RI"), ri);
    setContentType(content_type);
}

void OneM2MClient::setContentType(const utility::string_t &content_type) {
    int ty = getContentType(utility::conversions::to_utf8string(content_type));
    if (ty == -1) {
        throw std::invalid_argument("Invalid content type: " + utility::conversions::to_utf8string(content_type));
    }

    headers[U("Content-Type")] = U("application/json;ty=") + utility::conversions::to_string_t(std::to_string(ty));
}

void OneM2MClient::setRI(const utility::string_t &ri) {
    this->ri = ri;
    headers[U("X-M2M-RI")] = ri;
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
    setRI(U("retrieve_cse_base"));
    setContentType(U("cb"));
    return sendGetRequest(utility::string_t(U(CSE_URL)) + U("/") + U(CSE_BASE))
        .then([](web::http::http_response response) {
            if (response.status_code() == 200) {
                std::wcout << L"CSE is accessible." << std::endl;
            } else {
                std::wcerr << L"Failed to access CSE: " << response.status_code() << std::endl;
            }
        });
}

pplx::task<std::vector<utility::string_t>> OneM2MClient::fetchGRPMid() {
    setRI(U("retrieve_grp"));
    setContentType(U("grp"));
    return sendGetRequest(utility::string_t(U(CSE_URL)) + U("/") + U(CSE_BASE) + U("/") + U(GRP_RI))
        .then([](web::http::http_response response) -> std::vector<utility::string_t> {
            if (response.status_code() != 200) {
                throw std::runtime_error("GRP not found");
            }

            auto body = response.extract_json().get();
            auto midArray = body[U("m2m:grp")][U("mid")].as_array();

            std::vector<utility::string_t> midList;
            for (const auto &mid : midArray) {
                midList.push_back(mid.as_string());
            }

            return midList;
        });
}

pplx::task<void> OneM2MClient::ensureSubscription(const utility::string_t &subName) {
    setRI(U("retrieve_sub"));
    setContentType(U("sub"));
    return sendGetRequest(utility::string_t(U(CSE_URL)) + U("/") + U(CSE_BASE) + U("/") + subName)
        .then([this, subName](web::http::http_response response) {
            if (response.status_code() == 200) {
                std::wcout << L"Subscription already exists: " << subName.c_str() << std::endl;
                return pplx::task_from_result();
            } else if (response.status_code() == 404) {
                std::wcout << L"Subscription does not exist. Creating: " << subName.c_str() << std::endl;

                web::json::value payload;
                payload[U("m2m:sub")][U("rn")] = web::json::value::string(subName);
                payload[U("m2m:sub")][U("nu")] = web::json::value::array({ web::json::value::string(U(SUB_NOTIFY_URL)) });
                payload[U("m2m:sub")][U("enc")] = web::json::value::object({ { U("net"), web::json::value::array({ web::json::value::number(3), web::json::value::number(4) }) } });
                //payload[U("m2m:sub")][U("nct")] = web::json::value::number(2);
                setRI(U("create_sub"));
                return sendPostRequest(utility::string_t(U(CSE_URL)) + U("/") + U(CSE_BASE), payload)
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

web::json::value OneM2MClient::discoveryCIN(const utility::string_t &cntName) {
    setRI(U("retrieve_cin"));
    setContentType(U("cin"));
    web::http::http_response response = sendGetRequest(utility::string_t(U(CSE_URL)) + U("/") + cntName + U("/la")).get();
    if (response.status_code() == 200) {
        return response.extract_json().get();
    } else {
        std::cerr << "Failed to retrieve CIN for: " << utility::conversions::to_utf8string(cntName) << " (HTTP " << response.status_code() << ")" << std::endl;
        return web::json::value();
    }
}

std::pair<std::string, std::vector<double>> OneM2MClient::parseBody(const web::json::value& body) {
    std::string label;
    std::vector<double> con_values;
    try {
        web::json::value cin = body.at(U("m2m:cin"));

        auto lbl_array = cin.at(U("lbl")).as_array();
        label = utility::conversions::to_utf8string(lbl_array[1].as_string());

        std::string con_str = utility::conversions::to_utf8string(cin.at(U("con")).as_string());
        if (!con_str.empty() && con_str.front() == '[' && con_str.back() == ']') {
            con_str = con_str.substr(1, con_str.size() - 2);
        }
        std::stringstream ss(con_str);
        double num;
        char ch;
        while (ss >> num) {
            con_values.push_back(num);
            ss >> ch;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON body: " << e.what() << std::endl;
    }
    return {label, con_values};
}

void OneM2MClient::startNotificationServer(std::promise<void>& serverStarted) {
    utility::string_t listenURL = U("http:") U(SUB_NOTIFY_URL);
    auto listener = std::make_shared<web::http::experimental::listener::http_listener>(listenURL);

    listener->support(web::http::methods::POST, [this](web::http::http_request request) {
        request.extract_json().then([this, request](web::json::value body) {
            std::wcout << L"Notification received:" << std::endl;
            request.reply(web::http::status_codes::OK, body);
            std::wcout << body.serialize() << std::endl;
            parseBody(body);
        }).wait();
    });

    listener->open().then([&serverStarted]() {
        std::wcout << L"Notification server started at: http:" << U(SUB_NOTIFY_URL) << std::endl;
        serverStarted.set_value();  // 서버가 시작되었음을 알림
    }).wait();

    std::thread([listener]() {
        while (true) {
            std::this_thread::yield();
        }
    }).detach();
}

pplx::task<web::http::http_response> OneM2MClient::sendPostRequest(const utility::string_t &url, const web::json::value &payload) {
    try {
        web::http::client::http_client client(url);
        web::http::http_request request(web::http::methods::POST);
        request.headers() = headers;
        request.set_body(payload);

        return client.request(request);
    } catch (const std::exception &e) {
        std::cerr << "sendPostRequest error: " << e.what() << std::endl;
        return pplx::task_from_exception<web::http::http_response>(std::runtime_error("sendPostRequest failed"));
    }
}

pplx::task<web::http::http_response> OneM2MClient::sendGetRequest(const utility::string_t &url) {
    try {
        web::http::client::http_client client(url);
        web::http::http_request request(web::http::methods::GET);
        request.headers() = headers;

        return client.request(request);
    } catch (const std::exception &e) {
        std::cerr << "sendGetRequest error: " << e.what() << std::endl;
        return pplx::task_from_exception<web::http::http_response>(std::runtime_error("sendGetRequest failed"));
    }
}