#include "deve_ui_server.h"
#include "TweetNACL.h"
#include "Image.h"
#include "base64.h"

#include <fstream>
#include <vector>

#include <JSON.h>

using namespace Pistache;

void deathRoutine() {
    // Save here
    std::cerr << "Terminating..." << std::endl;
    exit(0);
}

std::string base64ImageFromFile(std::string img_path) {
    std::ifstream infile(img_path);
    if (infile.fail() || !infile.is_open())
        throw "invalid.path.for.image";

    std::vector<uint8> buffer;

    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);

    buffer.resize(length);
    infile.read((char*)&buffer[0], length);

    infile.close();

    return base64_encode((unsigned char const *)&*buffer.begin(), buffer.size());
}

DeveUIServer::DeveUIServer(std::string adsIP): adsIP(adsIP), ra(adsIP) {
}

std::vector<std::string> split(std::string s) {
    std::stringstream ss(s);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    return vstrings;
}

void DeveUIServer::serveCLI() {
    std::cout << "(Ctrl+D to exit)" << std::endl;
    std::cerr << "CLI Server Started" << std::endl;

    std::string cmd;
    while (std::cout << "> ", std::getline(std::cin, cmd)) {
        if (cmd.empty()) {
            continue;
        }
        std::vector<std::string> args = split(cmd);
        std::string func = args[0];
        if (func == "lsusr") {
            JSON userList = ra.getList();
            std::cout << userList;
        } else if (func == "signup") {
            if (args.size() < 2) {
                std::cerr << "nope: " << 2 << std::endl;
                continue;
            }

            signUp(args[1], args[2]);
        } else if (func == "signin") {
            if (args.size() < 2) {
                std::cerr << "nope: " << 2 << std::endl;
                continue;
            }

            signIn(args[1], args[2]);
        } else if (func == "signout") {
            signOut();
        } else if (func == "upload") {
            if (args.size() < 1) {
                std::cerr << "nope: " << 1 << std::endl;
            }
            std::string img_path = args[1];

            auto img = base64ImageFromFile(img_path);
            auto thumb = ""; //meh
            new Image(img, thumb);
            std::cout << "Image created" << std::endl;
        } else if (func == "lsimg") {
            if (args.size() < 1) {
                std::cerr << "nope: " << 1 << std::endl;
            }

            std::cout << fetchUserImages(args[1]);
        } else if (func == "getimg") {
            if (args.size() < 1) {
                std::cerr << "nope: " << 1 << std::endl;
            }

            std::cout << fetchUserImages(args[1]);
        }


        std::cout << std::endl;
    }
    deathRoutine();
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
    std::cerr << "UI Server Started" << std::endl;
    auto cli_thread = std::thread([&]() {
            serveCLI();
    });
    cli_thread.detach();

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
        auto image_b64 = fetchUserImage(json);
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

    auto found = body.find(';');
    if (found != std::string::npos) {
        auto user = body.substr(0, found);
        auto pass = body.substr(found + 1, body.size() - found - 1);
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

JSON DeveUIServer::fetchUserImage(JSON id) {
    return Image::getImage(&ra, id);
}

JSON DeveUIServer::fetchPendingRequests(std::string id) {
    JSON idObj(id);
    if (idObj["ownerID"] != RRAD::Dispatcher::singleton.getUID())
        throw "image.not.owned";
    auto& image = *(Image*)RRAD::Dispatcher::singleton.getObject(id);
    JSON requests = JSON::array();
    for (int i = 0; i < image.requests.size(); i++) {
        requests.push_back(image.requests.front().second);
        image.requests.pop();
    }
    return requests;
}
