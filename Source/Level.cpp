/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Level.hpp"

#include "DynamicTube.hpp"
#include "Player.hpp"
#include "Script.hpp"
#include "Game.hpp"
#include "OscBeatListener.hpp"
#include "OscSender.hpp"

#include <string>
 extern "C" {
#include <lua/lua.h> 
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

using namespace Warp;
using namespace Ogre;
using namespace std;

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Level::Level(Game* game, const std::string& name) :
    game_(game),
    tube_(new DynamicTube(game, "Levels/" + name + ".tube")),
    script_(new Script(game, "Scripts/" + name + ".lua"))
{
	player_.reset(new Player(game, this, "Ball"));
    game_->addListener(this);

	loadScriptCallbacks();
}

/** Loads Lua script functions for this level */
void Level::loadScriptCallbacks() {
	lua_State* env = game_->getScriptState();

	loadScript(env, "Scripts/Object.lua");
	loadScript(env, "Scripts/Level.lua");

	lua_getglobal(env, "Level"); // Get the Level table

    lua_pushlightuserdata(env, this); // Modifies a light
    lua_pushcclosure(env, &Level::luaGetLight, 1);
    lua_setfield(env, -2, "getLight");

    lua_pushlightuserdata(env, this); // Modifies a light
    lua_pushcclosure(env, &Level::luaSetLight, 1);
    lua_setfield(env, -2, "setLight");

	lua_pushlightuserdata(env, this); // Returns the current spine node
    lua_pushcclosure(env, &Level::luaGetSpineNodeId, 1);
    lua_setfield(env, -2, "getSpineNodeId");

    lua_pushlightuserdata(env, this); // Gets the current beat
    lua_pushcclosure(env, &Level::luaGetBeat, 1);
    lua_setfield(env, -2, "getBeat");

    lua_pushlightuserdata(env, this); // tell chuck to enqueue loop
    lua_pushcclosure(env, &Level::luaQueueStartLoop, 1);
    lua_setfield(env, -2, "queueStartLoop");

    lua_pushlightuserdata(env, this); // tell chuck to enqueue loop
    lua_pushcclosure(env, &Level::luaStartBeatServer, 1);
    lua_setfield(env, -2, "startBeatServer");

	lua_pop(env, 1); // Pop the Level table
}
	
/** Destroys the level */
Level::~Level() {
	game_->removeListener(this);
}

/** Lua callback.  Get the light state */
int Level::luaGetLight(lua_State* env) {
    Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	Game* game = level->game_;
	lua_remove(env, 1);

    if (!lua_isstring(env, 1)) {
        lua_pushstring(env, "Expected string for entity name");
        lua_error(env);
    }       
    string name(lua_tostring(env, 1)); // First argument: name of the light
    if (!game->getSceneManager()->hasLight(name)) {
        lua_pushstring(env, "Invalid entity name");
        lua_error(env);
    }
    Light* light = game->getSceneManager()->getLight(name);
    env << *light;

    return 1;
}

/** Lua callback.  Sets the given values for the light */
int Level::luaSetLight(lua_State* env) {
    Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	Game* game = level->game_;
	lua_remove(env, 1);

    if (!lua_isstring(env, 1)) {
        lua_pushstring(env, "Expected string for entity name");
        lua_error(env);
    }       
    string name(lua_tostring(env, 1)); // First argument: name of the light
    if (!game->getSceneManager()->hasLight(name)) {
        lua_pushstring(env, "Invalid entity name");
        lua_error(env);
    }
    Light* light = game->getSceneManager()->getLight(name);
    env >> *light;
    
    return 0;
}

/** Lua callback.  Returns the current spine node ID. */
int Level::luaGetSpineNodeId(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	lua_remove(env, 1);

	lua_pushinteger(env, level->player_->getSpineNodeIndex());

    return 1;
}

/** Lua callback.  Gets the current beat as set by chuck */
int Level::luaGetBeat(lua_State* env) {
    Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	Game* game = level->game_;
	lua_remove(env, 1);
    lua_pushinteger(env, game->getOscBeatListener()->getCurBeat());
    return 1;
}

/** Lua callback.  Sends chuck the message to start a loop
 *  It gives a string id for the loop and the pathname to find it
 */
int Level::luaQueueStartLoop(lua_State* env) {
    Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	Game* game = level->game_;
	lua_remove(env, 1);

    // get msg
	BeatLoop beat_loop;
    env >> beat_loop;

    // send msg
    OscSender* sender = game->getOscSender();
    sender->beginMsg("/loop/start");
    sender->addString(beat_loop.name);
    sender->addString(beat_loop.path_name);
    sender->addInt(beat_loop.bpm);
    sender->addInt(beat_loop.n_beats);
    sender->sendMsg();
    return 1;
}

int Level::luaStartBeatServer(lua_State* env) {
    Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	Game* game = level->game_;
	lua_remove(env, 1);

    OscSender* sender = game->getOscSender();

    int bpm = 120; // default value
    lua_getfield(env, -1, "bpm");
    if (!lua_isnil(env, -1)) {
        bpm = lua_tointeger(env, -1);
    }
    lua_pop(env, 1); 
    sender->beginMsg("/server/start");
    sender->addInt(bpm);
    sender->sendMsg();
    return 1;
}

#include <OIS/OIS.h>
/** Called once for each game loop */
void Level::onTimeStep() {
	
	// Hack hack hack
	if (game_->getKeyboard()->isKeyDown(OIS::KC_R)) {
		game_->setLevel("Tube1");
	}
}
