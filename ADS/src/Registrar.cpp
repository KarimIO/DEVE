#include "Registrar.h"

#include <sys/time.h>

Registrar::Registrar() {
    static int counter = 0;
    JSON id;
    id["ownerID"] = "deve-ads";
    id["unixTimestamp"] = (uint32)time(NULL);
    id["id"] = counter++;
    id["class"] = "Registrar";
    RRAD::Dispatcher::singleton.registerObject(id, this);
}

bool Registrar::reg(std::string name, std::string password) {
    if (registry.find(name) == registry.end()) {
        return false;
    }
    
    registry[name] = password;
    return true;
}

bool Registrar::authenticate(std::string name, std::string password) {
    if (registry.find(name) == registry.end()) {
        return false;
    }
    
    return registry[name] == password;
}

void Registrar::startStatistics(JSON id) {
    new Statistics(id);
}

JSON Registrar::executeRPC(std::string name, JSON arguments) {
    if (name == "authenticate") {
        JSON reply;
        reply["result"] = authenticate(arguments["userName"], arguments["password"]);
        return reply;
    } else if (name == "register") {
        JSON reply;
        reply["result"] = reg(arguments["userName"], arguments["password"]);
        return reply;
    } else if (name == "startStatistics") {
        startStatistics(arguments);
        return JSON();
    }
    throw "rpc.unknownMethod";
}