/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Interface.hpp>
#include <memory>
#include <Game.hpp>

namespace Criterium {

class Ball : public Interface {
public:
    typedef intrusive_ptr<Ball> Ptr;
	struct Impl;

	/** Creates a new bicycle */
    Ball(Game* game);

private:
	std::auto_ptr<Impl> impl_;
};

}