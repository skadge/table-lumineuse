#ifndef NOISE_H
#define NOISE_H

#include <chrono>
#include <memory>

#include "effects.h"
#include "color.h"
#include "ledstrip.h" // for ColorSet

class Perlin; // defined in noise.cpp

class Noise : public Effect {

    double z;
    float scale;
    float speed;
    const Gradient colormap;

    std::shared_ptr<Perlin> perlin; // pointer only to permit forward-declaration of Perlin class

public:
    Noise(const Gradient colormap, 
          float speed = .5f);

    ColorSet step(const std::chrono::milliseconds dt);

};

#endif
