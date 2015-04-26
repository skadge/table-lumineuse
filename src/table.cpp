#include <thread>
#include <iostream>

#include "table.h"
#include "light/gradients.h"
#include "light/fade.h"

using namespace std;

Table::Table(const string& sounds_prefix): 
        transition_type(FADE),
        transition(make_shared<Fade>()),
        previous_effect(make_shared<Plain>(Color::black)),
        current_effect(make_shared<Plain>(Color::white)),
        previously_active(true),
        sounds(sounds_prefix),
        active(true) {

}


void Table::step(chrono::milliseconds dt){

    // the table has just been switched to 'inactive'
    if (previously_active && !active) {
        cout << "Switching off..." << endl;
        set_next_effect(PLAIN_BLACK);
        sounds.stopall();
        previously_active = false;
    }

    // the table is waking up: restore previous effect
    if (!previously_active && active) {
        cout << "Waking up..." << endl;

        // if the current effect is plain black, we
        // restore the previous effect. Otherwise,
        // a new effect has been set, and we have nothing 
        // to do.
        if (current_effect == PLAIN_BLACK) {
            set_next_effect(previous_effect);
        }

        previously_active = true;
    }

    sounds.step(dt);

    // compute the color based on the current effect
    auto colors = current_effect->step(dt);

    if (!transition->done()) {
        // transition from a previous effect still running?
        // Then: 1- compute the colors based on the previous effect
        auto previous_colors = previous_effect->step(dt);
        // 2- blend them and display them
        ledstrip.set(transition->step(previous_colors, colors, dt));
    }
    else ledstrip.set(colors);

}

void Table::set_next_effect(shared_ptr<Effect> effect) {

    previous_effect = current_effect;
    current_effect = effect;

    // initiate the transition
    switch(transition_type) {
        case FADE:
            transition = make_shared<Fade>();
            break;
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
