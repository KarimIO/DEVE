#include <iostream>

#include "UserArbitration.h"
#include "ADSConstants.h"

#define __com(x) rg->communicateRMI(adsIP, ADS_PORT, x)

UserArbitration::UserArbitration(std::string ip, RRAD::RequestGenerator* rg) {
    adsIP = ip;
    auto request = rg->listRPC("Registrar", ADS_USERNAME);
    auto reply = __com(request);
    registrarID = reply[0];
}

bool UserArbitration::reg(RRAD::RequestGenerator* rg, std::string password, std::string publicKey) {
    auto request = rg->rmi("Registrar", ADS_USERNAME, registrarID, "register",
        {
            {"userName", rg->getUserName()},
            {"password", password},
            {"publicKey", publicKey}
        }
    );
    auto reply = __com(request);
    return reply["result"];
}

bool UserArbitration::authenticate(RRAD::RequestGenerator* rg, std::string password) {
    auto request = rg->rmi("Registrar", ADS_USERNAME, registrarID, "authenticate",
        {
            {"userName", rg->getUserName()},
            {"password", password}
        }
    );
    auto reply = __com(request);
    return reply["result"];
}
