#ifndef _user_arbitration_h
#define _user_arbitration_h

#include <RequestGenerator.h>
#include <Types.h>

#include <future>
#include "TweetNACL.h"

class UserArbitration {
    std::string adsIP;
    JSON registrarID;
    uint8 publicKey[crypto_box_PUBLICKEYBYTES];
    uint8 privateKey[crypto_box_SECRETKEYBYTES];
public:
    UserArbitration(std::string adsIP);
    bool reg(RRAD::RequestGenerator* rg, std::string password, std::string publicKey);
    bool updateUserList(RRAD::RequestGenerator* rg);
    bool authenticate(RRAD::RequestGenerator* rg, std::string password);
    JSON getList();
};

#endif // _user_arbitration_h
