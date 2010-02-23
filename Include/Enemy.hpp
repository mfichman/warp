/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include <list>
#include <boost/shared_ptr.hpp>

namespace Warp {

class Enemy {
public:

	/** Creates a new script and begins executing it inside a coroutine */
    Enemy(Game* game, const std::string& name, int id);

    /** Destructor */
    ~Enemy();

	/** Called every timestep by Level */
	void onTimeStep();
private:
    Enemy(const Enemy&);
    Enemy& operator=(const Enemy&);
	void loadScriptCallbacks();

	// Lua callbacks
	static int luaAddEntity(lua_State* env);
	static int luaSetEntity(lua_State* env);
	static int luaAddParticleSystem(lua_State* env);
	static int luaSetParticleSystem(lua_State* env);
	static int luaSet(lua_State* env);

	Game* game_;
	Ogre::SceneNode* node_;
	std::string type_;
	std::string name_;
	int table_; // Reference to the Lua table
	std::list<Ogre::AnimationState*> activeAnimations_;
	std::list<boost::shared_ptr<btRigidBody>> rigidBodies_;

	friend lua_State* Warp::operator>>(lua_State* env, Enemy& e);
};


}
