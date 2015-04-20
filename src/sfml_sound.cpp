#include <iostream>

#include "sfml_sound.h"

using namespace std;

SFMLSound::SFMLSound(const string& name) :
    SoundSource(name),
    _sound_loaded(false) {

        if (!buffer.loadFromFile(name)) {
            cerr << "Unable to load sound " << name << endl;
            return;
        }

        sound.setBuffer(buffer);
        _sound_loaded = true;
    }


bool SFMLSound::playing() {
    return sound.getStatus() == sound.Playing;
}

void SFMLSound::volume(float vol) {
    sound.setVolume(vol * 100);
}

void SFMLSound::play() {
    if (!_sound_loaded) return;

    sound.play();

}

void SFMLSound::pause() {
    if (!_sound_loaded) return;

    sound.pause();

}

void SFMLSound::stop() {
    if (!_sound_loaded) return;

    sound.stop();

}

