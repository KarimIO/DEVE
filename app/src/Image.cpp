#include "Image.h"

static auto defaultJSON = R"(
{
    "ownerID": "__NOUSER",
    "data": "hecc",
    "views": 0,
    "access": {
    }
}
)"_json;


Image::Image(std::string base64) {
    static int counter = 0;

    img_json = defaultJSON;
    img_json["ownerID"] = Dispatcher::singleton.getUID();
    img_json["data"] = base64;

    id = JSON();
    id["ownerID"] = Dispatcher::singleton.getUID();
    id["unixTimestamp"] = (uint32)time(NULL);
    id["id"] = counter++;
    id["class"] = "Image";

    RRAD::Dispatcher::singleton.registerObject(id, this);
}

// You use this one if you own an image and you want to change another user's access to it while THEY have it.
void Image::setAccess(uint32 view_cnt, std::string targetUser) {
    img_json["access"][targetUser] = view_cnt;
}

void Image::requestAccess(std::string requester) {
    Image::requests.push(std::pair(id.dump(), requester));
}

void Image::recordView(std::string viewer) {
    img_json["views"] += 1;
    img_json["access"][viewer] -= 1;
    if (img_json["access"][viewer] < 0) {
        std::cout << "[DEVE] " << viewer << "'s being cheeky with image " << id.dump() << "." << std::endl;
        img_json["access"][viewer] = 0;
    }
}

std::string Image::getString() {
    return img_json.dump();
}

JSON Image::getJSON() {
    return img_json;
}

JSON Image::executeRPC(std::string name, JSON arguments) {
    if (name == "__setAccess") {
        setAccess(arguments["view_cnt"], arguments["targetUser"]);
        return JSON();
    } else if (name == "__requestAccess") {
        requestAccess(arguments["userName"]);
        return JSON();
    } else if (name == "__sendView") {
        recordView(arguments["userName"])
        return JSON();
    }
    
    throw "rpc.unknownMethod";
}
