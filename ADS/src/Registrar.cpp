#include "Registrar.h"
#include "Constants.h"

#include <iostream>
#include <cstddef>
#include <sys/time.h>

Registrar::Registrar() {
    static int counter = 0;
    id["ownerID"] = ADS_USERNAME;
    id["unixTimestamp"] = (uint32)time(NULL);
    id["id"] = counter++;
    id["class"] = "Registrar";
    RRAD::Dispatcher::singleton.registerObject(this);
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

bool Registrar::logout(std::string name) {
    if (registry.find(name) != registry.end()) {
        registry[name].lastIP = std::nullopt;
        std::cout << "User " << name << " logged off." << std::endl;
        return true;
    }
    return false;
}

JSON Registrar::list() {
    JSON json = JSON::array();
    for (auto user: registry) {
        JSON obj;
        obj["userName"] = user.first;
        obj["ip"] = user.second.lastIP.has_value() ? user.second.lastIP.value() : nullptr;
        obj["publicKey"] = user.second.publicKey;
        json.push_back(obj);
    }
    return json;
}

JSON Registrar::executeRPC(std::string name, JSON arguments) {
    if (name == "authenticate") {
        JSON reply;
        reply["result"] = authenticate(arguments["RRAD::senderUserName"], arguments["password"], arguments["RRAD::senderIP"]);
        return reply;
    } else if (name == "__logout") {
        logout(arguments["RRAD::senderUserName"]);
        return JSON();
    } else if (name == "register") {
        JSON reply;
        reply["result"] = reg(arguments["RRAD::senderUserName"], arguments["password"], arguments["publicKey"]);
        return reply;
    } else if (name == "list") {
        JSON reply;
        reply["result"] = list();
        return reply;
    }
    throw "rpc.unknownMethod";
}
