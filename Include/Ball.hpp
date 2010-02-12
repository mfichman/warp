/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <memory>
#include <Game.hpp>

namespace Criterium {

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