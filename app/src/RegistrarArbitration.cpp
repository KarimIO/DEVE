#include <iostream>

#include <picosha2.h>
#include <Vectorize.h>
#include <TweetNACL.h>

#include "base64.h"
#include "RegistrarArbitration.h"
#include "Constants.h"

#define RDS RRAD::Dispatcher::singleton
#define __com(x) RDS.communicateRMI(adsIP, ADS_PORT, x)

RegistrarArbitration::RegistrarArbitration(std::string ip) {
    adsIP = ip;
    
    auto request = RDS.listRPC("Registrar", ADS_USERNAME);
    auto reply = RDS.communicateRMI(adsIP, ADS_PORT,request);
    registrarID = reply[0];

    publicKey.resize(crypto_sign_PUBLICKEYBYTES);
    privateKey.resize(crypto_sign_SECRETKEYBYTES);
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

void RegistrarArbitration::verifyArguments(JSON* ptr) {
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
    auto targetUser = json["data"]["userName"];
    if (localRegistry.find(targetUser) == localRegistry.end()) {
        updateUserList();
        if (localRegistry.find(targetUser) == localRegistry.end()) {
            throw "crypto.cannotFindUser";
        }
    }
    publicKeyPtr = &localRegistry[targetUser].publicKey;
    auto& publicKey = *publicKeyPtr;

    auto result = crypto_sign_open(&unsignedHash[0], &unsignLength, &signature[0], signature.size(), &publicKey[0]); // Challenger Hash (bytes + trash)
    unsignedHash.resize(unsignLength); // Challenger Hash (bytes)

    auto challenger = RRAD::devectorizeToString(unsignedHash); // Challenger Hash (string)

    if (result < 0 ) {
        throw "crypto.invalidKey";
    }
    if (comparable != challenger) {
        throw "crypto.hashMismatch";
    }
}

bool RegistrarArbitration::reg(std::string password) {
    crypto_sign_keypair(&publicKey[0], &privateKey[0]);
    RDS.cm = this;

    std::string pubKeyString = base64_encode(&publicKey[0], crypto_sign_PUBLICKEYBYTES);
    auto request = RDS.rmi("Registrar", ADS_USERNAME, registrarID, "register",
        {
            {"userName", RDS.getUID()},
            {"password", password},
            {"publicKey", pubKeyString}
        }
    );
    auto reply = __com(request);
    return reply["result"];
}

bool RegistrarArbitration::updateUserList() {
    auto request = RDS.rmi("Registrar", ADS_USERNAME, registrarID,  "list", {});
    auto reply = __com(request);
    for (auto user: reply["result"]) {
        localRegistry[user["userName"]] = User();
        auto& target = localRegistry[user["userName"]];
        target.publicKey = base64_decode(user["publicKey"]);
        target.ip = user["ip"];
    }
}

bool RegistrarArbitration::authenticate(std::string password) {
    auto request = RDS.rmi("Registrar", ADS_USERNAME, registrarID, "authenticate",
        {
            {"userName", RDS.getUID()},
            {"password", password}
        }
    );
    auto reply = __com(request);

    updateUserList();

    return reply["result"];
}


void RegistrarArbitration::logout() {
    auto request = RDS.rmi("Registrar", ADS_USERNAME, registrarID, "__logout",
        {
            {"userName", RDS.getUID()},
        }
    );
    auto reply = __com(request);
}