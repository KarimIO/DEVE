#ifndef _registrar_h
#define _registrar_h

#include "Dispatcher.h"
#include "Statistics.h"

#include <string>
#include <map>

class Registrar: public RRAD::RemoteObject {
    std::map<std::string, std::string> registry;
    bool authenticate(std::string userName, std::string password);
    bool reg(std::string userName, std::string password);
    void startStatistics(JSON id);
public:
    Registrar();
    virtual std::string getClassName() override { return "Registrar"; }
    virtual JSON executeRPC(std::string name, JSON arguments) override;
};

#endif // _registrar_h