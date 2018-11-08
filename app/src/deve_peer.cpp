#include <deve_peer.h>

DevePeer::DevePeer() : ui_server_(this) {
    std::cout << "Initializing DEVE Peer.\n";
}

void DevePeer::setUpUIServer(Pistache::Address addr) {    
    std::cout << "\tSetting up UI Server...\n";
    ui_server_.setUpUIServer(addr);
    std::cout << "\tUI Server set up. See output on: " << addr.host() << ":" << addr.port() << "\n";
}

void DevePeer::start() {
    ui_server_.start();
}

std::vector<std::string> DevePeer::getUserList() {
    std::vector<std::string> users;
    users.push_back("ahmedghazy");
    users.push_back("alikhaled");
    users.push_back("amrkadi");
    users.push_back("karimah");
    users.push_back("donn");

    return users;
}
