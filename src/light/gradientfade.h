#ifndef GRADIENTFADE_H
#define GRADIENTFADE_H

#include <chrono>
#include <memory>

#include "lighteffects.h"
#include "color.h"
#include "ledstrip.h" // for ColorSet

static const std::chrono::milliseconds CYCLE_DURATION{15000}; //ms

class GradientFade : public Effect {

    std::chrono::milliseconds cycle_duration;
    std::chrono::milliseconds elapsed_fade;
    const Gradient colormap;

public:
    GradientFade(const Gradient colormap, std::chrono::milliseconds cycle_duration = CYCLE_DURATION);

    ColorSet step(const std::chrono::milliseconds dt);

};

#endif
