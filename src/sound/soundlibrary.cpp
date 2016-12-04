#include <glob.h>
#include <boost/filesystem.hpp>

#include "soundlibrary.h"

#include "sfml_sound.h"

using namespace std;
using namespace boost::filesystem;

void SoundLibrary::find_sounds(const string& prefix) {
    for (auto sound : globbing(prefix + "/share/table-lumineuse/sounds/*.ogg")) {
        auto name = path(sound).replace_extension().leaf().string();

        cout << "Loading foreground sound " << name << endl;
        _foreground[name] = shared_ptr<SoundSource>(new SFMLSound(sound));
    }

    for (auto sound : globbing(prefix + "/share/table-lumineuse/background-sounds/*.ogg")) {
        auto name = path(sound).replace_extension().leaf().string();

        cout << "Loading background sound " << name << endl;
        _background[name] = shared_ptr<SoundSource>(new SFMLSound(sound, true));
    }

}

vector<string> SoundLibrary::globbing(const string& pattern){

    glob_t glob_result;
    glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);

    vector<string> ret;

    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
            ret.push_back(string(glob_result.gl_pathv[i]));
    }

    //TODO commented out the globfree as it causes an invalid free()...?
    //globfree(&glob_result);

    return ret;
}

void SoundLibrary::background(const string& sound) {
    current_background_fade->fade_to(_background[sound]);
}

void SoundLibrary::play(const string& sound) {
    current_foreground_fade->fade_to(_foreground[sound]);
    
}

void SoundLibrary::stopall(){
    current_foreground_fade->fade_to(SILENCE);
    current_background_fade->fade_to(SILENCE);
    
}


void SoundLibrary::step(std::chrono::milliseconds dt) {

    if (current_foreground_fade) current_foreground_fade->step(dt);
    if (current_background_fade) current_background_fade->step(dt);
}


