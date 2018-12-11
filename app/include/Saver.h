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
    std::vector<uint8> loadDownloadedImage(std::string filepath);
    std::vector<uint8> loadOwnImage(std::string filepath);

    void saveJSON(std::string filename,std::vector<uint8> privateKey,int remViews,std::string userID);
    void loadJSON();
};

#endif 