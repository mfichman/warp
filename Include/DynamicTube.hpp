/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Game.hpp"
#include <Ogre.h>
#include <memory>
#include <string>

namespace Warp {

class DynamicTube {
public:
	struct Impl;

	/** Initializes the tube from the tube file */
	DynamicTube(Game* game, const std::string& name);

    /** Destructor, destroys the tube */
    ~DynamicTube();

    SpineProjection getSpineProjection(const Ogre::Vector3& v, int& node_i) const;

private:
    DynamicTube(const DynamicTube&);

    DynamicTube& operator=(const DynamicTube&);

	std::auto_ptr<Impl> impl_;
};

}
