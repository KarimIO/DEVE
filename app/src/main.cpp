#include <iostream>
#include <exception>

#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

#include <Dispatcher.h>
#include <pistache/endpoint.h>
#include <DumpFile.h>
#include <Connection.h>

#include "DeveUIServer.h"
#include "Constants.h"

using namespace Pistache;

RRAD::Dispatcher RRAD::Dispatcher::singleton = RRAD::Dispatcher("__DEVE_INIT", REQ_PORT);

#define USERHOME std::string(getenv("HOME"))

int main(int argc, char *argv[]) {
    // Load here
    if (argc < 2) {
        std::cerr << "ADS address must be provided with invocation." << std::endl;
        exit(64);
    }
    
    // Create deve folder if it doesn't exist
    std::cout << USERHOME << " FDIDISAFIOJKAL<FKASLFK" << std::endl;
    if (mkdir(
            (USERHOME + "/.deve/owned/").c_str(),  // IM SO SORRY
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
        ) == -1) {
        if (errno != EEXIST) {
            std::cout << "[DEVE] Could not create DEVE folder with error: " << strerror(errno) << "." << std::endl;
            exit(-1);
        }
    }
        if (mkdir(
            (USERHOME + "/.deve/downloaded/").c_str(),  // IM SO SORRY
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
        ) == -1) {
        if (errno != EEXIST) {
            std::cout << "[DEVE] Could not create DEVE folder with error: " << strerror(errno) << "." << std::endl;
            exit(-1);
        }
    }

    auto adsIP = std::string(argv[1]);

    std::cout << "Using ADS at " << adsIP << std::endl;

    try {
        Port port(REST_PORT);
        Address addr(Ipv4::any(), port);
        
        DeveUIServer duis(adsIP);
        std::cout << "[DEVE] Serving REST on port " << REST_PORT << "." << std::endl;

        RRAD::Dispatcher::singleton.start();
        
        duis.startServers(addr);

    } catch(std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
    } catch(const char* e) {
        std::cerr << e << std::endl;
    }
}
