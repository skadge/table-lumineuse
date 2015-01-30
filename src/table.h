#ifndef TABLE_H
#define TABLE_H

#include <tuple>
#include <vector>
#include <array>
#include <cmath>
#include <memory>
#include <chrono>
#include <json/json.h>
#include <boost/optional.hpp>

#include "ledstrip.h"
#include "effects.h"

// Table 0,0: top left corner
static const int TABLE_WIDTH = 1000; //mm
static const int TABLE_HEIGHT = 500; //mm

static const int MAX_COLOR_DISTANCE = 1000; //mm (distance max to which a light source impact LEDs)

enum source_type { LIGHT = 0, SOUND  };
enum mode_type { PLAIN = 0, COLOR_MIX, PULSE, NOISE, CLOSING  };

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

class Table {


    std::vector<std::shared_ptr<LightSource>> sources;
    std::array<LED, NB_LEDS> leds;

    Ledstrip ledstrip;

    //** Mode-specific members **
    bool pulse_up; // direction: true when black->color, false when color->black

    // ****
    
public:

    // set this to false to enter 'soft sleep' mode, to true to wake up
    bool active;

    mode_type mode;

    Table(): 
        mode(PLAIN),
        pulse_up(true),
        active(true) {}

    void add_light(std::shared_ptr<LightSource> source) {sources.push_back(source);}

    void step(std::chrono::milliseconds dt = std::chrono::milliseconds(0));

    std::shared_ptr<LightSource> get_source(int id);

    Json::Value sources_to_JSON() const;

    void set_effect(std::shared_ptr<Effect> effect);
    boost::optional<Color> color() const {return ledstrip.color();}

    // for debugging: prints the list of LEDs values
    void show();

private:

   Color getTargetColor();
};

#endif
