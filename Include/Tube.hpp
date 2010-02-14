/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>

namespace Warp {

class Tube  {
public:
	struct Impl;
    struct SpineNode;

	/** Initializes the Tube */
	Tube(Game* game, const std::string& name);

    /** Destructor */
    ~Tube();

    /** 
     * Returns the spine that is closest to the given location.
     * position: this function will find the closest spine to this position
     * guess: where the function will start looking for the nearest spine
     */
    const SpineNode& getClosestSpineNode(const Ogre::Vector3& position, int guess);

private:
	std::auto_ptr<Impl> impl_;
};

struct Tube::SpineNode {
    Ogre::Vector3 position;
    Ogre::Vector3 forward;
    Ogre::Vector3 up;
    int index;
};

}