#ifndef _statistics_h
#define _statistics_h

#include "Dispatcher.h"

#include <string>

class Statistics: public RRAD::RemoteObject {
    int views = 0;
public:
    Statistics(JSON id);
    virtual std::string getClassName() override { return "Statistics"; }
    virtual JSON executeRPC(std::string name, JSON arguments) override;
};

#endif // _statistics_h