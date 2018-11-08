#ifndef _IMAGE_h
#define _IMAGE_h

#include <string>

#include "Dispatcher.h"
#include "base64.h"
#include "Types.h"
#include "JSON.h"

using JSON = nlohmann::json;
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
public:
    JSON id;
    JSON img_json;

    static std::queue< std::pair<std::string, std::string> > requests;
    Image(std::string base64, std::string thumbBase64);

    //gets the underlying JSON as a string to be stored in the JPEG
    std::string getString(); //for JPEG
    JSON getJSON();

    // Remote methods
    void setAccess(std::string targetUser, uint32 view_cnt);
    void recordView(std::string viewer);
    void requestAccess(std::string requester);
 
    JSON executeRPC(std::string name, JSON arguments) override;
};

#endif //_IMAGE_h
