/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include <OIS/OIS.h>
#include <memory>
#include <boost/shared_ptr.hpp>

namespace Warp {

	class Screen : public GameListener, public OIS::KeyListener {
public:
	/** Creates a new script and begins executing it inside a coroutine */
    Screen(Game* game, const std::string& name);

    /** Destructor */
    ~Screen();

private:
	void loadScriptCallbacks();
	void callMethod(const std::string& method, const std::string& arg);

	// Lua functions
	static int luaSetScreen(lua_State* env);
	static int luaSetLevel(lua_State* env);
	static int luaSetSplash(lua_State* env);
	
	// OIS callbacks
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);

	Game* game_;
	std::string type_;
	int table_;
};


}

