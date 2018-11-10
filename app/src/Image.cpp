#include "Image.h"
#include "DumpFile.h"

#include <iostream>

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

Image::Image(std::string title, std::string base64, std::string thumbBase64) {
    static int counter = 0;

    img_json = defaultJSON;
    img_json["title"] = title;
    img_json["ownerID"] = Dispatcher::singleton.getUID();
    img_json["data"] = base64;
    img_json["thumb"] = thumbBase64;

    id = JSON({});
    id["ownerID"] = Dispatcher::singleton.getUID();
    id["unixTimestamp"] = (uint32)time(NULL);
    id["id"] = counter++;
    id["class"] = "Image";

    RRAD::Dispatcher::singleton.registerObject(id, this);
}

void Image::setAccess(std::string targetUser, uint32 view_cnt) {
    img_json["access"][targetUser] = view_cnt;
}

void Image::requestAccess(std::string requester) {
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

JSON Image::executeRPC(std::string name, JSON arguments) {
    if (name == "__setAccess") {
        setAccess(arguments["view_cnt"], arguments["targetUser"]);
        return JSON({});
    } else if (name == "__requestAccess") {
        requestAccess(arguments["userName"]);
        return JSON({});
    } else if (name == "__sendView") {
        recordView(arguments["userName"]);
        return JSON({});
    } else if (name == "getImage") {
        JSON reply;
        reply["result"] = getSteganogramBase64();
        return reply;
    }
    throw "rpc.unknownMethod";
}
