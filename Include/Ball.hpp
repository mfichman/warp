/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include <memory>
#include <Game.hpp>

namespace Warp {

class Ball  {
public:
	struct Impl;

	/** Creates a new ball */
    Ball(Game* game);

    /** Destructor */
    ~Ball();

private:
    std::auto_ptr<Impl> impl_;
};

}