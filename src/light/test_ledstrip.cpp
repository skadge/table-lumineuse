#include <unistd.h>

#include "ledstrip.h"

int main() {

    Ledstrip leds = Ledstrip();

    leds.fill(255,0,0);

    for (int i = 0; i < NB_LEDS; i++) {
        leds.set(i, 0, 128, 128);
        usleep(50000);
    }

    return 0;
}
