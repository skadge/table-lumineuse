#ifndef SOURCES_H
#define SOURCES_H

#include <memory>
#include <vector>
#include <string>
#include <cmath>

#include "color.h"

// Table 0,0: top left corner
static const int TABLE_WIDTH = 1000; //mm
static const int TABLE_HEIGHT = 500; //mm

static const int MAX_COLOR_DISTANCE = 1000; //mm (distance max to which a light source impact LEDs)

enum source_type { LIGHT = 0, SOUND  };

class Source {

protected:


    int _id;

    float _x, _y, _theta;
    float _dx, _dy, _dtheta;
    bool enabled;

public:

    Source(int id) : _id(id) {}

    // virtual destructor -> polymorphic class
    virtual ~Source() {}

    source_type type;

    void enable() {
        _dx = _dy = _dtheta = 0.;
        enabled = true;
    }

    void disable() {
        enabled = false;
    }

    void position(float x, float y, float theta) {
        _dx += _x - x; _x = x;
        _dy += _y - y; _y = y;
        _dtheta += _theta - theta; _theta = theta;
    }

    int id() const {return _id;}

    float x() const {return _x;}
    float y() const {return _y;}
    float theta() const {return _theta;}

    float dx() const {return _dx;}
    float dy() const {return _dy;}
    float dtheta() const {return _dtheta;}
};

class LightSource : public Source {

public:
    LightSource(int id) : Source(id) {
        type = LIGHT;
    }

    Color color;

};

// SoundSource is declared in sound.h and defined in sound.cpp

class Actuator {

protected:
    float x, y;

public:
    inline float distance(float tx, float ty) {
        return sqrt(pow(tx - x, 2) + pow(ty - y, 2));
    }

    float distance(const Source& source) {
        return distance(source.x(), source.y());
    }

};

class LED : Actuator {

    static int last_id;

    int idx;

public:

    // compute the location of the LED based on its index.
    LED();

    void update(const std::vector<std::shared_ptr<LightSource>> lights);


    friend std::ostream& operator<< (std::ostream &out, const LED &led);

    Color color;
};

#endif // SOURCES_H
