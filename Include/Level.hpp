/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>

namespace Warp {

class Level {
public:
	struct Impl;

	/** Creates a new script and begins executing it inside a coroutine */
    Level(Game* game, const std::string& name);

    /** Descructor */
    ~Level();

private:
    Level(const Level&);
    Level& operator=(const Level&);

	std::auto_ptr<Impl> impl_;
};


}
