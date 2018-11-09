#include <iostream>
#include <exception>

#include <signal.h>

#include "deve_ui_server.h"
#include "UserArbitration.h"
#include "ADSConstants.h"

#include <Dispatcher.h>
#include <pistache/endpoint.h>

using namespace Pistache;


RRAD::Dispatcher RRAD::Dispatcher::singleton = RRAD::Dispatcher("__DEVE_INIT", REQ_PORT);

void deathRoutine() {
    // Save here
    exit(0);
}

int main(int argc, char *argv[]) {
    // Load here

    if (argc < 2) {
        std::cerr << "ADS address must be provided with invocation." << std::endl;
        exit(64);
    }

    auto adsIP = std::string(argv[1]);
#if 0

    auto killThread = std::thread([](){
        std::cout << "(Ctrl+D to exit)" << std::endl;
        for(;;) {
            std::string garbage;
            std::getline(std::cin, garbage);
            if (std::cin.eof()) {
                deathRoutine();
            }
        }
    });
    killThread.detach();

    try {

        Port port(REST_PORT);
        Address addr(Ipv4::any(), port);
        
        DeveUIServer duis;
        std::cout << "[DEVE] Serving REST on port " << chosenPort << "." << std::endl;
        duis.setUpUIServer(addr);

    } catch(std::runtime_error &e) {
        std::cerr << e.what() << "\n";
    }
#else
    auto rg = RRAD::RequestGenerator("donn");
    auto ua = UserArbitration(adsIP, &rg);
    if (ua.reg(&rg, "password", "not a pkey")) {
        if (ua.authenticate(&rg, "pass")) {
            std::cout << "Authorization success." << std::endl;
        }
    }
    
    
    
#endif
}
