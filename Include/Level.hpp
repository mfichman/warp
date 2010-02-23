/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

namespace Warp {

class Level : public GameListener {
public:
	/** Creates a new script and begins executing it inside a coroutine */
    Level(Game* game, const std::string& name);

    /** Destructor */
    ~Level();

	/** Returns the player object */
	inline Player* getPlayer() const {
		return player_.get();
	}

	/** Returns the tube for this level */
	inline DynamicTube* getTube() const {
		return tube_.get();
	}

private:
    Level(const Level&);
    Level& operator=(const Level&);
	virtual void onTimeStep();

	Game* game_;
	std::auto_ptr<DynamicTube> tube_;
	std::auto_ptr<Script> script_;
	std::auto_ptr<Player> player_;
};


}
