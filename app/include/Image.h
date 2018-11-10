#ifndef _IMAGE_h
#define _IMAGE_h

#include <string>

#include <Types.h>
#include <Dispatcher.h>

#include "base64.h"
#include "JPEG.h"

using namespace RRAD;

/*
wrapper for the JSON containing the user image that should be embedded in
the comment section of default JPEG
format:
    {
        title: ..,
        ownerID: ..,
        views: #,
        thumb: base64_img_data
        data: base64_img_data,
        access: {

        }
    }
*/

class Image : public RemoteObject {
    void setAccess(std::string targetUser, uint32 view_cnt);
    void recordView(std::string viewer);
    void requestAccess(std::string requester);
public:
    Image(std::string title, std::string base64, std::string thumbBase64);

    JSON id;
    JSON img_json;

    static std::queue< std::pair<std::string, std::string> > requests;
    static JPEG generateSteganogramJPEG();
    
    JSON getJSON();
    std::vector<uint8> getSteganogram();
    std::string getSteganogramBase64();
 
    JSON executeRPC(std::string name, JSON arguments) override;
};

#endif //_IMAGE_h
