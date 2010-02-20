/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once


#include <Ogre.h>
#include "Game.hpp"
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

    /** Sets the current spine node */
    void setSpineNode(const SpineNode& node);

	/** Gets the current spine node */
	const SpineNode& getSpineNode() const;

private:
    std::auto_ptr<Impl> impl_;
};

}
