#include "Saver.h"
#include <iostream>
#include "DumpFile.h"
#include "Image.h"
#include "base64.h"
#include <fstream>

void Saver::saveImage(std::string thumb64, std::string string64, std::string filename){
      Image test(filename, string64, thumb64);
      auto vec = test.getSteganogram();
      dumpIntoFile("saved/"+filename,vec);
}

void Saver::loadImage(std::string filepath ){
    auto vec = dumpFile(filepath);

    return vec;
//    std::cout<<vec<<std::endl;
}

void Saver::saveJSON(JSON  t){
    
}

void Saver::loadJSON(){

    
}
