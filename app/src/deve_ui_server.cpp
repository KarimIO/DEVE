
#include "deve_ui_server.h"
#include "TweetNACL.h"
#include "Image.h"
#include "base64.h"

#include <fstream>
#include <vector>

#include <JSON.h>

using namespace Pistache;

DeveUIServer::DeveUIServer(std::string adsIP): adsIP(adsIP), ra(adsIP) {
}

void DeveUIServer::setUpUIServer(Pistache::Address addr) {
    auto opts = Http::Endpoint::options()
        .threads(2)
        .flags(Tcp::Options::InstallSignalHandler);

    http_endpoint_ = std::shared_ptr<Pistache::Http::Endpoint>(new Http::Endpoint(addr));
    setupRoutes();
    http_endpoint_->init(opts);
    http_endpoint_->setHandler(router_.handler());
    http_endpoint_->serve();
}

void DeveUIServer::setupRoutes() {
    using namespace Rest;

    Routes::Get(router_, "/auth", Routes::bind(&DeveUIServer::doAuth, this));
    Routes::Get(router_, "/downloaded", Routes::bind(&DeveUIServer::getDownloadedImages, this));
    Routes::Get(router_, "/images/:id", Routes::bind(&DeveUIServer::getUserImages, this));
    Routes::Get(router_, "/images/:id/:img", Routes::bind(&DeveUIServer::getUserImage, this));
    Routes::Get(router_, "/userlist", Routes::bind(&DeveUIServer::getUserList, this));
}

void DeveUIServer::reg(std::string userName, std::string password) {
    RRAD::Dispatcher::singleton.setUID(userName);
    if (!ra.reg(password)) {
        throw "auth.registrationFail";
    }
}

void DeveUIServer::authenticate(std::string userName, std::string password) {
    RRAD::Dispatcher::singleton.setUID(userName);
    if (!ra.authenticate(password)) {
        throw "auth.fail";
    }
}

void DeveUIServer::doAuth(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    JSON j = {
        {"success"}
    };
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.send(Pistache::Http::Code::Ok, j.dump());
}

void DeveUIServer::getDownloadedImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "DEVE DownloadedImages Reached.");
}

void DeveUIServer::getUserImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto user = request.param(":id").as<std::string>();
    auto image = request.param(":img").as<std::string>();

    auto image_b64 = fetchUserImage(user, image);
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.send(Pistache::Http::Code::Ok, image_b64);
}

void DeveUIServer::getUserImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto user = request.param(":id").as<std::string>();

    auto images = fetchUserImages(user);
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.send(Pistache::Http::Code::Ok, images.dump());
}

JSON DeveUIServer::fetchUsers() {
    JSON returnValue = {};
    for (auto user: ra.localRegistry) {
        returnValue[user.first] = user.second.ip;
    }
    return returnValue;
}

void DeveUIServer::getUserList(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.send(Pistache::Http::Code::Ok, fetchUsers().dump());
}

JSON DeveUIServer::fetchUserImages(std::string user) {
    if (user == RRAD::Dispatcher::singleton.getUID()) {
        auto images = RRAD::Dispatcher::singleton.listMine("Image");
        JSON array = JSON::array();
        std::for_each(images.begin(), images.end(), [&](RemoteObject* ro){
            auto image = (Image*)ro;
            JSON json;
            json["id"] = image->id;
            json["data"] = image->img_json["thumb"];
            json["views"] = image->img_json["views"];
            array.push_back(json);
        });
        return array;
    }
    else {
        return JSON::array();
    }
}

std::string DeveUIServer::fetchUserImage(std::string user, std::string image) {
    std::string path = "tests/images/" + user + "/" + image + ".jpg";
    std::cout << "Fetching " << path << "\n";
    std::ifstream infile(path);
    if (infile.fail()) {
        std::cerr << "Failed to get Image.\n";
        return "";
    }

    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);

    std::vector<char> buffer;
    buffer.resize(length);
    infile.read((char*)&buffer[0], length);

    infile.close();
    return base64_encode((const unsigned char *)&buffer[0], length);
}