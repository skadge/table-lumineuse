#include "table.h"

#include <iostream>

using namespace std;

//Color Color::mix(const Color& color) {
//
//}

int LED::last_id = 0;

LED::LED() {
    idx = last_id;
    last_id++;

    /* Compute the LED location based on the table dimension
     *
     * (assumes a rectangular table, with LEDs on the perimeter)
     */
    float nbleds = (float) NB_LEDS;
    float total_lenght = 2 * TABLE_WIDTH + 2 * TABLE_HEIGHT;

    float ledx = total_lenght * idx / nbleds;

    if (ledx < TABLE_WIDTH) {
        x = ledx; y = 0;
    }
    else if (ledx < TABLE_WIDTH + TABLE_HEIGHT) {
        x = TABLE_WIDTH ; y = ledx - TABLE_WIDTH;
    }
    else if (ledx < 2 * TABLE_WIDTH + TABLE_HEIGHT) {
        x = TABLE_WIDTH - (ledx - (TABLE_WIDTH + TABLE_HEIGHT)) ; y = TABLE_HEIGHT;
    }
    else {
        x = 0; y = TABLE_HEIGHT - (ledx - (2 * TABLE_WIDTH + TABLE_HEIGHT));
    }
    ///////

    cout << "Created LED " << idx << " @(" << x << "x" << y << "mm)" << endl;
}

void LED::update(const std::vector<LightSource>& sources){

    vector<tuple<Color, float>> lightsources;

    for (auto& source : sources) {
        auto dist = distance(source);
        if (dist < MAX_COLOR_DISTANCE) {
            lightsources.push_back(
                    make_tuple(source.color(),
                               1. - dist / MAX_COLOR_DISTANCE)
                    );
        }
    }

    //_color = Color::from_mix(lightsources);
}


void Table::step(){

    array<Color, NB_LEDS> colors;

    int i = 0;
    for (auto& led : leds) {
        led.update(lights);
        colors[i] = led.color();
        i++;
    }

    ledstrip.set(colors);

}
