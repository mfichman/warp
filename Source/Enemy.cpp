/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Enemy.hpp"

#include "Game.hpp"

#include <sstream>
 extern "C" {
#include <lua/lua.h> 
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

using namespace Warp;
using namespace Ogre;
using namespace std;

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Enemy::Enemy(Game* game, const string& type, int id) :
    game_(game),
	type_(type)
{
	ostringstream os;
	os << type << id;
	name_ = os.str();

	node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name_);

	loadScriptCallbacks();
    game_->addListener(this);

}

Enemy::~Enemy() {
    game_->removeListener(this);
}

void Enemy::loadScriptCallbacks() {
	lua_State* env = game_->getScriptState();

	// Create the peer Lua object
	lua_getglobal(env, name_.c_str());
	lua_getfield(env, -1, "new");
	lua_remove(env, -2);

	// Call <name>.new()
	if (lua_pcall(env, 0, 1, 0)) {
		string message(lua_tostring(env, -1));
		lua_pop(env, 1);
		throw runtime_error("Error creating enemy: " + message);
	}

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Enemy::luaAddEntity, 1);
	lua_setfield(env, -2, "addEntity");

	table_ = lua_ref(env, -1);
}

/** Adds an entity to the enemy in its own scene node */
int Enemy::luaAddEntity(lua_State* env) {
	try {

	} catch (Exception& ex) {
		lua_pushstring(env, ex.getFullDescription().c_str());
		lua_error(env);
	} catch (exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

/** Called when the game updates */
void Enemy::onTimeStep() {

}