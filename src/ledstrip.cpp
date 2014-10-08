
#define MIN(a,b) (((a)<(b))?(a):(b))

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>

#include "ledstrip.h"

using namespace std;

void HSVtoRGB(double h, double s, double v, double *r, double *g, double *b);

Ledstrip::Ledstrip() : initialized(false) {

  int return_value;

  /* Open the device file using Low-Level IO */
  fd = open(SPIDEV,O_WRONLY);
  if(fd<0) {
    fprintf(stderr,"Error %d when trying to open the SPI port: %s\n",errno,strerror(errno));
    return;
  }

  /* Initialize the SPI bus for Total Control Lighting */
  return_value = spi_init(fd);
  if(return_value==-1) {
    fprintf(stderr,"SPI initialization error %d: %s\n",errno, strerror(errno));
    return;
  }

  /* Initialize pixel buffer */
  if(lpd8806_init(&buf, NB_LEDS) < 0) {
    fprintf(stderr,"Pixel buffer initialization error: Not enough memory.\n");
    return;
  }

  initialized = true;

  /* Set the gamma correction factors for each color */
  set_gamma(2.2,2.2,2.2);

  blank();
}

Ledstrip::~Ledstrip() {

    blank();
    lpd8806_free(&buf);
    close(fd);


}

void Ledstrip::blank() {
    if (!initialized) return;

  /* Blank the pixels */
  for(int i=0;i<NB_LEDS;i++) {
    write_gamma_color(&buf.pixels[i],0x00,0x00,0x00);
  }

  send_buffer(fd,&buf);

}


void Ledstrip::fill(uint8_t r, uint8_t g, uint8_t b) {
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
    if (!initialized) return;

    write_gamma_color(&buf.pixels[idx],r,g,b);
    send_buffer(fd,&buf);

}

void Ledstrip::set(int idx, Color color) {
    uint8_t r, g, b;
    std::tie(r, g, b) = color.rgb();
    set(idx, r, g, b);
}

void Ledstrip::set(const std::array<Color, NB_LEDS>& colors) {
    if (!initialized) return;

    uint8_t r, g, b;

    for(int i=0;i<NB_LEDS;i++) {
        std::tie(r, g, b) = colors[i].rgb();
        write_gamma_color(&buf.pixels[i],r,g,b);
    }
    send_buffer(fd,&buf);
}

/* Convert hsv values (0<=h<360, 0<=s<=1, 0<=v<=1) to rgb values (0<=r<=1, etc) */
void HSVtoRGB(double h, double s, double v, double *r, double *g, double *b) {
  int i;
  double f, p, q, t;

  if( s < 1.0e-6 ) {
    /* grey */
    *r = *g = *b = v;
    return;
  }

  h /= 60.0;			        /* Divide into 6 regions (0-5) */
  i = (int)floor( h );
  f = h - (double)i;			/* fractional part of h */
  p = v * ( 1.0 - s );
  q = v * ( 1.0 - s * f );
  t = v * ( 1.0 - s * ( 1.0 - f ) );

  switch( i ) {
    case 0:
      *r = v;
      *g = t;
      *b = p;
      break;
    case 1:
      *r = q;
      *g = v;
      *b = p;
      break;
    case 2:
      *r = p;
      *g = v;
      *b = t;
      break;
    case 3:
      *r = p;
      *g = q;
      *b = v;
      break;
    case 4:
      *r = t;
      *g = p;
      *b = v;
      break;
    default:		// case 5:
      *r = v;
      *g = p;
      *b = q;
      break;
  }
}


ostream& operator<< (ostream &out, const Color &color){
    out << "(" << static_cast<int>(color._r) << ", " 
               << static_cast<int>(color._g) << ", " 
               << static_cast<int>(color._b) << ")";
    return out;
}

/**
 * Currently, simple weighted additive blend
 */
Color Color::from_mix(const vector<tuple<Color, float>>& colors) {

    int r = 0, g = 0, b = 0;

    for (const auto& weighted_color : colors) {
        Color color;
        float weight;
        uint8_t _r, _g, _b;

        tie(color, weight) = weighted_color;
        tie(_r, _g, _b) = color.rgb();

        r += _r * weight;
        g += _g * weight;
        b += _b * weight;

    }

    return Color(MIN(r, 255), MIN(g, 255), MIN(b, 255));
}
