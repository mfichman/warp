/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Interface.hpp>
#include <memory>
#include <Game.hpp>

namespace Criterium {

class Bicycle : public Interface {
public:
    typedef intrusive_ptr<Bicycle> Ptr;
	struct Impl;

	/** Creates a new bicycle */
    Bicycle(Game* game);

private:
	Bicycle();
	std::auto_ptr<Impl> impl_;
};

}