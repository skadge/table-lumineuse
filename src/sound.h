#ifndef SOUND_H
#define SOUND_H

#include "sources.h"


class SoundSource : public Source {

protected:
    std::string path;
    float _playing;
    float loop;

public:
    SoundSource(const std::string& path, bool loop=false) : 
            Source(0), 
            path(path),
            _playing(false),
            loop(loop) {
        type = SOUND;
    }

    // virtual destructor -> polymorphic class
    virtual ~SoundSource() {}


    bool playing() const {return _playing;}

    /*  vol is a float between 0 and 1
     */
    virtual void volume(float vol) = 0;

    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
};

class Silence : public SoundSource {

public:
    Silence() : SoundSource("") {};

    void volume(float) {};

    void play() {};
    void pause() {};
    void stop() {};

};

static Silence SILENCE;

#endif
