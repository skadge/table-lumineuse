#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>

#include <json/json.h>

#include "http_server/server.hpp"
#include "http_server/request_handler.hpp"
#include "http_server/reply.hpp" // for stock reply HTTP 202 'accepted'
#include <boost/asio.hpp>

#include "table.h"

using namespace std;
using namespace http::server; // boost asio HHTP server

static const double MAIN_LOOP_FPS=60;
static const chrono::milliseconds main_loop_duration{(int) (1000/MAIN_LOOP_FPS)};


Json::Value root; // will contains the root value after parsing.
Json::Reader reader;

struct handler;

Table table;
auto src1 = make_shared<LightSource>();

bool running = true;

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
        src1->position(src["x"].asInt(), src["y"].asInt(), 0);
        src1->color = Color(src["value"][0].asInt(),
                            src["value"][1].asInt(),
                            src["value"][2].asInt());

        response = reply::stock_reply(reply::accepted);
    }

};

int main(int arg, char * argv[]) {



    table.add_light(src1);
    src1->color = Color(255,255,255);
    //src1->position(500, 250, 0);

    //auto red = make_shared<LightSource>();
    //table.add_light(red);
    //red->color = Color(255,0,0);
    //red->update(900, 34, 0);

    table.step();
    //table.show();

    cout << "Listening for clients..." << endl;
    http::server::server<handler> s("0.0.0.0", "8080");

    // boost.asio already captures termination signals to stop the server.
    // I add here my own handler to also quit the main loop
    boost::asio::signal_set signals(s.io_service);
    signals.add(SIGINT);
    signals.add(SIGTERM);
    signals.add(SIGQUIT);

    signals.async_wait(
      [](boost::system::error_code /*ec*/, int /*signo*/)
      {
        running = false;
      });


    auto start = chrono::high_resolution_clock::now();
    auto intermediate = start, end = start;
    chrono::milliseconds dt{0};

    while (running) {

        s.poll();

        table.step(dt);

        intermediate = chrono::high_resolution_clock::now();
        
        this_thread::sleep_for(main_loop_duration - (intermediate - start));
        end = chrono::high_resolution_clock::now();
        dt = chrono::duration_cast<chrono::milliseconds>(end-start);

        start = chrono::high_resolution_clock::now();
    }

}
