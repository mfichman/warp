/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Interface.hpp>
#include <memory>
#include <Game.hpp>

namespace Criterium {

class Script : public Interface {
public:
    typedef intrusive_ptr<Script> Ptr;
	struct Impl;

	/** Creates a new script and begins executing it inside a coroutine */
    Script(Game* game, const std::string& name);

    /** Creates a new script using the given reference to a user function */
    Script(Game* game, int reference);


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

}