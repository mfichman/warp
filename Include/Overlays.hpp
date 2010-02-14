/******************************************************************************
 * Warp: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>

namespace Warp {

class Game;

class Overlays {
public:
	struct Impl;

	/** Creates a new set of overlays */
    Overlays(Game* game);

    /** Destructor */
    ~Overlays();

private:
	std::auto_ptr<Impl> impl_;
};

}