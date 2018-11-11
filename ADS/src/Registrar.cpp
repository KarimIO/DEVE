#include "Registrar.h"
#include "Constants.h"

#include <iostream>
#include <sys/time.h>

Registrar::Registrar() {
    static int counter = 0;
    JSON id;
    id["ownerID"] = ADS_USERNAME;
    id["unixTimestamp"] = (uint32)time(NULL);
    id["id"] = counter++;
    id["class"] = "Registrar";
    RRAD::Dispatcher::singleton.registerObject(id, this);
}

bool Registrar::reg(std::string name, std::string password, std::string publicKey) {
    if (registry.find(name) != registry.end()) {
        return false;
    }
    auto newUser = User();
    newUser.password = password;
    newUser.publicKey = publicKey;
    newUser.lastIP = std::nullopt;
    registry[name] = newUser;
    std::cout << "User " << name << " registered with key " << publicKey << "." << std::endl;
    return true;
}

bool Registrar::authenticate(std::string name, std::string password, std::string ip) {
    if (registry.find(name) != registry.end() && registry[name].password == password) {
        registry[name].lastIP = ip;
        std::cout << "User " << name << " logged in on IP " << ip << "." << std::endl;
        return true;
    }
    return false;
}

JSON Registrar::list() {
    JSON json = JSON::array();
    for (auto user: registry) {
        if (!user.second.lastIP.has_value()) {
            continue;
        }
        JSON obj;
        obj["userName"] = user.first;
        obj["ip"] = user.second.lastIP.value();
        obj["publicKey"] = user.second.publicKey;
        json.push_back(obj);
    }
    return json;
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
    } else if (name == "list") {
        JSON reply;
        reply["result"] = list();
        return reply;
    }
    throw "rpc.unknownMethod";
}
