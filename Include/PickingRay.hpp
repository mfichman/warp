/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>
#include <string>

namespace Warp {

class Game;

class PickingRay {
public:
	struct Impl;

	/** Creates a new object factory */
    PickingRay(Game* game);

    /** Destructor */
    ~PickingRay();

private:
	std::auto_ptr<Impl> impl_;
};

}