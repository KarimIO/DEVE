#include <deve_ui_server.h>
#include <deve_peer.h>
#include <JSON.h>
#include <fstream>
#include <vector>
#include <base64.h>

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
    Routes::Get(router_, "/images/", Routes::bind(&DeveUIServer::getUserImages, this));
    Routes::Get(router_, "/userlist", Routes::bind(&DeveUIServer::getUserList, this));
}

void DeveUIServer::doAuth(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    json j = {
        {"success"}
    };
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.send(Pistache::Http::Code::Ok, j.dump());
}

void DeveUIServer::getDownloadedImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "DEVE DownloadedImages Reached.");
}

void DeveUIServer::getUserImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    std::ifstream infile("tests/input.jpeg");
    if (infile.fail()) {
        std::cerr << "FAIL\n";
        return; 
    }

    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);

    std::vector<char> buffer;
    buffer.resize(length);
    infile.read((char*)&buffer[0], length);

    infile.close();
    std::string b64 = base64_encode((const unsigned char *)&buffer[0], length);

    response.send(Pistache::Http::Code::Ok, b64);
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
