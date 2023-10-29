#include <map>
#include <string>

#include "color.h"

const static Gradient HALLOWEEN_GRADIENT({
        {0.f, {31, 78, 16}}, // green
        {0.33f, {65, 16, 64}}, // purple
        {0.66f, {231, 33, 0}}, // orange
        {1.f, {31, 78, 16}}, // green
    });

const static Gradient FOREST_GRADIENT({
        {0.f, {142, 116, 28}},
        {0.53f, {68, 135, 13}},
        {0.63f, {131, 229, 27}},
        {0.73f, {0, 87, 0}},
        {1.f, {0, 36, 0}}
    });

const static Gradient FIRE_GRADIENT({
        {0.f, {20, 0, 0}},
        {0.2f, {255, 48, 25}},
        {0.43f, {237, 80, 23}},
        {0.53f, {249, 201, 24}},
        {1.f, {207, 4, 4}}
    });

const static Gradient OCEAN_GRADIENT({
        {0.f, {8, 24, 48}},
        {0.35f, {0, 0, 28}},
        {0.45f, {75, 54, 176}},
        {0.55f, {0, 0, 28}},
        {1.f, {8, 24, 48}}
    });

const static Gradient RAINBOW_GRADIENT({
        {0.f, {0, 255, 0}},
        {0.4f, {0, 0, 255}},
        {0.1f, {255, 0, 0}}
    });


const static std::map<std::string, Gradient> GRADIENTS({
        {"halloween", HALLOWEEN_GRADIENT},
        {"nature", FOREST_GRADIENT},
        {"fire", FIRE_GRADIENT},
        {"ocean", OCEAN_GRADIENT},
//        {"rainbow", RAINBOW_GRADIENT}
    });
