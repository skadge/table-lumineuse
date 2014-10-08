#include <tuple>
#include <vector>
#include <array>
#include <cmath>
#include <memory>

#include "ledstrip.h"

// Table 0,0: top left corner
static const int TABLE_WIDTH = 1000; //mm
static const int TABLE_HEIGHT = 500; //mm

static const int MAX_COLOR_DISTANCE = 1000; //mm (distance max to which a light source impact LEDs)

enum source_type { LIGHT = 0, SOUND  };

class Source {

protected:

    float _x, _y, _theta;
    float _dx, _dy, _dtheta;
    bool enabled;

public:

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

    void update(float x, float y, float theta) {
        _dx += _x - x; _x = x;
        _dy += _y - y; _y = y;
        _dtheta += _theta - theta; _theta = theta;
    }

    float x() const {return _x;}
    float y() const {return _y;}
    float theta() const {return _theta;}

    float dx() const {return _dx;}
    float dy() const {return _dy;}
    float dtheta() const {return _dtheta;}
};

class LightSource : public Source {



public:
    LightSource() {
        type = LIGHT;
    }

    Color color;

};

class SoundSource : public Source {

protected:
    std::string sound_name;
    float volume;

public:
    SoundSource() {
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
    Color _color;

public:

    // compute the location of the LED based on its index.
    LED();

    void update(const std::vector<std::shared_ptr<LightSource>> lights);

    Color color() const {return _color;}

    friend std::ostream& operator<< (std::ostream &out, const LED &led);
};

class Table {

    std::vector<std::shared_ptr<LightSource>> lights;
    std::array<LED, NB_LEDS> leds;

    Ledstrip ledstrip;

public:

    void add_light(std::shared_ptr<LightSource> light) {lights.push_back(light);}

    void step();

    void show();

};
