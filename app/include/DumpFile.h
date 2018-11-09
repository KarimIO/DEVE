#ifndef _dump_file_h
#define _dump_file_h

inline std::vector<uint8> dumpFile(std::string filename) {
    std::vector<uint8> buffer;

    std::ifstream infile("tests/filename.jpeg");

    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);

    buffer.resize(length);
    infile.read((char*)&buffer[0], length);

    infile.close();

    return buffer;
}

#endif // _dump_file_h
