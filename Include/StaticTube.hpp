/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>
#include <string>

namespace Warp {

class StaticTube  {
public:
	struct Impl;

	/** Initializes the StaticTube */
	StaticTube(Game* game, const std::string& name);

    /** Destructor */
    ~StaticTube();

private:
    StaticTube(const StaticTube&);
    StaticTube& operator=(const StaticTube&);

	std::auto_ptr<Impl> impl_;
};

}