#include "sources.h"

#include "ledstrip.h" // for NB_LEDS

using namespace std;

int LED::last_id = 0;

LED::LED() {
    idx = last_id;
    last_id++;

    /* Compute the LED location based on the table dimension
     *
     * (assumes a rectangular table, with LEDs on the perimeter)
     */
    int offset = 15; // offset = 0 assumes led 0 in top left corner. offset = 30 assumes led 0 in top right corner, etc
    float nbleds = (float) NB_LEDS;
    float total_lenght = 2 * TABLE_WIDTH + 2 * TABLE_HEIGHT;

    float ledx = total_lenght * ((idx + offset) % NB_LEDS) / nbleds;

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
}

void LED::update(const std::vector<shared_ptr<LightSource>> sources){

    if (sources.size() == 0) return;

    vector<tuple<Color, float>> lightsources;

    for (auto source : sources) {
        auto dist = distance(*source);
        if (dist < MAX_COLOR_DISTANCE) {
            lightsources.push_back(
                    make_tuple(source->color,
                               1. - dist / MAX_COLOR_DISTANCE)
                    );
        }
    }

    if (lightsources.size() == 0) return;
    color = Color::from_mix(lightsources);
}

ostream& operator<< (ostream &out, const LED &led) {
    out << "LED " << led.idx 
              << " (" << (int) led.x << "x" << (int) led.y << "mm, color: " << led.color;

    return out;
}


