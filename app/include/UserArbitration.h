#ifndef _user_arbitration_h
#define _user_arbitration_h

#include "Dispatcher.h"
#include "Types.h"

#include <future>

class UserArbitration {
    std::string adsIP;
    JSON registrarID;
public:
    UserArbitration(std::string adsIP);
    std::promise<bool> authenticate();
};

#endif // _user_arbitration_h
