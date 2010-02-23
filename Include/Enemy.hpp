/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

namespace Warp {

class Enemy : public GameListener {
public:
	struct Impl;

	/** Creates a new script and begins executing it inside a coroutine */
    Enemy(Game* game, const std::string& name);

    /** Destructor */
    ~Enemy();
private:
    Enemy(const Enemy&);
    Enemy& operator=(const Enemy&);
	virtual void onTimeStep();

	Game* game_;
};


}
