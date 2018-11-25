#ifndef _IMAGE_h
#define _IMAGE_h

#include <string>

#include <Types.h>
#include <Dispatcher.h>

#include "base64.h"
#include "JPEG.h"
#include "RegistrarArbitration.h"

using namespace RRAD;

/*
wrapper for the JSON containing the user image that should be embedded in
the comment section of default JPEG
format:
    {
        ownerID: ..,
        views: #,
        thumb: base64_img_data
        data: base64_img_data,
        access: {

        }
    }
*/

class Image : public RemoteObject {
    void recordView(std::string viewer);
    void requestAccess(std::string requester);
public:
    Image(std::string title, std::string base64, std::string thumbBase64);
    Image(JSON id, bool owned, JSON img_json); //changed order to avoid ambiguity, thanks c++

    JSON id;
    JSON img_json;

    static std::queue< std::pair<std::string, std::string> > requests;
    static JPEG generateSteganogramJPEG();
    void setAccess(std::string targetUser, uint32 view_cnt);
    
    JSON getJSON();
    std::vector<uint8> getSteganogram();
    std::string getSteganogramBase64();
    static Image* imageFromSteganogram(JSON id, std::vector<uint8> steganogram);

    static JSON getList(RegistrarArbitration* ra, std::string user);
    static std::string getImageData(RegistrarArbitration* ra, JSON id);
 
    virtual std::string getClassName() override { return "Image"; }
    virtual JSON executeRPC(std::string name, JSON arguments) override;
    virtual JSON getID() override { return id; }
};

#endif //_IMAGE_h
