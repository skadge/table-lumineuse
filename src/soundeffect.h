#ifndef SOUND_EFFECTS_H
#define SOUND_EFFECTS_H

#include <chrono>
#include <memory>

#include "sound.h"

static const std::chrono::milliseconds CROSSFADE_DURATION{2000}; //ms

class Crossfade {

protected:
    std::chrono::milliseconds fade_duration;
    std::chrono::milliseconds elapsed_fade;

    bool _done;

public:
    Crossfade(const std::chrono::milliseconds fade_duration = CROSSFADE_DURATION);

    void step(SoundSource& start_sound,
              SoundSource& end_sound,
              const std::chrono::milliseconds dt);

    bool done() const {return _done;}
};

#endif
