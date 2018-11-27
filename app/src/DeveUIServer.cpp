#include "DeveUIServer.h"
#include "TweetNACL.h"
#include "Image.h"
#include "DumpFile.h"

#include <base64.h>

#include <fstream>
#include <vector>

#include <JSON.h>

#define RDS RRAD::Dispatcher::singleton

using namespace Pistache;

// Static Helpers
static void deathRoutine() {
    // Save here
    std::cerr << "Terminating..." << std::endl;
    exit(0);
}

static std::string base64ImageFromFile(std::string img_path) {
    std::ifstream infile(img_path);
    if (infile.fail() || !infile.is_open())
        throw "invalid.path.for.image";

    std::vector<uint8> buffer;

    infile.seekg(0, infile.end);
    size_t len = infile.tellg();
    infile.seekg(0, infile.beg);

    buffer.resize(len);
    infile.read((char*)&buffer[0], len);

    infile.close();

    return base64_encode((unsigned char const *)&buffer[0], buffer.size());
}

static std::vector<std::string> split(std::string s) {
    std::stringstream ss(s);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    return vstrings;
}


DeveUIServer::DeveUIServer(std::string adsIP): adsIP(adsIP), ra(adsIP) {
}


void DeveUIServer::startServers(Pistache::Address addr) {
    auto opts = Http::Endpoint::options()
        .threads(2)
        .flags(Tcp::Options::InstallSignalHandler)
        .maxPayload(SIZE_MAX);

    http_endpoint_ = std::shared_ptr<Pistache::Http::Endpoint>(new Http::Endpoint(addr));
    setupRoutes();
    http_endpoint_->init(opts);
    http_endpoint_->setHandler(router_.handler());
    std::cout << "[DEVE] GUI server started." << std::endl;
    auto cli_thread = std::thread([&]() {
            serveCLI();
    });
    cli_thread.detach();

    http_endpoint_->serve();

}
void DeveUIServer::serveCLI() {
    std::cout << "(Ctrl+D to exit)" << std::endl;
    std::cout << "[DEVE] CLI server started." << std::endl;

    std::string cmd;
    while (std::cout << "> ", std::getline(std::cin, cmd)) {
        if (cmd.empty()) {
            continue;
        }
        std::vector<std::string> args = split(cmd);
        std::string func = args[0];
        if (func == "lsusr") {
            JSON userList = fetchUsers();
            std::cout << userList;
        } else if (func == "signup") {
            if (args.size() <= 2) {
                std::cerr << "nope: " << 2 << std::endl;
                continue;
            }

            signUp(args[1], args[2]);
        } else if (func == "signin") {
            if (args.size() <= 2) {
                std::cerr << "nope: " << 2 << std::endl;
                continue;
            }

            signIn(args[1], args[2]);
        } else if (func == "signout") {
            signOut();
        } else if (func == "upload") {
            if (args.size() <= 1) {
                std::cerr << "nope: " << 1 << std::endl;
                continue;
            }
            std::string img_path = args[1];

            auto img = base64ImageFromFile(img_path);
            auto thumb = ""; //meh // big mood
            new Image(img_path, img, thumb);
            std::cout << "Image created" << std::endl;
        } else if (func == "lsimg") {
            if (args.size() <= 1) {
                std::cerr << "nope: " << 1 << std::endl;
                continue;
            }

            std::cout << fetchUserImages(args[1]);
        } else if (func == "getimg") { //getimg user timestamp id
            if (args.size() <= 3) {
                std::cerr << "nope: " << 3 << std::endl;
                continue;
            }
            JSON id;
            id["class"] = "Image";
            id["ownerID"] = args[1];
            id["unixTimestamp"] = stoll(args[2]);
            id["id"] = stoi(args[3]);

            try {
                dumpIntoFile("./output.jpg", base64_decode(Image::getImageData(&ra, id)));
                std::cout << "View the image at ./output.jpg" << std::endl; // CLI ONLY!! GUI SHOULD NEVER STORE THE IMAGE IN PLAIN BINARY EVER EVER EVER EVER
            } catch (const char * e) {
                std::cerr << "[DEVE] Error: " << e << std::endl;
            }
        } else if (func == "reqimg") {
            if (args.size() <= 3) {
                std::cerr << "nope: " << 3 << std::endl;
                continue;
            }
            JSON id;
            id["class"] = "Image";
            id["ownerID"] = args[1];
            id["unixTimestamp"] = stoll(args[2]);
            id["id"] = stoi(args[3]);

            try {
                requestImage(id);
                std::cout << "Requested." << std::endl;
            } catch (const char * e) {
                std::cerr << "[DEVE] Error: " << e << std::endl;
            }
        } else if (func == "getreqs") {
            std::cout << fetchPendingRequests() << std::endl;
        } else if (func == "grant") { //grant user num_access timestamp id
            if (args.size() <= 4) {
                std::cerr << "nope: " << 4 << std::endl;
                continue;
            }
            std::string user = args[1];
            int views = std::stoi(args[2]);

            JSON id;
            id["class"] = "Image";
            id["ownerID"] = RDS.getUID();
            id["unixTimestamp"] = stoll(args[3]);
            id["id"] = stoi(args[4]);

            try {
                setImageAccess(id, user, views);
            } catch (const char* e) {
                std::cerr << "[DEVE] Error: " << e << std::endl;
            }
            std::cout << "[DEVE] Granted." << std::endl;
        }
        std::cout << std::endl;
    }
    deathRoutine();
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

void DeveUIServer::doAuth(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    try {
        auto id = RDS.getUID();
        response.send(Pistache::Http::Code::Ok, id);
    } catch (const char* err) {
        response.send(Pistache::Http::Code::Forbidden, err);
    }
}

void DeveUIServer::getDownloadedImages(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");

    try {
        response.send(Pistache::Http::Code::Ok, "DEVE Downloaded Images Reached.");
    } catch (const char* err) {
        response.send(Pistache::Http::Code::Forbidden, err);
    }
}

void DeveUIServer::getUserImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto owner = request.param(":ownerID").as<std::string>();
    auto id = request.param(":id").as<uint32>();
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

    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    try {
    auto images = fetchUserImages(user);
        response.send(Pistache::Http::Code::Ok, images.dump());
    } catch (const char* err) {
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Pistache::Http::Code::Forbidden, err);
    }
}

void DeveUIServer::postImage(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto body = request.body();

    auto found = body.find(';');
    try {
        if (found != std::string::npos) {
            auto img = body.substr(0, found);
            auto thumb = body.substr(found + 1, body.size() - found - 1);

            new Image("new_image", img, thumb);

            response.send(Pistache::Http::Code::Ok, "imageUpload.succ");
        }
        else {
            throw "imageUpload.invalidSyntax";
        }
    } catch (const char* err) {
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Pistache::Http::Code::Forbidden, err);
    }
}

void DeveUIServer::getUserList(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    try {
        auto id = RDS.getUID();
        auto list = ra.getList();
        list.erase(id);
        response.send(Pistache::Http::Code::Ok, list.dump());
    } catch (const char* err) {
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Pistache::Http::Code::Forbidden, err);
    }
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

// MARK: Real development
bool DeveUIServer::signUp(std::string userName, std::string password) {
    RDS.setUID(userName);
    auto r = ra.reg(password);
    if (r) {
        return ra.authenticate(password);
    }

    return r;
}

bool DeveUIServer::signIn(std::string userName, std::string password) {
    RDS.setUID(userName);
    return ra.authenticate(password);
}

void DeveUIServer::signOut() {
    ra.logout();
}


JSON DeveUIServer::fetchUsers() {
    return ra.getList();
}


JSON DeveUIServer::fetchUserImages(std::string user) {
    return Image::getList(&ra, user);
}

JSON DeveUIServer::fetchUserImage(JSON id) {
    return Image::getImageData(&ra, id);
}

static inline Image* getImageObject(JSON id) {
    Image* ptr = (Image*)RDS.getObject(id);
    if (ptr == nullptr) {
        throw "image.doesNotExist";
    }
    return ptr;
}

void DeveUIServer::requestImage(JSON id) {
    if (id["ownerID"] == RDS.getUID()) {
        return; // This is stupid
    }
    auto& image = *getImageObject(id);
    image.requestAccess(&ra);

}

JSON DeveUIServer::fetchPendingRequests() {
    JSON requests = JSON::array();
    for (int i = 0; i < Image::requests.size(); i++) {
        auto& front = Image::requests.front();

        requests.push_back({ {"image", front.first->getID() }, {"user", front.second}});
        Image::requests.pop();
    }
    return requests;
}

void DeveUIServer::setImageAccess(JSON id, std::string targetUser, int views) {
    auto& image = *getImageObject(id);
    image.setAccess(&ra, targetUser, views);
}