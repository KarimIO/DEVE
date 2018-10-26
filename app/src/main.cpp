#include <pistache/endpoint.h>
#include <JSON.h>

using namespace Pistache;
using namespace nlohmann;


struct HelloHandler : public Http::Handler {
    HTTP_PROTOTYPE(HelloHandler)

    void onRequest(const Http::Request& request, Http::ResponseWriter writer) {    
        json j;
        j["test"] = "Wow";

        writer.send(Http::Code::Ok, j.dump());
    }
};

int main() {
    Http::listenAndServe<HelloHandler>("*:9080");
}