
#define MIN(a,b) (((a)<(b))?(a):(b))

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

#include "ledstrip.h"

using namespace std;

Ledstrip::Ledstrip() : 
    initialized(false)
{

    int return_value;

    /* Open the device file using Low-Level IO */
    fd = open(SPIDEV,O_WRONLY);
    if(fd<0) {
        fprintf(stderr,"Error %d when trying to open the SPI port: %s\n",errno,strerror(errno));
    }
    else {

        /* Initialize the SPI bus for Total Control Lighting */
        return_value = spi_init(fd);
        if(return_value==-1) {
            fprintf(stderr,"SPI initialization error %d: %s\n",errno, strerror(errno));
        }
        else {

            /* Initialize pixel buffer */
            if(lpd8806_init(&buf, NB_LEDS + LEDS_OFFSET + 1) < 0) {
                fprintf(stderr,"Pixel buffer initialization error: Not enough memory.\n");
            }
            else {

                initialized = true;

                /* Set the gamma correction factors for each color */
                set_gamma(2.2,2.2,2.2);

            }
        }
    }

    blank();

}

Ledstrip::~Ledstrip() {

    if (!initialized) return;

    blank();
    lpd8806_free(&buf);
    close(fd);


}

void Ledstrip::blank() {

    _colors.fill(Color()); // all LEDs are black

    if (!initialized) return;

    /* Blank the pixels */
    for(int i=0; i<NB_LEDS+LEDS_OFFSET; i++) {
        write_gamma_color(&buf.pixels[i],0x00,0x00,0x00);
    }

    send_buffer(fd,&buf);

}


void Ledstrip::fill(uint8_t r, uint8_t g, uint8_t b) {

    _colors.fill(Color(r, g, b));

    if (!initialized) return;

    for(int i=LEDS_OFFSET; i<NB_LEDS+LEDS_OFFSET; i++) {
        write_gamma_color(&buf.pixels[i],r,g,b);
    }

    send_buffer(fd,&buf);

}

void Ledstrip::fill(Color color) {
    uint8_t r, g, b;
    std::tie(r, g, b) = color.rgb();
    fill(r, g, b);
}

void Ledstrip::set(int idx, uint8_t r, uint8_t g, uint8_t b) {

    if (idx >= NB_LEDS) return;

    _colors[idx] = Color(r, g, b);

    if (!initialized) return;

    write_gamma_color(&buf.pixels[idx+LEDS_OFFSET],r,g,b);
    send_buffer(fd,&buf);

}

void Ledstrip::set(int idx, Color color) {
    uint8_t r, g, b;
    tie(r, g, b) = color.rgb();
    set(idx, r, g, b);
}

void Ledstrip::set(const ColorSet& colorarray) {

    if (colorarray == _colors) return;

    _colors = colorarray;

    if (!initialized) return;

    uint8_t r, g, b;

    for(int i=0; i<NB_LEDS; i++) {
        tie(r, g, b) = _colors[i].rgb();
        write_gamma_color(&buf.pixels[i+LEDS_OFFSET],r,g,b);
    }
    send_buffer(fd,&buf);

}

void Ledstrip::random_set(const ColorSet& colorarray) {

    ColorSet random(colorarray);
    random_shuffle(std::begin(random), std::end(random));
    set(random);
}

boost::optional<Color> Ledstrip::color () const {

    // TODO: implement shortcuts: after a 'Ledstrip::fill' for instance, we know
    // all the colors are the same.

    auto col = _colors[0];
    for (int i = 1; i < NB_LEDS; i++) {
        if (_colors[i] != col) return boost::none;
    }
    return col;

}

