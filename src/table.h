#ifndef TABLE_H
#define TABLE_H

#include <tuple>
#include <vector>
#include <array>
#include <memory>
#include <chrono>
#include <json/json.h>
#include <boost/optional.hpp>

#include "light/ledstrip.h"
#include "light/lighteffects.h"

#ifdef WITH_SOUNDS
#include "sound/soundlibrary.h"
#endif

#include "sources.h"

enum TransitionType { FADE = 0  };

class Table {

    TransitionType transition_type;
    std::shared_ptr<Transition> transition;

    std::vector<std::shared_ptr<LightSource>> sources;
    std::array<LED, NB_LEDS> leds;

    Ledstrip ledstrip;


    // we need to store two effects to perform transitions
    std::shared_ptr<Effect> previous_effect;
    std::shared_ptr<Effect> current_effect;

    bool previously_active;

public:

#ifdef WITH_SOUNDS
    // Access to the sound library, used to start/stop sounds
    SoundLibrary sounds;
#endif

    // set this to false to enter 'soft sleep' mode, to true to wake up
    bool active;

    Table(const std::string& sounds_prefix);

    void add_light(std::shared_ptr<LightSource> source) {sources.push_back(source);}

    void step(std::chrono::milliseconds dt = std::chrono::milliseconds(0));

    std::shared_ptr<LightSource> get_source(int id);

    Json::Value sources_to_JSON() const;

    void set_next_effect(std::shared_ptr<Effect> effect);

    // for debugging: prints the list of LEDs values
    void show();

};

#endif
