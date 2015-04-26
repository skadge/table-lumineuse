#ifndef SFML_SOUND_H
#define SFML_SOUND_H

#include <SFML/Audio.hpp>

#include "sound.h"


class SFMLSound : public SoundSource {

private:
    sf::SoundBuffer buffer;
    sf::Sound sound;

    bool _sound_loaded;

    void load_sound();

public:
    SFMLSound(const std::string& path, bool loop=false);

    ~SFMLSound() {
        stop();
    }

    bool playing() const;

    void volume(float vol);

    void play();
    void pause();
    void stop();
};

#endif
