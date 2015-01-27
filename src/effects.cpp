
#include "ledstrip.h"
#include "effects.h"

using namespace std;

///////////////////////////////// FADE ////////////////////////////////

Fade::Fade(Color target_color, chrono::milliseconds fade_duration) : 
    target_color(target_color),
    fade_duration(fade_duration),
    elapsed_fade(0) {};


void Fade::step(Ledstrip& leds, const chrono::milliseconds dt) {

    // First run: check if we already have the correct color
    if (!_running) {
        auto current_color = leds.color();
        if (current_color && current_color == target_color) {
            _done = true;
            return;
        }
        cout << "Starting to fade." << endl;
        _running = true;
    }
    ////////////////////////////////////////////////////////

    float alpha = (float) elapsed_fade.count() / fade_duration.count();

    if (alpha >= 1.f) {
        leds.fill(target_color);
        _running = false;
        _done = true;
        cout << "Fading complete." << endl;
        return;
    }

    elapsed_fade += dt;

    array<Color, NB_LEDS> next_colors;
    array<Color, NB_LEDS> current_colors = leds.colors();

    for (int i = 0; i < NB_LEDS; i++) {
        next_colors[i] = current_colors[i].interpolate(target_color, alpha);
    }

    leds.set(next_colors);

}
///////////////////////////////////////////////////////////////////////////


