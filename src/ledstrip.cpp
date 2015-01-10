
#define MIN(a,b) (((a)<(b))?(a):(b))

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "ledstrip.h"

using namespace std;

Ledstrip::Ledstrip() : 
    initialized(false),
    running_effect(false),
    elapsed_fade(0) {

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
            if(lpd8806_init(&buf, NB_LEDS + 1) < 0) {
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

    blank();
    lpd8806_free(&buf);
    close(fd);


}

void Ledstrip::blank() {

    colors.fill(Color()); // all LEDs are black

    if (!initialized) return;

    /* Blank the pixels */
    for(int i=0;i<NB_LEDS;i++) {
        write_gamma_color(&buf.pixels[i],0x00,0x00,0x00);
    }

    send_buffer(fd,&buf);

}


void Ledstrip::fill(uint8_t r, uint8_t g, uint8_t b) {

    colors.fill(Color(r, g, b));

    if (!initialized) return;

    for(int i=0;i<NB_LEDS;i++) {
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

    colors[idx] = Color(r, g, b);

    if (!initialized) return;

    write_gamma_color(&buf.pixels[idx],r,g,b);
    send_buffer(fd,&buf);

}

void Ledstrip::set(int idx, Color color) {
    uint8_t r, g, b;
    tie(r, g, b) = color.rgb();
    set(idx, r, g, b);
}

void Ledstrip::set(const std::array<Color, NB_LEDS>& colorarray) {

    colors = colorarray;

    if (!initialized) return;

    uint8_t r, g, b;

    for(int i=0;i<NB_LEDS;i++) {
        tie(r, g, b) = colors[i].rgb();
        write_gamma_color(&buf.pixels[i],r,g,b);
    }
    send_buffer(fd,&buf);

}

boost::optional<Color> Ledstrip::color () const {

    // TODO: implement shortcuts: after a 'Ledstrip::fill' for instance, we know
    // all the colors are the same.

    auto col = colors[0];
    for (int i = 1; i < NB_LEDS; i++) {
        if (colors[i] != col) return boost::none;
    }
    return col;

}

void Ledstrip::fade(Color target_color, 
                    chrono::milliseconds dt,
                    chrono::milliseconds duration) {

    if (!running_effect) {
        fade_duration = duration;
        running_effect = true;
    }

    auto current_color = color();

    if (current_color && target_color == current_color) {
        running_effect = false;
        return;
    }

    float alpha = (float) elapsed_fade.count() / fade_duration.count();

    if (alpha >= 1.f) {
        fill(target_color);
        running_effect = false;
        elapsed_fade = chrono::milliseconds(0);
        return;
    }

    elapsed_fade += dt;

    array<Color, NB_LEDS> next_colors;
    for (int i = 0; i < NB_LEDS; i++) {
        next_colors[i] = colors[i].interpolate(target_color, alpha);
    }

    set(next_colors);

}


