/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Interface.hpp>
#include <memory>
#include <Game.hpp>

namespace Criterium {

class Cylinder : public Interface {
public:
    typedef intrusive_ptr<Cylinder> Ptr;
	struct Impl;

	/** Creates a new bicycle */
    Cylinder(Game* game);

private:
	std::auto_ptr<Impl> impl_;
};

}