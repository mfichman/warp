/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include <list>
#include <boost/shared_ptr.hpp>

namespace Warp {

class Object {
public:

	/** Creates a new script and begins executing it inside a coroutine */
    Object(Game* game, const std::string& name, int id);

    /** Destructor */
    ~Object();

	/** Called every timestep by Level */
	void onTimeStep();
private:
    Object(const Object&);
    Object& operator=(const Object&);
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

	friend lua_State* Warp::operator>>(lua_State* env, Object& e);
};


}
