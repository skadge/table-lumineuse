#include <string>
#include <iostream>

#include <json/json.h>

#include "http_server/server.hpp"
#include "http_server/request_handler.hpp"
#include "http_server/reply.hpp" // for stock reply HTTP 202 'accepted'

#include "table.h"

using namespace std;
using namespace http::server; // boost asio HHTP server

Json::Value root; // will contains the root value after parsing.
Json::Reader reader;

struct handler;

Table table;
auto src1 = make_shared<LightSource>();

void replaceAll(std::string& str, const std::string& from, const std::string& to) {

    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

struct handler : public request_handler {

    handler () {};

    void handle_request(const request& request, reply& response)
    {
        // Decode url to path.
        std::string request_path;
        if (!url_decode(request.uri, request_path))
        {
            cerr << "Unable to decode URI: " << request_path << endl;
            response = reply::stock_reply(reply::bad_request);
            return;
        }

        // Request path must be absolute and not contain "..".
        if (request_path.empty() || request_path[0] != '/'
            || request_path.find("..") != std::string::npos)
        {
            cerr << "Invalid URI: " << request_path << endl;
            response = reply::stock_reply(reply::bad_request);
            return;
        }

        if (request_path.find("content=") == std::string::npos)
        {
            cerr << "URI must contains a 'content': " << request_path << endl;
            response = reply::stock_reply(reply::bad_request);
            return;
        }



        string query = request_path.substr(request_path.find("content=") + 8);
        bool parsingSuccessful = reader.parse( query, root );
        if (!parsingSuccessful) {
            cerr << "Invalid command: " << query << endl;
            response = reply::stock_reply(reply::bad_request);
            return;
        }

        //cout << "Command successfully parsed:\n";
        //cout << root;

        Json::Value src = root["src"];
        src1->update(src["x"].asInt(), src["y"].asInt(), 0);
        src1->color = Color(src["value"][0].asInt(),
                            src["value"][1].asInt(),
                            src["value"][2].asInt());
        table.step();

        response = reply::stock_reply(reply::accepted);
    }

};

int main(int arg, char * argv[]) {


    table.add_light(src1);
    src1->color = Color(0,255,0);
    src1->update(500, 250, 0);

    //auto red = make_shared<LightSource>();
    //table.add_light(red);
    //red->color = Color(255,0,0);
    //red->update(900, 34, 0);

    table.step();
    table.show();

    cout << "Listening for clients..." << endl;
    http::server::server<handler> s("0.0.0.0", "8080");
    s.run();
}
