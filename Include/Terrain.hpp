/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Interface.hpp>
#include <memory>
#include <Game.hpp>

namespace Criterium {

class Terrain : public Interface {
public:
    typedef intrusive_ptr<Terrain> Ptr;
	struct Impl;

	/** Creates a new bicycle */
	Terrain(Game* game, const std::string& name);

private:
	std::auto_ptr<Impl> impl_;
};

}