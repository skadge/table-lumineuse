#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <array>
#include <tuple>
#include <vector>
#include <iostream>
#include <chrono>

#include <boost/optional.hpp>

#include "color.h"
#include "lpd8806led.h"

static const std::chrono::milliseconds FADE_DURATION{1000}; //ms
static const std::chrono::milliseconds PULSE_DURATION{5000}; //ms

static const char *SPIDEV = "/dev/spidev0.0";
static const int NB_LEDS = 32 * 3 + 1;

class Ledstrip {

    lpd8806_buffer buf;
    int fd;
    bool initialized;

    std::array<Color, NB_LEDS> colors;

    bool running_effect;

    std::chrono::milliseconds fade_duration;
    std::chrono::milliseconds elapsed_fade;


public:
    Ledstrip();
    ~Ledstrip();
    void blank();
    void fill(uint8_t r, uint8_t g, uint8_t b);
    void fill(Color color);
    void set(int idx, uint8_t r, uint8_t g, uint8_t b);
    void set(int idx, Color color);
    void set(const std::array<Color, NB_LEDS>& colors);

    /**
     * Returns optionally the current color of the strip, or
     * nothing if the strip currently has several mixed colors
     */
    boost::optional<Color> color() const;
    
    /**
     * Returns true is an effect (like fading) is currently being performed
     */
    bool is_effect_running() const {return running_effect;}

    /***********************************************
     * EFFECTS
     **********************************************/

    /** Fades from the current color (or mix of colors) to the target color.
     *  Must be called at every loop, passing the elapsed time dt since last call.
     *
     *  Ledstrip::is_effect_running() returns false once the effect has completed.
     *
     *  The total duration of the fade effect can be defined via fade_duration
     *  and defaults to FADE_DURATION. Note that this value is only used when the
     *  fading effect is initiated (ie, you can not change the duration of the
     *  effect *during* the effect).
     */
    void fade(const Color target_color, 
              const std::chrono::milliseconds dt, 
              const std::chrono::milliseconds fade_duration = FADE_DURATION);

};


#endif
