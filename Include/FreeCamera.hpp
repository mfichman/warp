/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Game.hpp"
#include <memory>

namespace Warp {

class FreeCamera  {
public:
	struct Impl;

	/** Creates a new ball */
    FreeCamera(Game* game);

    /** Destructor */
    ~FreeCamera();

private:
    std::auto_ptr<Impl> impl_;
};

}