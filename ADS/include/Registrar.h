#ifndef _registrar_h
#define _registrar_h

#include "Dispatcher.h"

#include <string>
#include <map>
#include <optional>

struct User {
    std::string password;
    std::string publicKey;
    std::optional<std::string> lastIP;
};

class Registrar: public RRAD::RemoteObject {
    std::map<std::string, User> registry;
    bool authenticate(std::string userName, std::string password, std::string ip);
    bool reg(std::string userName, std::string password, std::string publicKey);
    JSON list();
public:
    Registrar();
    virtual std::string getClassName() override { return "Registrar"; }
    virtual JSON executeRPC(std::string name, JSON arguments) override;
};

#endif // _registrar_h
