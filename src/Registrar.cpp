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

bool Registrar::reg(std::string name, std::string password, std::string publicKey) {
    if (registry.find(name) == registry.end()) {
        return false;
    }
    auto newUser = User();
    newUser.password = password;
    newUser.publicKey = publicKey;
    registry[name] = newUser;
    return true;
}

bool Registrar::authenticate(std::string name, std::string password, std::string ip) {
    if (registry.find(name) != registry.end() && registry[name].password == password) {
        registry[name].lastIP = ip;
        return true;
    }
    return false;
}

void Registrar::startStatistics(JSON id) {
    new Statistics(id);
}

JSON Registrar::executeRPC(std::string name, JSON arguments) {
    if (name == "authenticate") {
        JSON reply;
        reply["result"] = authenticate(arguments["userName"], arguments["password"], arguments["__RRAD__INTERNAL__senderIP"]);
        return reply;
    } else if (name == "register") {
        JSON reply;
        reply["result"] = reg(arguments["userName"], arguments["password"], arguments["publicKey"]);
        return reply;
    } else if (name == "startStatistics") {
        startStatistics(arguments);
        return JSON();
    }
    throw "rpc.unknownMethod";
}
