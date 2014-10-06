#include "table.h"


using namespace std;

Color Color::mix(const Color& color) {

}


void LED::update(const std::vector<Source>& sources){

    vector<tuple<Color, float>> lightsources;

    for (auto& source : sources) {
        if (source.type == LIGHT) {
            auto dist = distance(source);
            if (dist < MAX_COLOR_DISTANCE)
                lightsources.push_back(
                        make_tuple(dynamic_cast<const LightSource&>(source).color(),
                                   1. - dist / MAX_COLOR_DISTANCE)
                        );
        }
    }

    color = Color::from_mix(lightsources);
}
