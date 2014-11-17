#ifndef TABLE_H
#define TABLE_H

#include <tuple>
#include <vector>
#include <array>
#include <cmath>
#include <memory>
#include <chrono>

#include "ledstrip.h"

// Table 0,0: top left corner
static const int TABLE_WIDTH = 1000; //mm
static const int TABLE_HEIGHT = 500; //mm

static const int MAX_COLOR_DISTANCE = 1000; //mm (distance max to which a light source impact LEDs)

enum source_type { LIGHT = 0, SOUND  };
enum mode_type { PLAIN = 0, COLOR_MIX, CLOSING  };

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

class SoundSource : public Source {

protected:
    std::string sound_name;
    float volume;

public:
    SoundSource(int id) : Source(id) {
        type = SOUND;
    }

};

class Actuator {

protected:
    float x, y;

public:
    float distance(const Source& source) {
        return sqrt(pow(source.x() - x, 2) + pow(source.y() - y, 2));
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

class Table {


    std::vector<std::shared_ptr<LightSource>> sources;
    std::array<LED, NB_LEDS> leds;

    Ledstrip ledstrip;

    std::chrono::milliseconds fade_duration;
    std::chrono::milliseconds elapsed_fade;
    bool fading;

    Color current_plain_color; // default constructor: black

public:

    mode_type mode;

    Table(): 
        mode(PLAIN),
        fade_duration(1000),
        elapsed_fade(0),
        fading(false) {}

    void add_light(std::shared_ptr<LightSource> source) {sources.push_back(source);}

    void step(std::chrono::milliseconds dt = std::chrono::milliseconds(0));

    std::shared_ptr<LightSource> get_source(int id);

    void show();

};

#endif
