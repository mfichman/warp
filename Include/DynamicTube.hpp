/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>
#include <string>

namespace Warp {

class DynamicTube  {
public:
	struct Impl;

	/** Initializes the tube from the tube file */
	DynamicTube(Game* game, const std::string& name);

    /** Destructor, destroys the tube */
    ~DynamicTube();

private:
	std::auto_ptr<Impl> impl_;
};

}