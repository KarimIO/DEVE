#ifndef _user_arbitration_h
#define _user_arbitration_h

#include <RequestGenerator.h>
#include <Types.h>

#include <future>

class UserArbitration {
    std::string adsIP;
    JSON registrarID;
public:
    UserArbitration(std::string adsIP, RRAD::RequestGenerator* rg);
    bool reg(RRAD::RequestGenerator* rg, std::string password, std::string publicKey);
    bool authenticate(RRAD::RequestGenerator* rg, std::string password);
};

#endif // _user_arbitration_h
