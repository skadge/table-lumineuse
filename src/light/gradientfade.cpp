#include <cmath>
#include <iostream>

#include "gradientfade.h"

using namespace std;


GradientFade::GradientFade(const Gradient colormap, 
          chrono::milliseconds cycle_duration) :
            cycle_duration(cycle_duration),
            colormap(colormap) {

    cout << "Starting gradient-fade effect." << endl;
};


ColorSet GradientFade::step(const std::chrono::milliseconds dt) {

    float alpha = (float) elapsed_fade.count() / cycle_duration.count();

    elapsed_fade += dt;
    if (elapsed_fade >= cycle_duration) elapsed_fade = chrono::milliseconds{0};

    auto res = ColorSet();
    res.fill(colormap[alpha]);
    return res;
}

