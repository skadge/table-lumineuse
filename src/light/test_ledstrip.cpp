#include <unistd.h>
#include <iostream>

#include "ledstrip.h"

using namespace std;

int main() {

    cout << "Connecting to the LED strip via SPI (root required!)..." << endl;
    Ledstrip leds = Ledstrip();
    cout << "Done." << endl;

    ColorSet colors;
    colors.fill({65,16,64});

    cout << "Turning all LEDs red for 1 sec..." << endl;
    leds.set(colors);
    usleep(1000000);
    cout << "Done." << endl;

    while(true){
    cout << "Turning LEDs cyan one after the other..." << endl;
    for (int i = 0; i < NB_LEDS; i++) {
	colors[i] = {31,78,16};
        leds.set(colors);
        usleep(50000);
    }
    for (int i = 0; i < NB_LEDS; i++) {
	colors[i] = {65,16,64};
        leds.set(colors);
        usleep(50000);
    }


    cout << "Turning LEDs cyan one after the other..." << endl;
    for (int i = 0; i < NB_LEDS; i++) {
	colors[i] = {231,3,0};
        leds.set(colors);
        usleep(50000);
    }
    cout << "Done." << endl;
    }

    cout << "Turning all LEDs black for 1 sec..." << endl;
    leds.fill(0,0,0);
    usleep(1000000);
    cout << "Done." << endl;


    cout << "Disconnecting & Exiting..." << endl;
    return 0;
}
