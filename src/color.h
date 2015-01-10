#ifndef COLOR_H
#define COLOR_H

#include <tuple>
#include <vector>
#include <iostream>

class Color {

    uint8_t _r, _g, _b;

public:
    Color(): _r(0), _g(0), _b(0) {}
    Color(uint8_t r, uint8_t g, uint8_t b) : _r(r), _g(g), _b(b) {}

    std::tuple<uint8_t, uint8_t, uint8_t> rgb() const {return std::make_tuple(_r, _g, _b);}
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
#endif
