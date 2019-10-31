#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <array>
#include <tuple>
#include <vector>
#include <iostream>
#include <chrono>
#include <memory>

#include <boost/optional.hpp>

#include "color.h"
#include "lpd8806led.h"

static const char *SPIDEV = "/dev/spidev0.0";
static const int NB_LEDS = 32 * 3 + 1;

typedef std::array<Color, NB_LEDS> ColorSet;

class Ledstrip {

    lpd8806_buffer buf;
    int fd;
    bool initialized;

    ColorSet _colors;

public:
    Ledstrip();
    ~Ledstrip();
    void blank();
    void fill(uint8_t r, uint8_t g, uint8_t b);
    void fill(Color color);
    void set(int idx, uint8_t r, uint8_t g, uint8_t b);
    void set(int idx, Color color);
    void set(const ColorSet& colors);

    /** Set the LED to the provided colors, randomizing the order
     */
    void random_set(const ColorSet& colors);

    /**
     * Returns optionally the current color of the strip, or
     * nothing if the strip currently has several mixed colors
     */
    boost::optional<Color> color() const;

    ColorSet colors() const {return _colors;}
};


#endif
