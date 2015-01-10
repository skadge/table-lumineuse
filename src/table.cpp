#include <thread>

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


Color Table::getTargetColor() {
    if (sources.empty()) {
        return Color::white;
    }
    else {
        return sources.back()->color;
    }

}

void Table::step(chrono::milliseconds dt){

    array<Color, NB_LEDS> colors;

    if (mode == PLAIN) {
        ledstrip.fade(getTargetColor(), dt);
    }

    else if (mode == PULSE) {

        if (pulse_up) {
            ledstrip.fade(getTargetColor(), dt, PULSE_DURATION);
            if (!ledstrip.is_effect_running()) pulse_up = false;
        }
        else {
            ledstrip.fade(Color::black, dt);
            if (!ledstrip.is_effect_running()) pulse_up = true;
        }
    }

    else if (mode == COLOR_MIX) {
        int i = 0;
        for (auto& led : leds) {
            led.update(sources);
            colors[i] = led.color;
            i++;
        }

        ledstrip.set(colors);
    }

    else if (mode == CLOSING) {
        cout << "Closing!" << endl;

        // block during the fade out.
        auto elapsed_fade = chrono::milliseconds(0);
        auto dt = chrono::milliseconds(16);

        while (elapsed_fade < FADE_DURATION) {

            ledstrip.fade(Color::black, dt);

            elapsed_fade += dt;
            this_thread::sleep_for(dt);
        }
        cout << "Bye bye!" << endl;
    }

}

shared_ptr<LightSource> Table::get_source(int id) {
    for (auto source : sources) {
        if (source->id() == id) return source;
    }
    return nullptr;
}

void Table::show(){

    for (auto& led : leds) {
        cout << led << endl;
    }
}

Json::Value Table::sources_to_JSON() const {

    Json::Value res;

    for (auto source : sources) {
        res["src"]["id"] = source->id();
        res["src"]["type"] = "color";
        res["src"]["x"] = source->x();
        res["src"]["y"] = source->y();
        res["src"]["theta"] = source->theta();
        uint8_t r, g, b;
        tie(r,g,b) = source->color.rgb();
        res["src"]["value"] = Json::arrayValue;
        res["src"]["value"].append(r);
        res["src"]["value"].append(g);
        res["src"]["value"].append(b);

    }

    return res;


}
