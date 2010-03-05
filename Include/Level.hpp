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

	/** Adds a projectile*/
	void createProjectile(Object* target, const Ogre::Vector3& pos);

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
	static int luaGetSpineNodeDistance(lua_State* env);
	static int luaGetBeat(lua_State* env);
	static int luaLoadLoop(lua_State* env);
	static int luaStartLoop(lua_State* env);
	static int luaStopLoop(lua_State* env);
	static int luaStartBeatServer(lua_State* env);
	static int luaStopBeatServer(lua_State* env);
	static int luaCreateObject(lua_State* env);
	static int luaCreateTask(lua_State* env);

	Game* game_;
	std::list<boost::shared_ptr<Object>> objects_;
	std::list<boost::shared_ptr<Projectile>> projectiles_;
	std::list<boost::shared_ptr<ScriptTask>> tasks_;
	std::auto_ptr<DynamicTube> tube_;
	std::auto_ptr<Player> player_;
	size_t entitiesCreated_;
};


}
