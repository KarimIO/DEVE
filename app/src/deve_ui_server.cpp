#include <deve_ui_server.h>
#include <deve_peer.h>
#include <JSON.h>

using namespace Pistache;
using namespace nlohmann;

DeveUIServer::DeveUIServer(DevePeer *peer) : peer_(peer) {

}

void DeveUIServer::setUpUIServer(Pistache::Address addr) {
    auto opts = Http::Endpoint::options()
        .threads(2)
        .flags(Tcp::Options::InstallSignalHandler);

    http_endpoint_ = new Http::Endpoint(addr);
    http_endpoint_->init(opts);
    setupRoutes();
}

void DeveUIServer::start() {
    http_endpoint_->setHandler(router_.handler());
    http_endpoint_->serve();
}

void DeveUIServer::setupRoutes() {
    using namespace Rest;

    Routes::Get(router_, "/auth", Routes::bind(&DeveUIServer::doAuth, this));
    Routes::Get(router_, "/downloaded", Routes::bind(&DeveUIServer::getDownloadedImages, this));
    Routes::Get(router_, "/images/:name/", Routes::bind(&DeveUIServer::getUserImages, this));
    Routes::Get(router_, "/userlist", Routes::bind(&DeveUIServer::getUserList, this));
}

void DeveUIServer::doAuth(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    json j = {
                {"success"}
            };
    response.send(Pistache::Http::Code::Ok, j.dump());
}

void DeveUIServer::getDownloadedImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "DEVE DownloadedImages Reached.");
}

void DeveUIServer::getUserImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "DEVE User Images List Reached.");
}

void DeveUIServer::getUserList(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    std::vector<std::string> list = peer_->getUserList();
    
    json j;
    for (auto &item : list) {
        j.push_back(item);
    }

    response.send(Pistache::Http::Code::Ok, j.dump());
}

DeveUIServer::~DeveUIServer() {
    if (http_endpoint_)
        http_endpoint_->shutdown();
}