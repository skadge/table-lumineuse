#include <iostream>
#include <string>
#include <memory>

#include "http_server/reply.hpp" // for stock reply HTTP 202 'accepted'
#include "http_server/request.hpp"

#include "handler.h"

#include "table.h"

#include "light/fade.h"
#include "light/noise.h"
#include "light/gradients.h"

using namespace std;
using namespace http::server; // boost asio HTTP server

// taken from http://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c
constexpr unsigned int str2int(const char* str, int h = 0)
{
        return !str[h] ? 5381 : (str2int(str, h+1)*33) ^ str[h];
}

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

    if (request_path.find("content=") != std::string::npos)
    {
        string query = request_path.substr(request_path.find("content=") + 8);
        bool parsingSuccessful = reader.parse( query, root );
        if (!parsingSuccessful) {
            cerr << "Invalid command: " << query << endl;
            response = reply::stock_reply(reply::bad_request);
            return;
        }
        response = process_command(root);
    }
    else if (request_path.find("state") != std::string::npos)
    {
        response = process_get_state();
    }
    else
    {
        cerr << "URI must contains a 'content': " << request_path << endl;
        response = reply::stock_reply(reply::bad_request);
        return;
    }



}

reply handler::process_command(const Json::Value& msg)
{
    //cout << "Command successfully parsed:\n";
    //cout << msg;


    // activate the table first, in case it was off (mode 'STOP' may disable it below)
    //table->active = true;

    if (!table->active) {
        cerr << "Table in sleep mode!" << endl;
        return reply::stock_reply(reply::bad_request);
    }

    // Handle the table's mode
    auto mode = msg.get("mode", "PLAIN").asString();

    // ------------ LIGHT EFFECTS ---------------
    if (mode == "PLAIN") {
        auto color = Color(msg["src"]["value"][0u].asInt(),
                           msg["src"]["value"][1u].asInt(),
                           msg["src"]["value"][2u].asInt());
        table->set_next_effect(make_shared<Plain>(color));
        cout << "PLAIN mode, color: " << color << endl;
        return reply::stock_reply(reply::accepted);
    }
    else if (mode == "COLOR_MIX") {
        cout << "Switching to COLOR_MIX mode" << endl;
        cout << "NOT IMPLEMENTED!" << endl;
    }
    else if (mode == "PULSE") {
        cout << "Switching to PULSE mode" << endl;
        cout << "NOT IMPLEMENTED!" << endl;
    }
    else if (mode == "NOISE") {
        auto type = msg["src"]["type"].asString();
        table->set_next_effect(make_shared<Noise>(GRADIENTS.at(type)));
        cout << "NOISE mode, type: " << type << endl;
        return reply::stock_reply(reply::accepted);
    }

#ifdef WITH_SOUNDS
    // ------------ SOUND EFFECTS ---------------
    else if (mode == "SOUND") {
        auto sound = msg["src"]["value"].asString();
        table->sounds.play(sound);
        cout << "Playing sound: " << sound << endl;
        return reply::stock_reply(reply::accepted);
    }
    else if (mode == "BACKGROUND_SOUND") {
        auto sound = msg["src"]["value"].asString();
        table->sounds.background(sound);
        cout << "Playing background sound in loop: " << sound << endl;
        return reply::stock_reply(reply::accepted);
    }
#endif

    else if (mode == "STOP") {
        cout << "STOP mode" << endl;
        table->active = false;
        return reply::stock_reply(reply::accepted);
    }
    else {
        cerr << "Invalid command: Unknown mode " << mode << endl;
        return reply::stock_reply(reply::bad_request);
    }




    auto src = msg["src"];
    int id = src["id"].asInt();
    auto source = table->get_source(id);
    if (source) {
        source->position(src["x"].asInt(), src["y"].asInt(), 0);
        source->color = Color(src["value"][0u].asInt(),
                src["value"][1u].asInt(),
                src["value"][2u].asInt());

        return reply::stock_reply(reply::accepted);
    }
    else
    {
        cerr << "Invalid command: Unknown source " << id << endl;
        return reply::stock_reply(reply::bad_request);
    }
}

reply handler::process_get_state()
{
    return reply::json_reply(table->sources_to_JSON());

}


