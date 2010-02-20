/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once


#include <Game.hpp>
#include <memory>

namespace Warp {

class Ball  {
public:
	struct Impl;

	/** Creates a new ball */
    Ball(Game* game, const std::string& name);

    /** Destructor */
    ~Ball();

private:
    std::auto_ptr<Impl> impl_;
};

}