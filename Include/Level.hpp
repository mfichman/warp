/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include <memory>
#include <boost/shared_ptr.hpp>

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
	void loadScriptCallbacks();
	virtual void onTimeStep();
	

	// Lua callbacks
	static int luaGetNode(lua_State* env);
	static int luaSetNode(lua_State* env);
	static int luaGetLight(lua_State* env);
	static int luaSetLight(lua_State* env);
	static int luaGetSpineNodeId(lua_State* env);
	static int luaGetBeat(lua_State* env);
	static int luaQueueStartLoop(lua_State* env);
	static int luaStartBeatServer(lua_State* env);
	static int luaCreateObject(lua_State* env);

	Game* game_;
	std::list<boost::shared_ptr<Object>> objects_;
	std::auto_ptr<DynamicTube> tube_;
	std::auto_ptr<Script> script_;
	std::auto_ptr<Player> player_;
	size_t entitiesCreated_;
};


}
