
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
        .flags(Tcp::Options::InstallSignalHandler)
        .maxPayload(SIZE_MAX);

    http_endpoint_ = std::shared_ptr<Pistache::Http::Endpoint>(new Http::Endpoint(addr));
    setupRoutes();
    http_endpoint_->init(opts);
    http_endpoint_->setHandler(router_.handler());
    http_endpoint_->serve();
}

void DeveUIServer::setupRoutes() {
    using namespace Rest;

    Routes::Get(router_, "/checkAuth", Routes::bind(&DeveUIServer::doAuth, this));
    Routes::Get(router_, "/downloaded", Routes::bind(&DeveUIServer::getDownloadedImages, this));
    Routes::Get(router_, "/images/:id", Routes::bind(&DeveUIServer::getUserImages, this));
    Routes::Get(router_, "/image/:ownerID/:id/:time", Routes::bind(&DeveUIServer::getUserImage, this));
    Routes::Get(router_, "/userlist", Routes::bind(&DeveUIServer::getUserList, this));
    Routes::Post(router_, "/image", Routes::bind(&DeveUIServer::postImage, this));
    Routes::Post(router_, "/signup", Routes::bind(&DeveUIServer::handleSignUp, this));
    Routes::Post(router_, "/signin", Routes::bind(&DeveUIServer::handleSignIn, this));
}

void DeveUIServer::signOut() {
    ra.logout();
}

void DeveUIServer::doAuth(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto id = RRAD::Dispatcher::singleton.getUID();

    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.send(Pistache::Http::Code::Ok, id);
}

void DeveUIServer::getDownloadedImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "DEVE Downloaded Images Reached.");
}

void DeveUIServer::getUserImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto owner = request.param(":ownerID").as<std::string>();
    auto id = request.param(":id").as<std::string>();
    auto time = request.param(":time").as<uint32>();

    JSON json;
    json["ownerID"] = owner;
    json["unixTimestamp"] = time;
    json["id"] = id;
    json["class"] = "Image";

    std::cout << json.dump() << "\n";

    try {
        auto image_b64 = fetchUserImage(json.dump());
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Pistache::Http::Code::Ok, image_b64);
    } catch (const char* err) {
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Pistache::Http::Code::Forbidden, err);
    }
}

void DeveUIServer::getUserImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto user = request.param(":id").as<std::string>();

    auto images = fetchUserImages(user);
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.send(Pistache::Http::Code::Ok, images.dump());
}

void DeveUIServer::postImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto body = request.body();
    
    auto found = body.find(';');
    if (found != std::string::npos) {
        auto img = body.substr(0, found);
        auto thumb = body.substr(found + 1, body.size() - found - 1);

        new Image(img, thumb);
        
        response.send(Pistache::Http::Code::Ok, "imageUpload.succ");
        return;
    }
    
    response.send(Pistache::Http::Code::Internal_Server_Error, "imageUpload.feel");
}

void DeveUIServer::getUserList(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.send(Pistache::Http::Code::Ok, ra.getList().dump());
}

void DeveUIServer::handleSignUp(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto body = request.body();

    std::cout << "Sign Up\n";
    
    auto found = body.find(';');
    if (found != std::string::npos) {
        auto user = body.substr(0, found);
        auto pass = body.substr(found + 1, body.size() - found - 1);

        std::cout << user << " " << pass << "\n";
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        
        try {
            if (signUp(user, pass)) {
                response.send(Pistache::Http::Code::Ok, "auth.success");
            }
            else {
                response.send(Pistache::Http::Code::Not_Found, "auth.notValid");
            }
        }
        catch(const char *e) {
            std::cerr << e << "\n";
            response.send(Pistache::Http::Code::Internal_Server_Error, e);
        }
    }
    else {
        response.send(Pistache::Http::Code::No_Content, "auth.notWorking");
    }
}

void DeveUIServer::handleSignIn(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto body = request.body();

    std::cout << "Sign In\n";
    
    auto found = body.find(';');
    if (found != std::string::npos) {
        auto user = body.substr(0, found);
        auto pass = body.substr(found + 1, body.size() - found - 1);

        std::cout << user << " " << pass << "\n";
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        
        try {
            if (signIn(user, pass)) {
                response.send(Pistache::Http::Code::Ok, "auth.success");
            }
            else {
                response.send(Pistache::Http::Code::Not_Found, "auth.notFound");
            }
        }
        catch(const char *e) {
            std::cerr << e << "\n";
            response.send(Pistache::Http::Code::Internal_Server_Error, e);
        }
    }
    else {
        response.send(Pistache::Http::Code::No_Content, "auth.notWorking");
    }
}

bool DeveUIServer::signUp(std::string userName, std::string password) {
    RRAD::Dispatcher::singleton.setUID(userName);
    auto r = ra.reg(password);
    if (r) {
        return ra.authenticate(password);
    }

    return r;
}

bool DeveUIServer::signIn(std::string userName, std::string password) {
    RRAD::Dispatcher::singleton.setUID(userName);
    return ra.authenticate(password);
}

JSON DeveUIServer::fetchUserImages(std::string user) {
    return Image::getList(&ra, user);
}

JSON DeveUIServer::fetchUserImage(std::string id) {
    return Image::getImage(&ra, id);
}