#include "Image.h"
#include "DumpFile.h"
#include "Constants.h"

#include <iostream>

#define RDS RRAD::Dispatcher::singleton

std::queue< std::pair<std::string, std::string> > Image::requests = std::queue< std::pair<std::string, std::string> >();

static auto defaultJSON = R"(
{
    "ownerID": "__NOUSER",
    "data": "hecc",
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

Image::Image(std::string base64, std::string thumbBase64) {
    static int counter = 0;

    img_json = defaultJSON;
    img_json["ownerID"] = Dispatcher::singleton.getUID();
    img_json["data"] = base64;
    img_json["thumb"] = thumbBase64;

    id = JSON({});
    id["ownerID"] = Dispatcher::singleton.getUID();
    id["unixTimestamp"] = (uint32)time(NULL);
    id["id"] = counter++;
    id["class"] = "Image";

    RDS.registerObject(id, this);
}

Image::Image(JSON id, JSON content, bool owned) {
    static int counter = 0;
    img_json = content;
    RDS.registerObject(id, this, owned);
}

void Image::setAccess(std::string targetUser, uint32 view_cnt) {
    img_json["access"][targetUser] = view_cnt;
}

void Image::requestAccess(std::string requester) {
    //why do we need the id?
    Image::requests.push(std::pair(id.dump(), requester));
}

void Image::recordView(std::string viewer) {
    img_json["views"] = (uint32)img_json["views"] + 1;
    img_json["access"][viewer] = (uint32)img_json["access"][viewer] - 1;
    if (img_json["access"][viewer] < 0) {
        std::cout << "[DEVE] " << viewer << "'s being cheeky with image " << id.dump() << "." << std::endl;
        img_json["access"][viewer] = 0;
    }
}

JSON Image::getJSON() {
    return img_json;
}

std::vector<uint8> Image::getSteganogram() {
    auto hidable = Image::generateSteganogramJPEG();
    hidable.comment = getJSON().dump();
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
    return new Image(id, content, owned);
}

JSON Image::executeRPC(std::string name, JSON arguments) {
    if (name == "__setAccess") {
        setAccess(arguments["view_cnt"], arguments["targetUser"]);
        return JSON({});
    } else if (name == "__requestAccess") {
        requestAccess(arguments["ownerID"]);
        return JSON({});
    } else if (name == "__sendView") {
        recordView(arguments["ownerID"]);
        return JSON({});
    } else if (name == "getImage") {
        JSON reply;
        reply["result"] = getSteganogramBase64();
        return reply;
    }
    throw "rpc.unknownMethod";
}

JSON Image::getList(RegistrarArbitration* ra, std::string user) {
    JSON array = JSON::array();
    if (user == RDS.getUID()) {
        auto images = RDS.listMine("Image");
        std::for_each(images.begin(), images.end(), [&](RemoteObject* ro){
            auto image = *(Image*)ro;
            JSON json;
            json["id"] = image.id;
            json["thumb"] = image.img_json["thumb"];
            json["views"] = image.img_json["views"];
            array.push_back(json);
        });
    } else {
        auto listRequest = RDS.listRPC("Image", user);
        auto ip = ra->getUserIP(user);
        if (!ip.has_value()) {
            throw "user.doesNotExist";
        }
        auto idList = RDS.communicateRMI(ip.value(), REQ_PORT, listRequest);
        for (auto id: idList) {
            auto rmi = RDS.rmi("Image", id["ownerID"], id, "getImage", {});
            auto reply = RDS.communicateRMI(ip.value(), REQ_PORT, rmi);
            if (reply.find("error") != reply.end()) {
                std::cout << "[DEVE] Skipping deleted image " << id.dump() << "..." << std::endl;
                continue;
            }
            auto steganogram = base64_decode(reply["result"]);
            auto& image = *imageFromSteganogram(id, steganogram);
            JSON json;
            json["id"] = image.id;
            json["thumb"] = image.img_json["thumb"];
            json["views"] = image.img_json["views"];

            array.push_back(json);
        }
    }
    return array;
}

JSON Image::getImage(RegistrarArbitration* ra, JSON id) {
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
        auto request = RDS.rmi("Image", owner, id, "__sendView", {});
        auto ip = ra->getUserIP(owner);
        if (!ip.has_value()) {
            throw "user.doesNotExist";
        }

        auto reply = RDS.communicateRMI(ip.value(), REQ_PORT, request);

        int accesses = image.img_json["access"][self];
        image.img_json["access"][self] = accesses - 1;
        return image.img_json["data"];
    }
    throw "image.unauthorized";
}
