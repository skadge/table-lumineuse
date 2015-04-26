#include <iostream>

#include "soundeffect.h"

using namespace std;

Crossfade::Crossfade(const chrono::milliseconds fade_duration) :
            start_sound(SILENCE),
            fade_duration(fade_duration),
            elapsed_fade(0),
            _done(true) {};


void Crossfade::fade_to(shared_ptr<SoundSource> to) {

    if (!_done) {
        start_sound->stop();
        start_sound = end_sound;
        cout << "Sound cross-fading interrupted by another one." << endl;
    }

    end_sound = to;
    end_sound->volume(0);
    end_sound->play();
    elapsed_fade = chrono::milliseconds(0);
    _done = false;
}

void Crossfade::step(const std::chrono::milliseconds dt){

    if (_done) return;

    float alpha = (float) elapsed_fade.count() / fade_duration.count();

    if (alpha >= 1.f) {
        _done = true;
        end_sound->volume(1.f);
        start_sound->stop();
        start_sound = end_sound;
        cout << "Sound cross-fading complete." << endl;
        return;
    }

    elapsed_fade += dt;

    start_sound->volume(1.f - alpha);
    end_sound->volume(alpha);

    return;
}

