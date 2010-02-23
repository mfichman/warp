/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

namespace Warp {

class Script : public GameListener {
public:
	/** Creates a new script and begins executing it inside a coroutine */
    Script(Game* game, const std::string& name);

    /** Descructor */
    ~Script();

private:
    Script(const Script&);
    Script& operator=(const Script&);
	void onTimeStep();
	bool hasTriggerFired();

	enum Event { WAIT_BEAT, WAIT_SPINE_NODE, SLEEP };

	Game* game_;
    std::string path_;
    int trigger_;
    int coroutine_;
    Event waitEvent_;
};

}
