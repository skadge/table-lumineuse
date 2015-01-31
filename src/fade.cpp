#ifndef FADE_H
#define FADE_H

#include "fade.h"

using namespace std;

Fade::Fade(chrono::milliseconds fade_duration) : 
    fade_duration(fade_duration),
    elapsed_fade(0) {};


ColorSet Fade::step(const ColorSet& start_colors,
                    const ColorSet& end_colors,
                    const chrono::milliseconds dt) {


    float alpha = (float) elapsed_fade.count() / fade_duration.count();

    if (alpha >= 1.f) {
        _done = true;
        cout << "Fading complete." << endl;
        return end_colors;
    }

    elapsed_fade += dt;

    ColorSet next_colors;

    for (size_t i = 0; i < next_colors.size(); i++) {
        next_colors[i] = start_colors[i].interpolate(end_colors[i], alpha);
    }

    return next_colors;

}

#endif
