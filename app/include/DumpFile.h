#ifndef _dump_file_h
#define _dump_file_h

#include <vector>
#include <string>
#include <fstream>
#include "Types.h"

#include <iostream>

inline std::vector<uint8> dumpFile(std::string filename) {

    std::ifstream infile(filename);
    if (infile.fail()) {
        throw "io.fail";
    }
    infile.seekg(0, infile.end);
    ssize_t length = infile.tellg();
    infile.seekg(0, infile.beg);
    uint8* buffer = new uint8[length];

    infile.read((char*)&buffer[0], length);

    infile.close();

    return std::vector<uint8>(buffer, buffer + length);
}

inline void dumpIntoFile(std::string filename, std::vector<uint8> vec) {
    std::ofstream outfile = std::ofstream(filename, std::ios::binary);
    if (outfile.fail()) {
        throw "io.fail";
    }
    outfile.write((char*)&vec[0], vec.size());
    outfile.close();
}

#endif // _dump_file_h
