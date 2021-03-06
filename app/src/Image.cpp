#include "Image.h"
#include "DumpFile.h"
#include "Constants.h"

#include <iostream>

#define RDS RRAD::Dispatcher::singleton

std::queue< std::pair<Image*, std::string> > Image::requests = std::queue< std::pair<Image*, std::string> >();
std::map<std::string, Image*> Image::pseudoDownloaded = std::map<std::string, Image*>();
std::mutex Image::psuedoDownloadedMutex;

static auto defaultJSON = R"(
{
    "ownerID": "__NOUSER",
    "data": "hecc",
    "title": "sorry",
    "thumb": "heccer",
    "views": 0,
    "access": {
    }
}
)"_json;


JPEG Image::generateSteganogramJPEG() {
    static auto data = dumpFile("rsc/input.jpg");
    static JPEG example = JPEG::fromByteVector(&data);
    return example;
}

Image::Image(std::string title, std::string base64, std::string thumbBase64) {
    static int counter = 0;

    img_json = defaultJSON;
    img_json["ownerID"] = Dispatcher::singleton.getUID();
    img_json["data"] = base64;
    img_json["thumb"] = thumbBase64;
    img_json["title"] = title;

    id = EmptyJSO;
    id["ownerID"] = Dispatcher::singleton.getUID();
    id["unixTimestamp"] = counter + 5; // (uint32)time(NULL); // Got annoying to debug
    id["id"] = counter++;
    id["class"] = "Image";

    RDS.registerObject(this);
}

Image::Image(JSON id, bool owned, JSON img_json) {
    static int counter = 0;
    this->id = id;
    this->img_json = img_json;
    RDS.registerObject(this, owned);
}

void Image::recordAccessChange(std::string targetUser, int view_cnt) {
    auto idStr = id.dump();
    std::lock_guard lg(psuedoDownloadedMutex);
    if (targetUser == RDS.getUID()) {
        if (view_cnt <= 0) {
            if (pseudoDownloaded.find(idStr) != pseudoDownloaded.end()) {
                pseudoDownloaded.erase(idStr);
            }
        } else if (view_cnt > 0) {
            pseudoDownloaded[idStr] = this;
        }
    }
    img_json["access"][targetUser] = view_cnt;
}

void Image::addRequest(std::string requester) {
    // Q: why do we need the id?
    // A: Image::requests is a global object. You need the image's id beyond this.
    Image::requests.push(std::pair(this, requester));
}

void Image::recordView(std::string viewer) {
    img_json["views"] = (int)img_json["views"] + 1;

    img_json["access"][viewer] = (int)img_json["access"][viewer] - 1;
    if (img_json["access"][viewer] < 0) {
        std::cout << "[DEVE] " << viewer << "'s being cheeky with image " << id.dump() << "." << std::endl;
        img_json["access"][viewer] = 0;
    }
}

std::vector<uint8> Image::getSteganogram() {
    auto hidable = Image::generateSteganogramJPEG();
    hidable.comment = img_json.dump();
    return hidable.toByteVector();
}

std::string Image::getSteganogramBase64() {
    auto steganogram = getSteganogram();
    return base64_encode(&steganogram[0], steganogram.size());
}

Image* Image::imageFromSteganogram(JSON id, std::vector<uint8> steganogram) {
    JPEG jpeg = JPEG::fromByteVector(&steganogram);
    JSON content = JSON::parse(jpeg.comment);
    auto owned = content["ownerID"] == RDS.getUID();
    return new Image(id, owned, content);
}

JSON Image::executeRPC(std::string name, JSON arguments) {
    if (name == "__setAccess") {
        recordAccessChange(arguments["targetUser"], arguments["viewCount"]);
        return EmptyJSO;
    } else if (name == "__requestAccess") {
        addRequest(arguments["RRAD::senderUserName"]);
        return EmptyJSO;
    } else if (name == "__sendView") {
        recordView(arguments["RRAD::senderUserName"]);
        return EmptyJSO;
    } else if (name == "getImage") {
        return getSteganogramBase64();
    }
    throw "rpc.unknownMethod";
}

JSON Image::getList(RegistrarArbitration* ra, std::string user) {
    JSON array = JSON::array();
    if (user == RDS.getUID()) {
        auto images = RDS.listMine("Image");
        std::for_each(images.begin(), images.end(), [&](RemoteObject* ro){
            auto& image = *(Image*)ro;
            array.push_back(image.getMetadata());
        });
    } else {
        auto listRequest = RDS.listRPC("Image", user);
        auto ip = ra->getUserIP(user);
        if (!ip.has_value()) {
            throw "user.doesNotExist";
        }
        auto idList = RDS.communicateRMI(ip.value(), REQ_PORT, listRequest);
        for (auto& object: idList["result"]) {
            auto rmiReqMsg = RDS.rmiReqMsg("Image", object["ownerID"], object, "getImage", EmptyJSO);
            auto reply = RDS.communicateRMI(ip.value(), REQ_PORT, rmiReqMsg);
            if (reply.find("error") != reply.end()) {
                std::cout << "[DEVE] Skipping deleted image " << object.dump() << "..." << std::endl;
                continue;
            }

            auto steganogram = base64_decode(reply["result"]);
            auto& image = *imageFromSteganogram(object, steganogram);
            JSON json = image.getMetadata();

            

            array.push_back(json);
        }
    }
    return array;
}

JSON Image::getMetadata() {
    JSON json;
    json["object"] = getID();
    json["thumb"] = img_json["thumb"];
    json["title"] = img_json["title"];
    json["views"] = img_json["views"];

    if (id["ownerID"] != RDS.getUID()) {
        JSON access = img_json["access"][RDS.getUID()];
        if (access.is_null()) {
            access = 0;
        }
        json["access"] = access;
    }

    return json;
}

std::string Image::getImageData(RegistrarArbitration* ra, JSON id) {
    auto& owner = id["ownerID"];
    auto self = RDS.getUID();
    auto ptr = (Image*)(RDS.getObject(id));
    if (!ptr) {
        throw "image.notFound";
    }
    auto& image = *ptr;
    if (owner == self) {
        return image.img_json["data"];
    }
    if (image.img_json["access"][self] > 0) {
        image.recordView(self);
        auto request = RDS.rmiReqMsg("Image", owner, id, "__sendView", EmptyJSO);
        auto ip = ra->getUserIP(owner);
        if (!ip.has_value()) {
            throw "user.offline";
        }
        
        RDS.communicateRMI(ip.value(), REQ_PORT, request);

        return image.img_json["data"];
    }
    throw "image.unauthorized";
}

void Image::setAccess(RegistrarArbitration* ra, std::string targetUser, int views) {
    // You need to set access both locally and remotely.
    recordAccessChange(targetUser, views);

    auto rmiReqMsg = RDS.rmiReqMsg("Image", targetUser, id, "__setAccess", JSON({
            {"viewCount", views},
            {"targetUser", targetUser}
        })
    );

    auto ip = ra->getUserIP(targetUser);
    if (!ip.has_value()) {
        throw "user.doesNotExist";
    }
    
    RDS.communicateRMI(ip.value(), REQ_PORT, rmiReqMsg);
}

void Image::requestAccess(RegistrarArbitration* ra) {
    auto rmiReqMsg = RDS.rmiReqMsg("Image", img_json["ownerID"], id, "__requestAccess", EmptyJSO);

    auto ip = ra->getUserIP(img_json["ownerID"]);
    if (!ip.has_value()) {
        throw "user.doesNotExist";
    }

    RDS.communicateRMI(ip.value(), REQ_PORT, rmiReqMsg);
}

void Image::destroyImage(RegistrarArbitration* ra, Image* image) {
    auto accessRights = image->img_json["access"];
    auto id = image->id;

    RDS.destroyObject(image->id); // After this point, the Image pointer will NOT be pointing to anything!! Take all the data you need out of it first.

    for (auto iterator: accessRights.items()) {
        std::thread tr([=](){
            auto targetUser = iterator.key();
            auto rmiReqMsg = RDS.rmiReqMsg("Image", targetUser, id, "__setAccess", JSON({
                    {"viewCount", 0},
                    {"targetUser", targetUser}
                })
            );

            auto ip = ra->getUserIP(targetUser);
            if (!ip.has_value()) {
                throw "user.doesNotExist";
            }
            
            RDS.communicateRMI(ip.value(), REQ_PORT, rmiReqMsg);
        });
        tr.detach();
    }
}

void Image::destroy(RegistrarArbitration* ra) {
    std::thread tr([=]() {
        destroyImage(ra, this);
    });
    tr.detach();
}