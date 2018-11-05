// clang++ -g -Iinclude -I../submodules/rrad/include -L -lrrad src/JPEG.cpp tests/jpeg_main.cpp -o jpeg.out && LD_LIBRARY_PATH=. lldb ./jpeg.out

#include "JPEG.h"

#include <iostream>
#include <fstream>

int main() {
    //Read input
    std::ifstream infile("tests/input.jpeg");

    std::vector<uint8> buffer;

    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);

    buffer.resize(length);
    infile.read((char*)&buffer[0], length);

    infile.close();


    // Construct JPEG
    JPEG jpeg = JPEG::fromByteVector(&buffer);
    jpeg.comment = "This is a test comment!";
    std::vector<uint8> vec = jpeg.toByteVector();

    // Write output
    std::ofstream outfile = std::ofstream("output.jpeg", std::ios::binary);
    outfile.write((char*)&vec[0], vec.size());
    outfile.close();

    // Reread output
    infile.open("output.jpeg");

    infile.seekg(0, infile.end);
    length = infile.tellg();
    infile.seekg(0, infile.beg);

    buffer.resize(length);
    infile.read((char*)&buffer[0], length);

    infile.close();

    // Print out comment to verify
    jpeg = JPEG::fromByteVector(&buffer);
    std::cout << jpeg.comment << std::endl;

    return 0;
}
