#include <tuple>
#include <vector>
#include <cmath>

static const float MAX_COLOR_DISTANCE = 1.; //meters (distance max to which a light source impact LEDs)

enum source_type { LIGHT = 0, SOUND  };

class Color {

    float _r, _g, _b;

public:
    Color(): _r(0.), _g(0.), _b(0.) {}
    Color(char r, char g, char b) : _r(r * 1./255), _g(g * 1./255), _b(b * 1./255) {}
    Color(float r, float g, float b) : _r(r), _g(g), _b(b) {}

    std::tuple<float, float, float> rgb() {return std::make_tuple(_r, _g, _b);}
    std::tuple<float, float, float> hsl() {return std::make_tuple(_r, _g, _b);}

    Color mix(const Color& color);
    static Color from_mix(const std::vector<std::tuple<Color, float>>& colors);
};

class Source {

protected:

    float _x, _y, _theta;
    float _dx, _dy, _dtheta;
    bool enabled;

public:

    // virtual destructor -> polymorphic class
    virtual ~Source();

    source_type type;

    void enable() {
        _dx = _dy = _dtheta = 0.;
        enabled = true;
    }

    void disable() {
        enabled = false;
    }

    void update(float x, float y, float theta) {
        _dx += _x - x; x = _x;
        _dy += _y - y; y = _y;
        _dtheta += _theta - theta; theta = _theta;
    }

    float x() const {return _x;}
    float y() const {return _y;}
    float theta() const {return _theta;}

    float dx() const {return _dx;}
    float dy() const {return _dy;}
    float dtheta() const {return _dtheta;}
};

class LightSource : Source {


protected:
    Color _color;

public:
    LightSource() {
        type = LIGHT;
    }

    Color color() const { return _color;}

};

class SoundSource : Source {

protected:
    std::string sound_name;
    float volume;

public:
    SoundSource() {
        type = SOUND;
    }

};

class Actuator {


    float x, y;

public:
    float distance(const Source& source) {
        return sqrt(pow(source.x() - x, 2) + pow(source.y() - y, 2));
    }

};

class LED : Actuator {


    Color color;

public:

    // compute the location of the LED based on its index.
    LED(char idx);

    void update(const std::vector<Source>& sources);

    std::tuple<char, char, char> get() {return value;}

};
