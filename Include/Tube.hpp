/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>
#include <string>

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