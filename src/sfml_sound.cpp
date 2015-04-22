#include <iostream>

#include "sfml_sound.h"

using namespace std;

SFMLSound::SFMLSound(const string& path, bool loop) :
    SoundSource(path,loop),
    _sound_loaded(false) {}

void SFMLSound::load_sound() {

    if (!_sound_loaded) {

        if (!buffer.loadFromFile(path)) {
            cerr << "Unable to load sound " << path << endl;
            return;
        }

        sound.setBuffer(buffer);

        sound.setLoop(loop);

        _sound_loaded = true;
    }
}


bool SFMLSound::playing() const {
    return sound.getStatus() == sound.Playing;
}

void SFMLSound::volume(float vol) {
    sound.setVolume(vol * 100);
}

void SFMLSound::play() {
    if (!_sound_loaded) {
        // try to load the sound...
        load_sound();
        if (!_sound_loaded) {
            return;
        }
    }

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

