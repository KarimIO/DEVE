#ifndef _DEVE_UI_SERVER_H
#define _DEVE_UI_SERVER_H

#include <algorithm>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

class DevePeer;

class DeveUIServer {
public:
    DeveUIServer(DevePeer *Peer);
    void setUpUIServer(Pistache::Address addr);
    void start();
    ~DeveUIServer();
private:
    void setupRoutes();
    void doAuth(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getDownloadedImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserList(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    Pistache::Http::Endpoint *http_endpoint_;
    Pistache::Rest::Router router_;

    DevePeer *peer_;
};

#endif