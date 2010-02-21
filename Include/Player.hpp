/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Game.hpp"
#include <Ogre.h>
#include <memory>

namespace Warp {

class Player  {
public:
	struct Impl;

	/** Creates a new ball */
    Player(Game* game, const std::string& name);

    /** Destructor */
    ~Player();

    const Ogre::Vector3& getPosition() const;

	/** Gets the current spine node index */
	int getSpineNode() const;

private:
    std::auto_ptr<Impl> impl_;
};

}
