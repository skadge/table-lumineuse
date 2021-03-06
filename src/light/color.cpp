
#define MIN(a,b) (((a)<(b))?(a):(b))

#include <math.h>

#include "color.h"

#include <cassert>

using namespace std;

const Color Color::white(255,255,255);
const Color Color::black(0,0,0);
const Color Color::red(255,0,0);
const Color Color::blue(0,0,255);
const Color Color::green(0,255,0);


bool Color::operator==(const Color& target) const {

    uint8_t r,g,b;

    tie(r, g, b) = target.rgb();

    return _r == r &&
           _g == g &&
           _b == b;
}

ostream& operator<< (ostream &out, const Color &color){
    out << "[" << static_cast<int>(color._r) << "," 
               << static_cast<int>(color._g) << "," 
               << static_cast<int>(color._b) << "]";
    return out;
}

/**
 * Currently, simple weighted additive blend
 */
Color Color::from_mix(const vector<tuple<Color, float>>& colors) {

    int r = 0, g = 0, b = 0;

    for (const auto& weighted_color : colors) {
        Color color;
        float weight;
        uint8_t _r, _g, _b;

        tie(color, weight) = weighted_color;
        tie(_r, _g, _b) = color.rgb();

        r += _r * weight;
        g += _g * weight;
        b += _b * weight;

    }

    return Color(MIN(r, 255), MIN(g, 255), MIN(b, 255));
}

Color Color::interpolate(const Color target, float alpha) const {

    uint8_t r,g,b;

    tie(r, g, b) = target.rgb();

    return Color(_r + alpha * (r - _r),
                 _g + alpha * (g - _g),
                 _b + alpha * (b - _b));

}

/* Convert hsv values (0<=h<360, 0<=s<=1, 0<=v<=1) to rgb values (0<=r<=1, etc) */
Color Color::fromHSV(double h, double s, double v) {

    double r,g,b;

    int i;
    double f, p, q, t;

    if( s < 1.0e-6 ) {
        /* grey */
        r = g = b = v;
    }
    else {

        h /= 60.0;			        /* Divide into 6 regions (0-5) */
        i = (int)floor( h );
        f = h - (double)i;			/* fractional part of h */
        p = v * ( 1.0 - s );
        q = v * ( 1.0 - s * f );
        t = v * ( 1.0 - s * ( 1.0 - f ) );

        switch( i ) {
            case 0:
                r = v;
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = v;
                b = p;
                break;
            case 2:
                r = p;
                g = v;
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = v;
                break;
            case 4:
                r = t;
                g = p;
                b = v;
                break;
            default:		// case 5:
                r = v;
                g = p;
                b = q;
                break;
        }
    }

    return Color((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255));
}


Gradient::Gradient(const std::map<float, Color>& gradient) : _gradient(MAX_COLORS){
    // generate all possible interpolated colors
    for (auto i = 0; i < MAX_COLORS ; i++) {
            
        float alpha0 = 0.f;
        float alpha = ((float)i) / MAX_COLORS;

        for (auto& kv : gradient) {
            if (kv.first >= alpha) {

                auto col1 = gradient.at(alpha0);
                auto col2 = gradient.at(kv.first);
                auto ratio = (alpha - alpha0) / (kv.first - alpha0);
                
                auto new_col = col1.interpolate(col2, ratio);
                _gradient[i] = new_col;
            }
            else alpha0 = kv.first;
        }

    }

};

Color Gradient::operator [](float alpha) const {

    // clamp alpha in [0,1]
    alpha = min(1.f, max(alpha, 0.f));

    int i = round(alpha * MAX_COLORS);
    if (i == MAX_COLORS) i = MAX_COLORS - 1;

    return _gradient[i];
}


