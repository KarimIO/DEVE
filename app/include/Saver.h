#ifndef _SAVER_h
#define _SAVER_h

#include <string>
#include <fstream>
#include <Types.h>

#include "Image.h"
#include "DumpFile.h"

class Saver {
  public:  
    void saveImage(std::string thumb64,std::string string64,std::string filename);
    void loadImage(std::string filepath);

    void saveJSON(JSON t);
    void loadJSON();
};

#endif 