/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Screen.hpp"
#include "Game.hpp"
#include "ScriptTask.hpp"

extern "C" {
#include <lua/lua.h> 
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

using namespace Warp;
using namespace Ogre;
using namespace std;
using namespace boost;
using namespace OIS;

Screen::Screen(Game* game, const std::string& type) :
	game_(game),
	type_(type) {

	Overlay* splash = OverlayManager::getSingleton().getByName("Warp/Splash");
	splash->show();

	loadScriptCallbacks();
	game_->getKeyboard()->setEventCallback(this);
}

Screen::~Screen() {
	callMethod("onDestroy", "");
	lua_State* env = game_->getScriptState();
	lua_unref(env, table_);
	game_->getKeyboard()->setEventCallback(0);

	Overlay* splash = OverlayManager::getSingleton().getByName("Warp/Splash");
	splash->hide();
}

void Screen::loadScriptCallbacks() {
	lua_State* env = game_->getScriptState();
	StackCheck check(env);
	loadScript(env, "Scripts/Object.lua");
	loadScriptFolder(env, "Scripts/Screens/");

	lua_getglobal(env, type_.c_str());
	lua_getfield(env, -1, "new");
	lua_pushvalue(env, -2);
	lua_remove(env, -3);

	lua_pushlightuserdata(env, this); // Returns the current spine node
    lua_pushcclosure(env, &Screen::luaSetScreen, 1);
    lua_setfield(env, -2, "setScreen");

	lua_pushlightuserdata(env, this); // Returns the current spine node
    lua_pushcclosure(env, &Screen::luaSetLevel, 1);
    lua_setfield(env, -2, "setLevel");

	lua_pushlightuserdata(env, this); // Returns the current spine node
    lua_pushcclosure(env, &Screen::luaSetSplash, 1);
    lua_setfield(env, -2, "setSplash");

	lua_newtable(env);

	if (lua_pcall(env, 2, 1, 0)) {
		string message(lua_tostring(env, -1));
		lua_pop(env, 1);
		throw runtime_error("Error creating Screen: " + message);
	}

	table_ = lua_ref(env, -1);
}

/** Calls a method on the peer Lua object */
void Screen::callMethod(const string& method, const string& arg) {
	lua_State* env = game_->getScriptState();
	StackCheck check(env);

	lua_getref(env, table_);
	lua_getfield(env, -1, method.c_str());
	if (!lua_isfunction(env, -1)) {
		lua_pop(env, 2);
		return;
	}

	lua_pushvalue(env, -2);
	lua_remove(env, -3);
	lua_pushstring(env, arg.c_str());

	if (lua_pcall(env, 2, 0, 0)) {
		string message(lua_tostring(env, -1));
		lua_pop(env, 1);
		throw runtime_error(message);
	}
}

int Screen::luaSetScreen(lua_State* env) {
	Screen* screen = (Screen*)lua_touserdata(env, lua_upvalueindex(1));

	string name;
	env >> name;
	screen->game_->setScreen(name);
	return 0;
}

int Screen::luaSetLevel(lua_State* env) {
	Screen* screen = (Screen*)lua_touserdata(env, lua_upvalueindex(1));
	string name;
	env >> name;
	screen->game_->setLevel(name);
	return 0;
}

int Screen::luaSetSplash(lua_State* env) {
	Screen* screen = (Screen*)lua_touserdata(env, lua_upvalueindex(1));
	string name;
	env >> name;

	OverlayElement* guiSplash = OverlayManager::getSingleton().getOverlayElement("Warp/Splash");
	MaterialPtr material = guiSplash->getMaterial();
	Technique* tech = material->getTechnique(0);
	Pass* pass = tech->getPass(0);
	TextureUnitState* tex = pass->getTextureUnitState(0);
	tex->setTextureName(name);
	return 0;
}

bool Screen::keyPressed(const KeyEvent &arg) {
	string key = game_->getKeyboard()->getAsString(arg.key);
	callMethod("onKeyPressed", key);

	return true;
}

bool Screen::keyReleased(const KeyEvent &arg) {
	string key = game_->getKeyboard()->getAsString(arg.key);
	callMethod("onKeyReleased", key);

	return true;
}