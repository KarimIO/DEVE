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
    // Frontend Hispter Stuff
    void setupRoutes();
    void doAuth(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getDownloadedImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserList(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void postImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void handleSignUp(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void handleSignIn(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void handleRequestImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    std::shared_ptr<Pistache::Http::Endpoint> http_endpoint_ = NULL;
    Pistache::Rest::Router router_;

    // Real development
    bool signUp(std::string userName, std::string password);
    bool signIn(std::string userName, std::string password);
    void signOut(); // Deprecated, do not use.

    JSON fetchUsers();

    JSON fetchUserImages(std::string user);
    JSON fetchUserImage(JSON id);

    void requestImage(JSON id);
    JSON fetchPendingRequests();
    void setImageAccess(JSON id, std::string targetUser, int views);
    
    void serveCLI();

public:
    DeveUIServer(std::string adsIP);

    void startServers(Pistache::Address addr);

    std::string adsIP;
    RegistrarArbitration ra;
};

#endif
