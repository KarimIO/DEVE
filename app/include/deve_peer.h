#ifndef _DEVE_PEER_H
#define _DEVE_PEER_H

#include "deve_ui_server.h"

#include <vector>
#include <string>

class DevePeer {
public:
    DevePeer();
    void setUpUIServer(Pistache::Address addr);
    void start();
    std::vector<std::string> getUserList();
    ~DevePeer();
private:
    DeveUIServer ui_server_;
};

#endif