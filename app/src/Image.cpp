#include "Image.h"


Image::Image(JSON img_json){
    static int counter = 0;

    this->img_json = img_json;
    JSON id;
    id["ownerID"] = Dispatcher::singleton.getUID();
    id["unixTimestamp"] = (uint32)time(NULL);
    id["id"] = counter++;
    id["class"] = "Registrar";
    RRAD::Dispatcher::singleton.registerObject(id, this);
}

void Image::setAccess(uint32 view_cnt){
    img_json["nr_views"] = view_cnt;
}

void Image::increaseAccess(uint32 view_cnt){
    uint32 nr_views = img_json["nr_views"];
    setAccess(nr_views+view_cnt > 0? nr_views+view_cnt : 0);
}

std::string Image::getString(){
    return img_json.dump();
}

JSON Image::getJSON(){
    return img_json;
}

JSON Image::executeRPC(std::string name, JSON arguments) {
    uint32 view_cnt = arguments["view_cnt"];
    if (name == "__setAccess")
        setAccess(view_cnt);
    else if (name == "__increaseAccess")
        increaseAccess(view_cnt);
    else 
        throw "rpc.unknownMethod";
    return JSON({});
}
