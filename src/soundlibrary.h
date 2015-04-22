#include <map>
#include <string>
#include <memory>

#include "sound.h"

class SoundLibrary {

public:
    std::map<std::string, std::unique_ptr<SoundSource>> foreground;
    std::map<std::string, std::unique_ptr<SoundSource>> background;

    SoundLibrary(const std::string& prefix_path) {
        find_sounds(prefix_path);
    }

private:

    void find_sounds(const std::string& path);

    std::vector<std::string> globbing(const std::string& pattern);
};
