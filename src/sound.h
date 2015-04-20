#ifndef SOUND_H
#define SOUND_H

#include "sources.h"


class SoundSource : public Source {

protected:
    std::string sound_name;
    float _playing;

public:
    SoundSource(const std::string& name) : 
            Source(0), 
            sound_name(name),
            _playing(false) {
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
