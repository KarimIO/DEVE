#include <iostream>
#include <exception>

#include <signal.h>

#include "deve_ui_server.h"
#include "UserArbitration.h"
#include "Constants.h"
#include "DumpFile.h"
#include "Connection.h"

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
#if 0
    if (argc < 2) {
        std::cerr << "ADS address must be provided with invocation." << std::endl;
        exit(64);
    }

    auto adsIP = std::string(argv[1]);

    std::cout << "Using ADS at " << adsIP << std::endl;
#endif
#if 1

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
        
        DeveUIServer duis(adsIP);
        std::cout << "[DEVE] Serving REST on port " << REST_PORT << "." << std::endl;
        duis.setUpUIServer(addr);

    } catch(std::runtime_error &e) {
        std::cerr << e.what() << "\n";
    }

#endif
#if 0
    auto rg = RRAD::RequestGenerator("donn");
    auto ua = UserArbitration(adsIP, &rg);
    ua.reg(&rg, "password", "not a pkey");
    if (!ua.authenticate(&rg, "password")) {
        std::cout << "Authorization failed." << std::endl;
    }
    std::cout << "Logged in." << std::endl;
    
    auto imgData = dumpFile("tests/images/xc201.jpg");
    auto conn = RRAD::Connection("192.168.64.8", 20400);
    conn.write(imgData);
    
#endif
}
