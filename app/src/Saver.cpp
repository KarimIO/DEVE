#include "Saver.h"
#include <iostream>
#include "DumpFile.h"
#include "Image.h"
#include "base64.h"
#include <fstream>

void Saver::saveImage(std::string thumb64, std::string string64, std::string filename){
      Image test(thumb64,string64);
      auto vec = test.getSteganogram();
      dumpIntoFile("out/"+filename,vec);

}; 

void Saver::loadImage(std::string filepath){
    auto vec = dumpFile(filepath);

//    std::cout<<vec<<std::endl;
};

void Saver::saveJSON(JSON  t){
    
};

void Saver::loadJSON(){

    
};