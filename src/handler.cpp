#include <iostream>
#include <string>

#include "http_server/reply.hpp" // for stock reply HTTP 202 'accepted'
#include "http_server/request.hpp"

#include "handler.h"

#include "table.h"

using namespace std;
using namespace http::server; // boost asio HHTP server


//void replaceAll(std::string& str, const std::string& from, const std::string& to) {
//
//    size_t start_pos = 0;
//    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
//        str.replace(start_pos, from.length(), to);
//        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
//    }
//}


void handler::handle_request(const request& request, reply& response)
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
    int id = src["id"].asInt();
    auto source = table->get_source(id);
    source->position(src["x"].asInt(), src["y"].asInt(), 0);
    source->color = Color(src["value"][0u].asInt(),
            src["value"][1u].asInt(),
            src["value"][2u].asInt());

    response = reply::stock_reply(reply::accepted);
}


