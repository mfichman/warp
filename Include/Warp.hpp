/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************
 *
 * This contains forward declarations for all of the
 * classes as well as some grobal structs and functions
 */

#pragma once

#include <Ogre.h>
#include <string>

namespace Warp {

class Overlays;
class Objects;
class Script;
class Player;
class DynamicTube;
class Level;


inline int mod(int num, int divisor) {
    int remainder = num % divisor;
    if (remainder < 0) remainder += divisor;
    return remainder;
}

struct SpineNode {
    SpineNode() : index(0) {}
    Ogre::Vector3 position;
    Ogre::Vector3 forward;
    int index;
};

struct SpineProjection {
    Ogre::Vector3 position;
    Ogre::Vector3 forward;
};

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
