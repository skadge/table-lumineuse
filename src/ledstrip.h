#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <array>
#include <tuple>
#include <vector>
#include <iostream>
#include <chrono>
#include <memory>

#include <boost/optional.hpp>

#include "effects.h"
#include "color.h"
#include "lpd8806led.h"

static const char *SPIDEV = "/dev/spidev0.0";
static const int NB_LEDS = 32 * 3 + 1;

class Ledstrip {

    lpd8806_buffer buf;
    int fd;
    bool initialized;

    std::array<Color, NB_LEDS> _colors;

    std::vector<std::shared_ptr<Effect>> _effects;

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

    std::array<Color, NB_LEDS> colors() const {return _colors;}

    /** runs the pending effects, if any. Must be called at every loop
     */
    void step(std::chrono::milliseconds dt);

    /**
     * Returns true is an effect (like fading) is currently being performed
     */
    bool is_effect_running() const;

    /***********************************************
     * EFFECTS
     **********************************************/

    /** Adds an effect. The effect is actually 'played' when calling
     * Ledstrip::step(dt)
     */
    void effect(std::shared_ptr<Effect> effect);

};


#endif
