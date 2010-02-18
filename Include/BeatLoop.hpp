#pragma once
#include <string>

namespace Warp {

struct BeatLoop {

    BeatLoop()
        : name("Default")
        , path_name("no_path_name")
        , bpm(120)
        , n_beats(8)
    {}
    
    std::string name;
    std::string path_name;
    int bpm;
    int n_beats;
};

}
