#ifndef _DEVE_UI_SERVER_H
#define _DEVE_UI_SERVER_H

#include <algorithm>
#include <string>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <JSON.h>

#include <RequestGenerator.h>

class DeveUIServer {
public:
    void setUpUIServer(Pistache::Address addr);
    void start();

    std::string adsIP;
    RequestGenerator rg;
    UserArbitration ua;

private:
    void setupRoutes();
    void doAuth(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getDownloadedImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserList(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    std::shared_ptr<Pistache::Http::Endpoint> http_endpoint_ = NULL;
    Pistache::Rest::Router router_;

    void reg(std::string userName, std::stirng password);
    void authenticate(std::string userName, std::stirng password);

    std::string fetchUserImage(std::string user, std::string image);
    nlohmann::json fetchUserImages(std::string user);
    nlohmann::json fetchUsers();
};

#endif
