#include <chrono>
#include <memory>

#include "lighteffects.h"
#include "ledstrip.h" // for ColorSet

static const std::chrono::milliseconds FADE_DURATION{2000}; //ms

class Fade : public Transition {

    std::chrono::milliseconds fade_duration;
    std::chrono::milliseconds elapsed_fade;

public:

    Fade(const std::chrono::milliseconds fade_duration = FADE_DURATION);

    ColorSet step(const ColorSet& start_colors,
              const ColorSet& end_colors,
              const std::chrono::milliseconds dt);

};


