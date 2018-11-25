#ifndef _registrar_arbitration_h
#define _registrar_arbitration_h

#include <Types.h>
#include <Dispatcher.h>

#include <optional>

struct User {
    std::optional<std::string> ip;
    std::vector<uint8> publicKey;
};

class RegistrarArbitration: public RRAD::CryptoModule {
    std::string adsIP;
    JSON registrarID;
public: // temp
    std::map<std::string, User> localRegistry;
    std::vector<uint8> publicKey;
    std::vector<uint8> privateKey;

public:
    RegistrarArbitration(std::string adsIP);
    bool reg(std::string password);
    bool authenticate(std::string password);
    void logout();
    void updateUserList();

    JSON getList();
    std::optional<std::string> getUserIP(std::string userName);

    virtual void encodeArguments(JSON* ptr) override;
    virtual void verifyArguments(std::string userName, JSON* ptr) override;
};

#endif // _registrar_arbitration_h
