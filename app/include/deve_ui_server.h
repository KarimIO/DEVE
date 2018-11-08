#ifndef _DEVE_UI_SERVER_H
#define _DEVE_UI_SERVER_H

#include <algorithm>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

class DeveUIServer {
public:
    void setUpUIServer(Pistache::Address addr);
    void start();
    ~DeveUIServer();
private:
    void setupRoutes();
    void doAuth(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getDownloadedImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void getUserList(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    std::shared_ptr<Pistache::Http::Endpoint> http_endpoint_ = NULL;
    Pistache::Rest::Router router_;
};

#endif
