#include "lpd8806led.h"

static const char *SPIDEV = "/dev/spidev0.0";
static const int NB_LEDS = 32 * 3 + 1;

class Ledstrip {

  lpd8806_buffer buf;
  int fd;

public:
    Ledstrip();
    ~Ledstrip();
    void blank();
    void fill(int r, int g, int b);
    void set(int idx, int r, int g, int b);
};
