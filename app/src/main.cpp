#include <iostream>
#include <exception>

#include <signal.h>

#include "deve_ui_server.h"
#include "UserArbitration.h"

#include <Dispatcher.h>
#include <pistache/endpoint.h>

using namespace Pistache;

RRAD::Dispatcher RRAD::Dispatcher::singleton = RRAD::Dispatcher("__DEVE_INIT", 9999);


int main(int argc, char *argv[]) {

#if 1
    uint32 chosenPort = 9090;
    int thr = 2;

    auto killThread = std::thread([](){
        std::cout << "*whispers* (Ctrl+D to exit)" << std::endl;
        std::string garbage;
        std::getline(std::cin, garbage);
        if (std::cin.eof()) {
            exit(101);
        }
    });
    killThread.detach();

    try {

        if (argc >= 2) {
            chosenPort = std::stoi(argv[1]);
        }

        Port port(chosenPort);
        Address addr(Ipv4::any(), port);
        
        DeveUIServer duis;
        std::cout << "[DEVE] Serving REST on port " << chosenPort << "." << std::endl;
        duis.setUpUIServer(addr);

    } catch(std::runtime_error &e) {
        std::cerr << e.what() << "\n";
    }
#else
    auto rg = RRAD::RequestGenerator("donn");
    auto ua = UserArbitration("0.0.0.0", &rg);
    if (ua.reg(&rg, "pass", "aksdmasldmlkas")) {
        if (ua.authenticate(&rg, "pass")) {
            std::cout << "Success." << std::endl;
        }
    }
    
#endif
}
