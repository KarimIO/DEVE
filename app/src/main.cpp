#include <iostream>
#include <exception>

#include <signal.h>

#include <Dispatcher.h>
#include <pistache/endpoint.h>
#include <DumpFile.h>
#include <Connection.h>

#include "DeveUIServer.h"
#include "Constants.h"

using namespace Pistache;

RRAD::Dispatcher RRAD::Dispatcher::singleton = RRAD::Dispatcher("__DEVE_INIT", REQ_PORT);


int main(int argc, char *argv[]) {
    // Load here
    if (argc < 2) {
        std::cerr << "ADS address must be provided with invocation." << std::endl;
        exit(64);
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
