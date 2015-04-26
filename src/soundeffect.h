#ifndef SOUND_EFFECTS_H
#define SOUND_EFFECTS_H

#include <chrono>
#include <memory>

#include "sound.h"

static const std::chrono::milliseconds CROSSFADE_DURATION{2000}; //ms

class Crossfade {

protected:
    std::shared_ptr<SoundSource> start_sound;
    std::shared_ptr<SoundSource> end_sound;

    std::chrono::milliseconds fade_duration;
    std::chrono::milliseconds elapsed_fade;

    bool _done;

public:
    Crossfade(const std::chrono::milliseconds fade_duration = CROSSFADE_DURATION);

    void fade_to(std::shared_ptr<SoundSource> to);

    void step(const std::chrono::milliseconds dt);

    bool done() const {return _done;}
};

#endif
