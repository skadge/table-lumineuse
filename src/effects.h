#ifndef EFFECTS_H
#define EFFECTS_H

#include <chrono>
#include <memory>

#include "color.h"
#include "ledstrip.h" // for ColorSet

static const std::chrono::milliseconds PULSE_DURATION{6000}; //ms

class Transition {

protected:
    bool _done;

public:
    Transition() : _done(false) {}

    // virtual destructor -> polymorphic class
    virtual ~Transition() {}

    virtual ColorSet step(const ColorSet& start_colors,
                      const ColorSet& end_colors,
                      const std::chrono::milliseconds dt) = 0;

    bool done() const {return _done;}
};

class Effect {

public:
    Effect() {}

    // virtual destructor -> polymorphic class
    virtual ~Effect() {}


    virtual ColorSet step(const std::chrono::milliseconds dt) = 0;
};

/* Most basic effect: always return the same plain color
 */
class Plain : public Effect {

protected:
    ColorSet plain_colors;

public:
    Plain(const Color& color) {for (auto& col : plain_colors) col = color;}

    ColorSet step(const std::chrono::milliseconds dt) {return plain_colors;}
};



#endif
