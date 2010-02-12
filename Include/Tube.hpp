/******************************************************************************
 * Warp: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>

namespace Warp {

class Tube  {
public:
	struct Impl;

	/** Initializes the Tube */
	Tube(Game* game, const std::string& name);

    /** Destructor */
    ~Tube();

private:
	std::auto_ptr<Impl> impl_;
};

}