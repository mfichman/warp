/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Interface.hpp>
#include <memory>
#include <Game.hpp>

namespace Criterium {

class Factory {
public:
	struct Impl;

	/** Creates a new object factory */
    Factory(Game* game);

    /** Creates a new ball using parameters in the given table */
    Ball* createBall(int table);

    /** Creates a new cube using parameters in the given table */
    Cube* createCube(int table);

    /** Creates a new ship using parameters in the given table */
    Ship* createShip(int table);

private:
	std::auto_ptr<Impl> impl_;
};

}