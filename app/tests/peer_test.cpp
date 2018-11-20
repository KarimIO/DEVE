// clang++ -g -Iinclude -I../submodules/rrad/libinclude -I../submodules/rrad/include -L -lrrad tests/peer_test.cpp -o peer_test.out 
#include <iostream>
#include "Dispatcher.h"
#include "Image.h"
using namespace RRAD;


int main(int argc, char const* argv[])
{
    Image m;
    std::cout << m.getClassName() << std::endl;

    
    return 0;
}
