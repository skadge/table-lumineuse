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
    _color = Color::from_mix(lightsources);
}

ostream& operator<< (ostream &out, const LED &led) {
    out << "LED " << led.idx 
              << " (" << (int) led.x << "x" << (int) led.y << "mm, color: " << led._color;

    return out;
}

void Table::step(chrono::milliseconds dt){

    array<Color, NB_LEDS> colors;

    if (mode == PLAIN) {
        Color active_color;
        if (lights.empty()) {
            active_color = Color::white;
        }
        else {
            active_color = lights.back()->color;
        }

        if (active_color != current_plain_color) {
            if (!fading) {
                elapsed_fade = chrono::milliseconds(0);
                fading = true;
            }
            if (elapsed_fade > fade_duration) {
                fading = false;
                current_plain_color = active_color;
            }

            float alpha = (float) elapsed_fade.count() / fade_duration.count();

            ledstrip.fill(current_plain_color.interpolate(active_color, alpha));

            elapsed_fade += dt;
        }

    }

    else if (mode == COLOR_MIX) {
        int i = 0;
        for (auto& led : leds) {
            led.update(lights);
            colors[i] = led.color();
            i++;
        }

        ledstrip.set(colors);
    }

}

void Table::show(){

    for (auto& led : leds) {
        cout << led << endl;
    }
}
