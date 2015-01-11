#ifndef EFFECTS_H
#define EFFECTS_H

#include <chrono>

#include "color.h"

static const std::chrono::milliseconds FADE_DURATION{2000}; //ms
static const std::chrono::milliseconds PULSE_DURATION{6000}; //ms

class Ledstrip;

class Effect {

protected:
    bool _running;
    bool _done;

public:
    Effect() : _running(false), _done(false) {}

    // virtual destructor -> polymorphic class
    virtual ~Effect() {}


    virtual void step(Ledstrip& leds,
                      const std::chrono::milliseconds dt) = 0;

    bool running() const {return _running;}
    bool done() const {return _done;}
};

class Fade : public Effect {

    Color target_color;
    std::chrono::milliseconds fade_duration;
    std::chrono::milliseconds elapsed_fade;


public:
    /*
     *  FADE:
     *
     *  Fades from the current color (or mix of colors) to the target color.
     *  Fade::step(leds, dt) be called at every loop, passing the elapsed time
     *  dt since last call.
     *
     *  Use Fade::done() to know when the effect has completed.
     *
     *  The total duration of the fade effect can be defined via fade_duration
     *  and defaults to FADE_DURATION.
     */
    Fade(const Color target_color, 
         const std::chrono::milliseconds fade_duration = FADE_DURATION);


    void step(Ledstrip& leds,
              const std::chrono::milliseconds dt);


};

#endif
