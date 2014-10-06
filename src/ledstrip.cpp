
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>

#include "ledstrip.h"

void HSVtoRGB(double h, double s, double v, double *r, double *g, double *b);

Ledstrip::Ledstrip() {

  int return_value;

  /* Open the device file using Low-Level IO */
  fd = open(SPIDEV,O_WRONLY);
  if(fd<0) {
    fprintf(stderr,"Error %d: %s\n",errno,strerror(errno));
    exit(1);
  }

  /* Initialize the SPI bus for Total Control Lighting */
  return_value = spi_init(fd);
  if(return_value==-1) {
    fprintf(stderr,"SPI initialization error %d: %s\n",errno, strerror(errno));
    exit(1);
  }

  /* Initialize pixel buffer */
  if(lpd8806_init(&buf, NB_LEDS) <0) {
    fprintf(stderr,"Pixel buffer initialization error: Not enough memory.\n");
    exit(1);
  }

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
  /* Blank the pixels */
  for(int i=0;i<NB_LEDS;i++) {
    write_gamma_color(&buf.pixels[i],0x00,0x00,0x00);
  }

  send_buffer(fd,&buf);

}


void Ledstrip::fill(int r, int g, int b) {
  for(int i=0;i<NB_LEDS;i++) {
    write_gamma_color(&buf.pixels[i],r,g,b);
  }

  send_buffer(fd,&buf);

}

void Ledstrip::set(int idx, int r, int g, int b) {
    write_gamma_color(&buf.pixels[idx],r,g,b);
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
