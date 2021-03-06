/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include <memory>

namespace Warp {

class Level : public GameListener {
public:
	/** Creates a new script and begins executing it inside a coroutine */
    Level(Game* game, const std::string& name);

    /** Destructor */
    ~Level();

	/** Returns the player object */
	inline PlayerPtr getPlayer() const {
		return player_;
	}

	/** Returns the tube for this level */
	inline DynamicTube* getTube() const {
		return tube_.get();
	}

	inline Overlays* getOverlays() const {
		return overlays_.get();
	}

	ProjectilePtr createProjectile(const std::string& type);

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
	static int luaGetSpineProjection(lua_State* env);
	static int luaGetBeat(lua_State* env);
	static int luaLoadLoop(lua_State* env);
	static int luaStartLoop(lua_State* env);
	static int luaStopLoop(lua_State* env);
	static int luaStartBeatServer(lua_State* env);
	static int luaStopBeatServer(lua_State* env);
	static int luaLoadSFX(lua_State* env);
	static int luaPlaySFX(lua_State* env);
	static int luaCreateObject(lua_State* env);
	static int luaCreateCity(lua_State* env);
	static int luaCreateEnemy(lua_State* env);
	static int luaCreateTask(lua_State* env);
	static int luaSetGravity(lua_State* env);
	static int luaSetCompositor(lua_State* env);
	static int luaGetPlayerPosition(lua_State* env);
	static int luaGetPlayerVelocity(lua_State* env);
	static int luaGetPlayerOrientation(lua_State* env);
	static int luaGetTimeStep(lua_State* env);
	static int luaGetPlayer(lua_State* env);

	Game* game_;
	std::list<ObjectPtr> objects_;
	std::list<ScriptTaskPtr> tasks_;
	std::list<CityPtr> cities_;
	ScriptTaskPtr levelScript_;
	ScriptTaskPtr audioScript_;
	PlayerPtr player_;
	std::auto_ptr<DynamicTube> tube_;
	std::auto_ptr<Warp::Overlays> overlays_;
	size_t objectsCreated_;
	std::string compositor_;
	std::string name_;
	int scriptNumber_;
	int oldObjectCount_;
};


}
