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
    JSON id;
    JSON img_json;

    // RPC methods
    void recordView(std::string viewer);
    void addRequest(std::string requester);
public:
    void recordAccessChange(std::string targetUser, int views);
    std::vector<uint8> getSteganogram();
    std::string getSteganogramBase64();
    // : RemoteObject
    virtual std::string getClassName() override { return "Image"; }
    virtual JSON executeRPC(std::string name, JSON arguments) override;
    virtual JSON getID() override { return id; }
    std::string getThumbnail() { return img_json["thumb"]; }

    // Constructors
    Image(std::string title, std::string base64, std::string thumbBase64);
    Image(JSON id, bool owned, JSON img_json); //changed order to avoid ambiguity, thanks c++

    // Local per-image helpers
    void setAccess(RegistrarArbitration* ra, std::string targetUser, int views);
    void requestAccess(RegistrarArbitration* ra);
    JSON getMetadata();

    // Static helpers
    static std::queue< std::pair<Image*, std::string> > requests;

    static std::mutex psuedoDownloadedMutex;
    static std::map<std::string, Image*> pseudoDownloaded;

    static Image* imageFromSteganogram(JSON id, std::vector<uint8> steganogram);
    static JPEG generateSteganogramJPEG();

    static JSON getList(RegistrarArbitration* ra, std::string user);
    static std::string getImageData(RegistrarArbitration* ra, JSON id);
};

#endif //_IMAGE_h
