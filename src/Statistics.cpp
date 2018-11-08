#include "Statistics.h"

Statistics::Statistics(JSON id) {
    RRAD::Dispatcher::singleton.registerObject(id, this);
}

JSON Statistics::executeRPC(std::string name, JSON arguments) {
    if (name == "view") {
        views += 1;
        return JSON();
    } else if (name == "register") {
        JSON reply;
        reply["views"] = views;
        return reply;
    }
    throw "rpc.unknownMethod";
}