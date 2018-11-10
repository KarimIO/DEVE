#ifndef _jpeg_h
#define _jpeg_h

#include "Vectorize.h"

struct JPEG {
    std::string comment;
    
    static JPEG fromByteVector(const std::vector<uint8>* byteVector); // I hate passing by reference but in this static hell, can you truly blame me.
    std::vector<uint8> toByteVector();

    JPEG();
private:
    std::vector<uint8> notString;
};

#endif
