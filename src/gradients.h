#include <map>
#include <string>

#include "color.h"


const static Gradient FOREST_GRADIENT({
        {0.f, {93, 204, 38}},
        {0.53f, {68, 135, 13}},
        {0.63f, {131, 229, 27}},
        {0.73f, {0, 87, 0}},
        {1.f, {0, 36, 0}}
    });

const static Gradient FIRE_GRADIENT({
        {0.f, {255, 48, 25}},
        {0.43f, {237, 80, 23}},
        {0.53f, {249, 201, 24}},
        {1.f, {207, 4, 4}}
    });

const static Gradient OCEAN_GRADIENT({
        {0.f, {44, 83, 158}},
        {0.15f, {7, 13, 28}},
        {0.35f, {7, 13, 28}},
        {0.45f, {53, 198, 214}},
        {1.f, {44, 83, 158}}
    });

const static std::map<std::string, Gradient> GRADIENTS({
        {"nature", FOREST_GRADIENT},
        {"fire", FIRE_GRADIENT},
        {"ocean", OCEAN_GRADIENT}
    });
