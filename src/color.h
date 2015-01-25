#ifndef COLOR_H
#define COLOR_H

#include <tuple>
#include <map>
#include <vector>
#include <iostream>

#ifdef HAS_OPENCV
#include <opencv2/core/core.hpp>
#endif

typedef std::tuple<uint8_t, uint8_t, uint8_t> RGB;

class Color {

    uint8_t _r, _g, _b;

public:
    Color(): _r(0), _g(0), _b(0) {}
    Color(uint8_t r, uint8_t g, uint8_t b) : _r(r), _g(g), _b(b) {}

    RGB rgb() const {return std::make_tuple(_r, _g, _b);}

#ifdef HAS_OPENCV
    // returns a Vec3f, as expected for OpenCV images of type 32FC3, BGR format
    cv::Vec3f vec3f() const {return cv::Vec3f(_b/255.f, _g/255.f, _r/255.f);}
#endif

    //std::tuple<float, float, float> hsl() const {return std::make_tuple(_r, _g, _b);}

    bool operator==(const Color& target) const;
    bool operator!=(const Color& target) const { return !(operator==(target));}

    friend std::ostream& operator<< (std::ostream &out, const Color &color);

    static Color from_mix(const std::vector<std::tuple<Color, float>>& colors);

    /** returns a color which is the linear interpolation at 'alpha' of the current
     * color and the target color. alpha = 0 -> current color, alpha = 1 -> target 
     * color.
     */
    Color interpolate(const Color target, float alpha) const;

    static const Color white;
    static const Color black;
    static const Color red;
    static const Color blue;
    static const Color green;

};

class Gradient {

    const static int MAX_COLORS = 10000;

    std::vector<Color> _gradient;

public:
    Gradient(const std::map<float, Color>& gradient);

    Color operator [](float alpha) const;
};

#endif
