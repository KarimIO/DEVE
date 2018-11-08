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
        nr_views: #,
        img_dat: base64_img_data,
        rights: {

        }
    }
*/

class Image : public RemoteObject{
    JSON img_json;

    public:
    Image(JSON img_json);

    //gets the underlying JSON as a string to be stored in the JPEG
    std::string getString(); //for JPEG
    JSON getJSON();
    void setAccess(uint32 view_cnt);
    void increaseAccess(uint32 view_cnt);
    JSON executeRPC(std::string name, JSON arguments) override;
};

#endif //_IMAGE_h
