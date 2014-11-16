#include <tuple>
#include <vector>
#include <iostream>

#include "color.h"
#include "lpd8806led.h"

static const char *SPIDEV = "/dev/spidev0.0";
static const int NB_LEDS = 32 * 3 + 1;

class Ledstrip {

    lpd8806_buffer buf;
    int fd;
    bool initialized;

    bool running_animation;

public:
    Ledstrip();
    ~Ledstrip();
    void blank();
    void fill(uint8_t r, uint8_t g, uint8_t b);
    void fill(Color color);
    void set(int idx, uint8_t r, uint8_t g, uint8_t b);
    void set(int idx, Color color);
    void set(const std::array<Color, NB_LEDS>& colors);
};


