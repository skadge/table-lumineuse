#include <chrono>
#include <iostream>
#include <thread>

#include "ledstrip.h"
#include "lighteffects.h"
#include "fade.h"

using namespace std;

static const double MAIN_LOOP_FPS=60;
static const chrono::milliseconds main_loop_duration{(int) (1000/MAIN_LOOP_FPS)};


void fade(Ledstrip* ledstrip, shared_ptr<Effect> coloreffect1, shared_ptr<Effect> coloreffect2) {

    std::chrono::milliseconds elapsed_time{0};
    std::chrono::milliseconds dt{0};

    auto transition = Fade();

    cout << "Fading over 2sec..." << endl;

    auto start = chrono::high_resolution_clock::now();
    auto intermediate = start, end = start;

    while (elapsed_time.count() < 2000) {

        ledstrip->set(transition.step(coloreffect1->step(dt), coloreffect2->step(dt), dt));

        intermediate = chrono::high_resolution_clock::now();
        this_thread::sleep_for(main_loop_duration - (intermediate - start));

        end = chrono::high_resolution_clock::now();
        dt = chrono::duration_cast<chrono::milliseconds>(end-start);

        elapsed_time += dt;

        start = chrono::high_resolution_clock::now();

    }


}

int main() {

    cout << "Connecting to the LED strip via SPI (root required!)..." << endl;
    Ledstrip leds = Ledstrip();
    cout << "Done." << endl;

    cout << "Plain black for 1sec..." << endl;
    leds.set(PLAIN_BLACK->step());
    this_thread::sleep_for(std::chrono::milliseconds{1000});
    cout << "Plain white for 1sec..." << endl;
    leds.set(PLAIN_WHITE->step());
    this_thread::sleep_for(std::chrono::milliseconds{1000});
    cout << "Plain black for 1sec..." << endl;
    leds.set(PLAIN_BLACK->step());
    this_thread::sleep_for(std::chrono::milliseconds{1000});

    auto effect1 = std::make_shared<Plain>(Color::red);
    auto effect2 = std::make_shared<Plain>(Color::blue);

    fade(&leds, PLAIN_BLACK, effect1);
    fade(&leds, effect1, effect2);
    fade(&leds, effect2, PLAIN_BLACK);

    cout << "Disconnecting & Exiting..." << endl;
    return 0;
}
