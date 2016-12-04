#include <unistd.h>
#include <iostream>

#include "ledstrip.h"

using namespace std;

int main() {

    cout << "Connecting to the LED strip via SPI (root required!)..." << endl;
    Ledstrip leds = Ledstrip();
    cout << "Done." << endl;

    cout << "Turning all LEDs red for 1 sec..." << endl;
    leds.fill(255,0,0);
    usleep(1000000);
    cout << "Done." << endl;

    cout << "Turning LEDs cyan one after the other..." << endl;
    for (int i = 0; i < NB_LEDS; i++) {
        leds.set(i, 0, 128, 128);
        usleep(100000);
    }
    cout << "Done." << endl;

    cout << "Turning all LEDs black for 1 sec..." << endl;
    leds.fill(0,0,0);
    usleep(1000000);
    cout << "Done." << endl;


    cout << "Disconnecting & Exiting..." << endl;
    return 0;
}
