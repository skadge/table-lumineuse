#ifndef SOUNDLIBRARY_H
#define SOUNDLIBRARY_H

#include <map>
#include <string>
#include <memory>
#include <chrono>

#include "soundeffect.h"

class SoundLibrary {

    std::map<std::string, std::shared_ptr<SoundSource>> _foreground;
    std::map<std::string, std::shared_ptr<SoundSource>> _background;

    // for now, only support a single foreground sound 
    // and a single background sound
    std::shared_ptr<Crossfade> current_foreground_fade;
    std::shared_ptr<Crossfade> current_background_fade;

public:

    SoundLibrary(const std::string& prefix_path = "/usr/local"):
        current_foreground_fade(std::make_shared<Crossfade>()),
        current_background_fade(std::make_shared<Crossfade>()) {
        find_sounds(prefix_path);
    }

    void background(const std::string& sound);
    void play(const std::string& sound);
    void stopall();

    void step(std::chrono::milliseconds dt);

private:

    void find_sounds(const std::string& path);

    std::vector<std::string> globbing(const std::string& pattern);
};

#endif
