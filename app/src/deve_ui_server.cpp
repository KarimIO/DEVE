
#include "deve_ui_server.h"
#include "TweetNACL.h"

#include <fstream>
#include <vector>
#include <base64.h>
#include <JSON.h>

using namespace Pistache;
using namespace nlohmann;

DeveUIServer::DeveUIServer(std::string adsIP): adsIP(adsIP) {
    rg = RRAD::RequestGenerator(INIT_USERNAME);
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
    rg = RRAD::RequestGenerator(userName);
    crypto_box_keypair(publicKey, privateKey);
    std::string pubKeyString = base64_encode(publicKey);
    if (!ua.reg(&rg, password, pubKeyString)) {
        throw "auth.registrationFail";
    }
}

void DeveUIServer::authenticate(std::string userName, std::string password) {
    if (!ua.authenticate(&rg, "password")) {
        throw "auth.fail";
    }

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

void DeveUIServer::getUserList(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.send(Pistache::Http::Code::Ok, fetchUsers().dump());
}

json DeveUIServer::fetchUserImages(std::string user) {
    if (user == "karimah") {
        return {
            {
                {"name", "input"},
                {"views", 4},
                {"thumb", fetchUserImage(user, "input_thumb")}
            }, {
                {"name", "bee"},
                {"views", 4},
                {"thumb", fetchUserImage(user, "bee_thumb")}
            }
        };
    }
    else {
        return json::array();
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

json DeveUIServer::fetchUsers() {
    return {
        "ahmedghazy",
        "alikhaled",
        "amrkadi",
        "donn",
        "karimah"
    };
}