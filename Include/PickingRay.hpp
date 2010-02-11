/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>
#include <string>

namespace Criterium {

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