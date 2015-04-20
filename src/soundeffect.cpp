#include <iostream>

#include "soundeffect.h"

using namespace std;

Crossfade::Crossfade(chrono::milliseconds fade_duration) :
    fade_duration(fade_duration),
    elapsed_fade(0),
    _done(false) {};



void Crossfade::step(SoundSource& start_sound,
                     SoundSource& end_sound,
                     const std::chrono::milliseconds dt){

    float alpha = (float) elapsed_fade.count() / fade_duration.count();

    if (alpha >= 1.f) {
        _done = true;
        end_sound.volume(1.f);
        start_sound.stop();
        cout << "Sound cross-fading complete." << endl;
        return;
    }

    elapsed_fade += dt;

    start_sound.volume(1.f - alpha);
    end_sound.volume(alpha);

    return;
}

