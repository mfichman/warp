/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>

namespace Warp {

class Script {
public:
	struct Impl;

	/** Creates a new script and begins executing it inside a coroutine */
    Script(Game* game, const std::string& name);

    /** Descructor */
    ~Script();

private:
	std::auto_ptr<Impl> impl_;
};


/** Methods for sending Ogre and ODE values to a script */
lua_State* operator<<(lua_State* env, const Ogre::Vector3& v);
lua_State* operator<<(lua_State* env, const Ogre::Quaternion& q);
lua_State* operator<<(lua_State* env, const Ogre::SceneNode& n);
lua_State* operator<<(lua_State* env, const Ogre::Light& l);
lua_State* operator<<(lua_State* env, const Ogre::ColourValue& c);

/** Methods for reading Ogre and ODE values from a script */
lua_State* operator>>(lua_State* env, Ogre::Vector3& v);
lua_State* operator>>(lua_State* env, Ogre::Quaternion& q);
lua_State* operator>>(lua_State* env, Ogre::SceneNode& n);
lua_State* operator>>(lua_State* env, Ogre::Light& l);
lua_State* operator>>(lua_State* env, Ogre::ColourValue& c);
lua_State* operator>>(lua_State* env, const std::string& s);

}