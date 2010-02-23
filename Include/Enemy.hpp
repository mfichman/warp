/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

namespace Warp {

class Enemy : public GameListener {
public:
	struct Impl;

	/** Creates a new script and begins executing it inside a coroutine */
    Enemy(Game* game, const std::string& name, int id);

    /** Destructor */
    ~Enemy();
private:
    Enemy(const Enemy&);
    Enemy& operator=(const Enemy&);
	virtual void onTimeStep();
	void loadScriptCallbacks();

	// Lua callbacks
	static int luaAddEntity(lua_State* env);

	Game* game_;
	Ogre::SceneNode* node_;
	std::string type_;
	std::string name_;
	std::list<Ogre::Entity*> entities_;
	int table_; // Reference to the Lua table
};


}
