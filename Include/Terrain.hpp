/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>

namespace Warp {

class Terrain  {
public:
	struct Impl;

	/** Initializes the terrain */
	Terrain(Game* game, const std::string& name);

private:
	std::auto_ptr<Impl> impl_;
};

}