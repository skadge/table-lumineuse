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
        foreground[name] = unique_ptr<SoundSource>(new SFMLSound(sound));
    }

    for (auto sound : globbing(prefix + "/share/table-lumineuse/background-sounds/*.ogg")) {
        auto name = path(sound).replace_extension().leaf().string();

        cout << "Loading background sound " << name << endl;
        background[name] = unique_ptr<SoundSource>(new SFMLSound(sound, true));
    }

}

vector<string> SoundLibrary::globbing(const string& pattern){

    glob_t glob_result;
    glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);

    vector<string> ret;

    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
            ret.push_back(string(glob_result.gl_pathv[i]));
    }

    globfree(&glob_result);

    return ret;
}
