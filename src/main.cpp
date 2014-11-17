#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "http_server/server.hpp"
#include "handler.h"
#include "table.h"

using namespace std;

static const double MAIN_LOOP_FPS=60;
static const chrono::milliseconds main_loop_duration{(int) (1000/MAIN_LOOP_FPS)};


bool running = true;


int main(int arg, char * argv[]) {

    auto table = make_shared<Table>();
    auto src1 = make_shared<LightSource>(1);

    table->add_light(src1);
    src1->color = Color(255,255,255);
    //src1->position(500, 250, 0);

    //auto red = make_shared<LightSource>();
    //table.add_light(red);
    //red->color = Color(255,0,0);
    //red->update(900, 34, 0);

    table->step();
    //table.show();

    cout << "Listening for clients..." << endl;
    http::server::server<handler> s("0.0.0.0", "8080");
    s.request_handler.set_table(table);


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

        table->step(dt);

        intermediate = chrono::high_resolution_clock::now();
        
        this_thread::sleep_for(main_loop_duration - (intermediate - start));
        end = chrono::high_resolution_clock::now();
        dt = chrono::duration_cast<chrono::milliseconds>(end-start);

        start = chrono::high_resolution_clock::now();
    }

    table->mode = CLOSING;
    table->step(dt); // the last stepping blocks until LEDs fade out

}
