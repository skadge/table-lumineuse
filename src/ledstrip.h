#include <tuple>
#include <vector>
#include "lpd8806led.h"

static const char *SPIDEV = "/dev/spidev0.0";
static const int NB_LEDS = 32 * 3 + 1;

class Color;

class Ledstrip {

  lpd8806_buffer buf;
  int fd;

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


class Color {

    uint8_t _r, _g, _b;

public:
    Color(): _r(0), _g(0), _b(0) {}
    Color(uint8_t r, uint8_t g, uint8_t b) : _r(r), _g(g), _b(b) {}
    Color(float r, float g, float b) : _r(r * 255), _g(g * 255), _b(b * 255) {}

    std::tuple<uint8_t, uint8_t, uint8_t> rgb() {return std::make_tuple(_r, _g, _b);}
    std::tuple<float, float, float> hsl() {return std::make_tuple(_r, _g, _b);}

    Color mix(const Color& color);
    static Color from_mix(const std::vector<std::tuple<Color, float>>& colors);
};

