#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "http_server/server.hpp"
#include "handler.h"
#include "table.h"
#include "gpio.h"

using namespace std;

static const double MAIN_LOOP_FPS=60;
static const chrono::milliseconds main_loop_duration{(int) (1000/MAIN_LOOP_FPS)};

static const int POWERSWITCH_GPIO=27;

bool running = true;

bool on_raspi = true;

bool last_switch_state = false;

// returns true if the switch has been pressed, eliminating bouncing
bool switchPressed() {

    if (!on_raspi) return false;

    bool res = false;

    auto state = (GPIORead(POWERSWITCH_GPIO) == 1);

    if (state && (last_switch_state != state)) {
        res = true;
    }

    last_switch_state = state;
    return res;
}


int main(int arg, char * argv[]) {

    /*
     * Enable and configure  GPIO pin 27 (ie, GPIO 2 on the board)
     */
    if (-1 == GPIOExport(POWERSWITCH_GPIO)
        || -1 == GPIODirection(POWERSWITCH_GPIO, IN)) {
        cout << "Cannot setup the GPIOs. Probably not running on the Raspi. I won't use the switch." << endl;
        on_raspi = false;
    }

    auto table = make_shared<Table>();
    auto src1 = make_shared<LightSource>(1);

    table->add_light(src1);
    src1->color = Color(255,255,255);
    //src1->position(500, 250, 0);

    table->step();

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
        cout << "Received a quit signal..." << endl;
        running = false;
      });


    auto last_mode = PLAIN;

    auto start = chrono::high_resolution_clock::now();
    auto intermediate = start, end = start;
    chrono::milliseconds dt{0};

    cout << "Entering main loop." << endl;
    while (running) {

        s.poll();

        // 'active' toggles when the power switch is pressed (ie, 'soft' poweroff)
        // active = active XOR switch_pressed
        table->active = (table->active != switchPressed());

        if (table->active) {
            // if the table was previously inactive and no new mode has been
            // set, reset the mode to the last one.
            if (table->mode == CLOSING) {
                cout << "Waking up! Continuing what I was doing..." << endl;
                table->mode = last_mode;
            }

            table->step(dt);
        }
        else {
            if (table->mode != CLOSING) {
                cout << "Going to 'soft' sleep" << endl;
                last_mode = table->mode;
                table->mode = CLOSING;
                table->step(dt); // blocks until LEDs fade out
            }
        }

        intermediate = chrono::high_resolution_clock::now();
        this_thread::sleep_for(main_loop_duration - (intermediate - start));

        end = chrono::high_resolution_clock::now();
        dt = chrono::duration_cast<chrono::milliseconds>(end-start);

        start = chrono::high_resolution_clock::now();
    }


    cout << "Fading off the table... (this may take a few seconds...)" << endl;

    table->mode = CLOSING;
    table->step(dt); // the last stepping blocks until LEDs fade out

    cout << "Quitting now." << endl;
}
