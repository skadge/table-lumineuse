#include <boost/network/uri.hpp>
#include <boost/network/protocol/http/server.hpp>
#include <string>
#include <iostream>

#include <json/json.h>

#include "table.h"

using namespace std;

namespace http = boost::network::http;
namespace uri = boost::network::uri;

Json::Value root; // will contains the root value after parsing.
Json::Reader reader;

struct handler;
typedef http::server<handler> http_server;

void replaceAll(std::string& str, const std::string& from, const std::string& to) {

    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

struct handler {
    void operator() (http_server::request const &request,
                     http_server::response &response) {
        string ip = source(request);
        string path = request.destination;
        string query = path.substr(path.find("content=") + 8);
        replaceAll(query,"%22","\"");
        replaceAll(query,"%20"," ");
        bool parsingSuccessful = reader.parse( query, root );
        if (!parsingSuccessful) {
            cerr << "Invalid command: " << query << endl;
            return;
        }

        cout << root;

        response = http_server::response::stock_reply(
            http_server::response::ok, "Hello, world!");
    }

    void log(http_server::string_type const &info) {
        std::cerr << "ERROR: " << info << '\n';
    }
};

int main(int arg, char * argv[]) {

    Table table;
    auto orange = make_shared<LightSource>();
    table.add_light(orange);
    orange->color = Color(255,255,0);
    orange->update(800, 200, 0);

    auto red = make_shared<LightSource>();
    table.add_light(red);
    red->color = Color(255,0,0);
    red->update(900, 34, 0);

    table.step();
    table.show();

    handler handler_;
    http_server::options options(handler_);
    http_server server_(
        options.address("0.0.0.0")
               .port("8000"));
    server_.run();
}
