#include <iostream>
#include <exception>

#include <signal.h>

#include "deve_peer.h"

#include <Dispatcher.h>
#include <pistache/endpoint.h>

using namespace Pistache;

RRAD::Dispatcher RRAD::Dispatcher::singleton = RRAD::Dispatcher("__DEVE_INIT", 32000);

int main(int argc, char *argv[]) {
#if 1
    try {
        Port port(9080);

        int thr = 2;

        if (argc >= 2) {
            port = std::stol(argv[1]);
        }

        Address addr(Ipv4::any(), port);
        
        DevePeer peer;
        //peer.setUpUIServer(addr);
        //peer.start();
    } catch(std::runtime_error &e) {
        std::cerr << e.what() << "\n";
    }
#else
    auto ua = UserArbitration();
#endif
}
