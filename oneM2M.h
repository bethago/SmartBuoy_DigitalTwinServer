#ifndef ONEM2M_H
#define ONEM2M_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <string>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class oneM2MClient {
public:
    oneM2MClient(const std::string& cseBase, const std::string& RVI);

    pplx::task<http_response sendPostRequest(const std::string& resource, const json::value& payload);

private:
  http_client client;
  std::string RVI;

  http_headers setHeaders();
};

#endif // ONEM2M_H
