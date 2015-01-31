#include <thread>
#include <iostream>

#include "table.h"
#include "gradients.h"
#include "fade.h"
#include "effects.h"

using namespace std;

Table::Table(): 
        transition_type(FADE),
        transition(make_shared<Fade>()),
        previous_effect(make_shared<Plain>(Color::black)),
        current_effect(make_shared<Plain>(Color::white)),
        previously_active(true),
        active(true) {

}


void Table::step(chrono::milliseconds dt){

    // the table has just been switched to 'inactive'
    if (previously_active && !active) {
        set_next_effect(make_shared<Plain>(Color::black));
    }
    
    // the table is waking up
    if (!previously_active && active) {
        set_next_effect(previous_effect);
    }


    auto colors = current_effect->step(dt);

    if (!transition->done()) {
        auto previous_colors = previous_effect->step(dt);
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
