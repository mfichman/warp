/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>

namespace Criterium {

class Tube  {
public:
	struct Impl;

	/** Initializes the Tube */
	Tube(Game* game, const std::string& name);

private:
	std::auto_ptr<Impl> impl_;
};

}