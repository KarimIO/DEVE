#include <Dispatcher.h>
#include <Registrar.h>

#include "ADSConstants.h"

RRAD::Dispatcher RRAD::Dispatcher::singleton = RRAD::Dispatcher(ADS_USERNAME, ADS_PORT);

int main(int argc, char** argv) {
    new Registrar();
    RRAD::Dispatcher::singleton.syncLoop();
}
