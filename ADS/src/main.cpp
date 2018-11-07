#include "Dispatcher.h"
#include "Registrar.h"

RRAD::Dispatcher RRAD::Dispatcher::singleton = RRAD::Dispatcher(31000);

int main(int argc, char** argv) {
    new Registrar();
    RRAD::Dispatcher::singleton.syncLoop();
}