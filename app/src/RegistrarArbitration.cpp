#include <iostream>

#include <Vectorize.h>
#include <DumpFile.h>

#include <picosha2.h>
#include <base64.h>
#include <TweetNACL.h>

#include "RegistrarArbitration.h"
#include "Constants.h"

#define RDS RRAD::Dispatcher::singleton
#define __com(x) RDS.communicateRMI(adsIP, ADS_PORT, x)

#define USERHOME std::string(getenv("HOME"))

RegistrarArbitration::RegistrarArbitration(std::string ip) {
    adsIP = ip;
    
    auto request = RDS.listRPC("Registrar", ADS_USERNAME);
    auto reply = RDS.communicateRMI(adsIP, ADS_PORT,request);
    registrarID = reply["result"][0];

    try {
        auto vector = dumpFile(USERHOME + "/.deve/userData.json");
        auto string = RRAD::devectorizeToString(vector);
        auto json = JSON::parse(string);

        auto pubKeyString = json["publicKey"];
        auto secKeyString = json["secretKey"];

        publicKey  = base64_decode(pubKeyString);
        privateKey = base64_decode(secKeyString);

    } catch (const char *e) {
        std::cout << "[DEVE] No user file found, user is advised to sign up." << std::endl;

        publicKey.resize(crypto_sign_PUBLICKEYBYTES);
        privateKey.resize(crypto_sign_SECRETKEYBYTES);
    }

}

void RegistrarArbitration::encodeArguments(JSON* ptr) {
    auto& json = *ptr;

    std::vector<uint8> hash(picosha2::k_digest_size);

    auto hashable = json["data"].dump(); // Hashable
    picosha2::hash256(hashable.begin(), hashable.end(), hash.begin(), hash.end()); // Hash (bytes)


    auto signable = picosha2::bytes_to_hex_string(hash.begin(), hash.end()); // Hash (string)

    std::vector<uint8> signedMessage;
    unsigned long long signLength;
    signedMessage.resize(signable.length() + crypto_sign_BYTES);

    crypto_sign(&signedMessage[0], &signLength, (unsigned char*)signable.c_str(), signable.length(), &privateKey[0]); // Signed (bytes)

    json["signature"] = base64_encode(&signedMessage[0], signLength); // Base64 (string)
}

void RegistrarArbitration::verifyArguments(std::string userName, JSON* ptr) {
    auto& json = *ptr;

    std::vector<uint8> hash(picosha2::k_digest_size);

    auto hashable = json["data"].dump(); // Hashable
    picosha2::hash256(hashable.begin(), hashable.end(), hash.begin(), hash.end()); // Hash (bytes)
    auto comparable = picosha2::bytes_to_hex_string(hash.begin(), hash.end()); // Hash (string)

    auto signature = base64_decode(json["signature"]); // Signature (bytes)

    std::vector<uint8> unsignedHash;
    unsigned long long unsignLength;
    unsignedHash.resize(signature.size());

    std::vector<uint8>* publicKeyPtr;
    if (localRegistry.find(userName) == localRegistry.end()) {
        updateUserList();
        if (localRegistry.find(userName) == localRegistry.end()) {
            throw "crypto.cannotFindUser";
        }
    }
    publicKeyPtr = &localRegistry[userName].publicKey;
    auto& publicKey = *publicKeyPtr;

    auto result = crypto_sign_open(&unsignedHash[0], &unsignLength, &signature[0], signature.size(), &publicKey[0]); // Challenger Hash (bytes + trash)

    if (result < 0) {
        throw "crypto.invalidKey";
    }

    unsignedHash.resize(unsignLength); // Challenger Hash (bytes)
    auto challenger = RRAD::devectorizeToString(unsignedHash); // Challenger Hash (string)

    if (comparable != challenger) {
        std::cerr << "[DEVE] Cryptographic Hash Mismatch: "
                  << "Hash 0:" << comparable << std::endl
                  << "Hash 1:" << challenger << std::endl
                  << "String (" << hashable.length() << " bytes): " << hashable  << std::endl
                  << std::endl;
        throw "crypto.hashMismatch";
    }
}

bool RegistrarArbitration::reg(std::string password) {
    crypto_sign_keypair(&publicKey[0], &privateKey[0]);
    RDS.cm = this;

    std::string pubKeyString = base64_encode(&publicKey[0],  crypto_sign_PUBLICKEYBYTES);
    std::string secKeyString = base64_encode(&privateKey[0], crypto_sign_SECRETKEYBYTES);

    auto request = RDS.rmiReqMsg("Registrar", ADS_USERNAME, registrarID, "register",
        {
            {"password", password},
            {"publicKey", pubKeyString}
        }
    );
    auto reply = __com(request);
    if (reply["result"]) {
        JSON json;
        json["userName"] = RDS.getUID();
        json["publicKey"] = pubKeyString;
        json["privateKey"] = secKeyString;

        auto string = json.dump();
        auto userHome = std::string(getenv("HOME"));
        dumpIntoFile(userHome + "/.deve/userData.json", RRAD::vectorize(string));
    } else {
        std::cerr << "[DEVE] Signup failed!" << std::endl;
    }
    return reply["result"];
}

void RegistrarArbitration::updateUserList() {
    auto request = RDS.rmiReqMsg("Registrar", ADS_USERNAME, registrarID,  "list", EmptyJSO);
    auto reply = __com(request);
    localRegistry = std::map<std::string, User>();
    for (auto user: reply["result"]) {
        localRegistry[user["userName"]] = User();
        auto& target = localRegistry[user["userName"]];
        target.publicKey = base64_decode(user["publicKey"]);
        if (user["ip"].is_null()) {
            target.ip = std::nullopt;
        } else {
            target.ip = user["ip"];
        }
    }
}

JSON RegistrarArbitration::getList() {
    updateUserList();
    JSON returnValue = EmptyJSO;
    for (auto user: localRegistry) {
        returnValue[user.first] = user.second.ip.has_value() ? user.second.ip.value() : nullptr;
    }
    return returnValue;
}

std::optional<std::string> RegistrarArbitration::getUserIP(std::string userName) {
    if (localRegistry.find(userName) == localRegistry.end()) {
        updateUserList();
    }
    if (localRegistry.find(userName) == localRegistry.end()) {
        return std::nullopt;
    }
    return localRegistry[userName].ip;
}

bool RegistrarArbitration::authenticate(std::string password) {
    auto request = RDS.rmiReqMsg("Registrar", ADS_USERNAME, registrarID, "authenticate",
        {
            {"password", password}
        }
    );
    auto reply = __com(request);

    updateUserList();

    return reply["result"];
}


void RegistrarArbitration::logout() {
    auto request = RDS.rmiReqMsg("Registrar", ADS_USERNAME, registrarID, "__logout", EmptyJSO);
    auto reply = __com(request);
}
