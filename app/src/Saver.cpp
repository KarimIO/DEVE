#include "Saver.h"
#include <iostream>
#include "DumpFile.h"
#include "Image.h"
#include "base64.h"
#include <fstream>

#define USERHOME std::string(getenv("HOME"))

void Saver::saveImage(std::string thumb64, std::string string64, std::string filename){
      Image test(filename, string64, thumb64);
      auto vec = test.getSteganogram();
      std::string fulldir = USERHOME + "/.deve/owned/" + filename;
      dumpIntoFile(fulldir,vec);
}




void Saver::saveJSON(std::string filename,std::vector<uint8> privateKey,int remViews,std::string userID){
    JSON obj;
    obj["FileName"] = filename;
    obj["ownerID"] = userID;
    obj["privateKey"] = privateKey;
    obj["views"]=remViews;
    auto string = obj.dump();
    dumpIntoFile(USERHOME + "/.deve/downloaded/"+filename+".json", RRAD::vectorize(string));

}

void Saver::loadJSON(){

    
}
