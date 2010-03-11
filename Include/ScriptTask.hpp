/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include "SharedObject.hpp"

namespace Warp {

class ScriptTask : public SharedObject {
public:
	/** Creates a new script and begins executing it inside a coroutine */
    ScriptTask(Game* game, const std::string& name);

	/** Creates a new task and begins executing it inside a coroutine */
	ScriptTask(Game* game, int functionRef);

    /** Descructor */
    ~ScriptTask();

	
	/** Returns true if the script is alive */
	bool isAlive() const { return alive_; }

	/** Called every timestep by Level */
	void onTimeStep();

private:
    ScriptTask(const ScriptTask&);
    ScriptTask& operator=(const ScriptTask&);
	void init(int functionRef);
	bool hasTriggerFired();

	enum Event { WAIT_BEAT, WAIT_SPINE_NODE, SLEEP };

	Game* game_;
    int trigger_;
    int coroutine_;
    Event waitEvent_;
	bool alive_;
};

class StackCheck {
public:
	StackCheck(lua_State* env);
	~StackCheck();

private:
	int top_;
	lua_State* env_;
};

}
