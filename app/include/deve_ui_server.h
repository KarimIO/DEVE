#ifndef _DEVE_UI_SERVER_H
#define _DEVE_UI_SERVER_H

#include <algorithm>
#include <string>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <JSON.h>

#include <Dispatcher.h>

#include "RegistrarArbitration.h"
#include "Constants.h"

class DeveUIServer {
public:
    void setupRoutes();
    void doAuth(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getDownloadedImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserList(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void postImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void handleSignUp(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void handleSignIn(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    std::shared_ptr<Pistache::Http::Endpoint> http_endpoint_ = NULL;
    Pistache::Rest::Router router_;

    bool signUp(std::string userName, std::string password);
    bool signIn(std::string userName, std::string password);
    void signOut();

    JSON fetchUserImage(JSON id);
    JSON fetchPendingRequests(std::string id);
    nlohmann::json fetchUserImages(std::string user);
    nlohmann::json fetchUsers();
    void serveCLI();
    DeveUIServer(std::string adsIP);
    void setUpUIServer(Pistache::Address addr);
    void start();

    std::string adsIP;
    RegistrarArbitration ra;
};

#endif
