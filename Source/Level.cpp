/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Level.hpp"

#include "DynamicTube.hpp"
#include "Object.hpp"
#include "Player.hpp"
#include "ScriptTask.hpp"
#include "Game.hpp"
#include "Projectile.hpp"
#include "OscBeatListener.hpp"
#include "OscSender.hpp"
#include "City.hpp"
#include "Overlays.hpp"

#include <string>
#include <algorithm>
 extern "C" {
#include <lua/lua.h> 
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}
#include <boost/lexical_cast.hpp>

using namespace Warp;
using namespace Ogre;
using namespace std;
using namespace boost;

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Level::Level(Game* game, const std::string& name) :
    game_(game),
    tube_(new DynamicTube(game, "Levels/" + name + ".tube")),
	objectsCreated_(0),
	scriptNumber_(0),
	name_(name),
	oldObjectCount_(0) {

	loadScriptCallbacks();
	player_.reset(new Player(game, this, "Player", 0));
	objects_.push_back(player_);
	game_->getSceneManager()->setWorldGeometry("terrain.cfg");
    game_->addListener(this);

	// Load beat script
	string audioPath = "Scripts/" + name + "." + lexical_cast<string>(scriptNumber_) + ".beat.lua";
	audioScript_.reset(new ScriptTask(game, audioPath));
	tasks_.push_back(audioScript_);

	// Load level script
	string levelPath = "Scripts/" + name + "." + lexical_cast<string>(scriptNumber_) + ".level.lua";
	levelScript_.reset(new ScriptTask(game, levelPath));
	tasks_.push_back(levelScript_);

	overlays_.reset(new Overlays(game_));

	player_->onTimeStep(); // Need this to initialize the spine node
}

/** Destroys the level */
Level::~Level() {
    // send kill signal to chuck beat server:
    OscSender* sender = game_->getOscSender();
    sender->beginMsg("/server/stop");
    sender->addInt(1);
    sender->sendMsg();

	game_->removeListener(this);

	// Manually clear all targets to break any remaining
	// circular references
	for (list<ObjectPtr>::iterator i = objects_.begin(); i != objects_.end(); i++) {
		(*i)->setTarget(0);
		(*i)->clearAllTrackers();
	}
}

#include <OIS/OIS.h>
/** Called once for each game loop */
void Level::onTimeStep() {

	if (oldObjectCount_ != objects_.size()) {
		oldObjectCount_ = objects_.size();
		//cout << "Object count: " << objects_.size() << endl;
	}
	
	//player_->onTimeStep();

	for (list<ObjectPtr>::iterator i = objects_.begin(); i != objects_.end();) {
		if (!(*i)->isAlive()) {
			i = objects_.erase(i);
		} else {
			(*i)->onTimeStep();
			i++;
		}
	}

	for (list<CityPtr>::iterator i = cities_.begin(); i != cities_.end();) {
		if (!(*i)->isAlive()) {
			i = cities_.erase(i);
		} else {
			(*i)->onTimeStep();
			i++;
		}
	}

	for (list<ScriptTaskPtr>::iterator i = tasks_.begin(); i != tasks_.end();) {
		if (!(*i)->isAlive()) {
			i = tasks_.erase(i);
		} else {
			(*i)->onTimeStep();
			i++;
		}
	}

	int newScriptNumber = getPlayer()->getPlayerProjection().script;
	if (scriptNumber_ != newScriptNumber) {
		// Switch script
		cout << "Switching level script: " << scriptNumber_ << " >>> " << newScriptNumber << endl;
		tasks_.erase(find(tasks_.begin(), tasks_.end(), levelScript_));

		scriptNumber_ = newScriptNumber;

		// Load beat script
		string audioPath = "Scripts/" + name_ + "." + lexical_cast<string>(scriptNumber_) + ".beat.lua";
		audioScript_.reset(new ScriptTask(game_, audioPath));
		tasks_.push_back(audioScript_);

		// Load level script
		string levelPath = "Scripts/" + name_ + "." + lexical_cast<string>(scriptNumber_) + ".level.lua";
		levelScript_.reset(new ScriptTask(game_, levelPath));
		tasks_.push_back(levelScript_);
	}
}

/** Loads Lua script functions for this level */
void Level::loadScriptCallbacks() {
	lua_State* env = game_->getScriptState();
	StackCheck check(env);

	loadScript(env, "Scripts/Object.lua");
	loadScript(env, "Scripts/Vector.lua");
	loadScript(env, "Scripts/Quaternion.lua");
	loadScript(env, "Scripts/Level.lua");
	loadScript(env, "Scripts/AI.lua");

	loadScriptFolder(env, "Scripts/Objects/");

	lua_getglobal(env, "Level"); // Get the Level table

    lua_pushlightuserdata(env, this); // Modifies a light
    lua_pushcclosure(env, &Level::luaGetLight, 1);
    lua_setfield(env, -2, "getLight");

    lua_pushlightuserdata(env, this); // Modifies a light
    lua_pushcclosure(env, &Level::luaSetLight, 1);
    lua_setfield(env, -2, "setLight");

	lua_pushlightuserdata(env, this); // Returns the current spine node
    lua_pushcclosure(env, &Level::luaGetSpineNodeDistance, 1);
    lua_setfield(env, -2, "getSpineNodeDistance");

    lua_pushlightuserdata(env, this); // Gets the current beat
    lua_pushcclosure(env, &Level::luaGetBeat, 1);
    lua_setfield(env, -2, "getBeat");

    lua_pushlightuserdata(env, this); // tell chuck to load loop
    lua_pushcclosure(env, &Level::luaLoadLoop, 1);
    lua_setfield(env, -2, "loadLoop");

    lua_pushlightuserdata(env, this); // tell chuck to enqueue loop
    lua_pushcclosure(env, &Level::luaStartLoop, 1);
    lua_setfield(env, -2, "startLoop");

    lua_pushlightuserdata(env, this); // tell chuck to stop loop
    lua_pushcclosure(env, &Level::luaStopLoop, 1);
    lua_setfield(env, -2, "stopLoop");

    lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaStartBeatServer, 1);
    lua_setfield(env, -2, "startBeatServer");

    lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaStopBeatServer, 1);
    lua_setfield(env, -2, "stopBeatServer");

    lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaLoadSFX, 1);
    lua_setfield(env, -2, "loadSFX");

    lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaPlaySFX, 1);
    lua_setfield(env, -2, "playSFX");

    lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaCreateObject, 1);
    lua_setfield(env, -2, "createObject");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaCreateCity, 1);
    lua_setfield(env, -2, "createCity");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaCreateEnemy, 1);
    lua_setfield(env, -2, "createEnemy");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaCreateTask, 1);
    lua_setfield(env, -2, "createTask");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaSetGravity, 1);
    lua_setfield(env, -2, "setGravity");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaSetCompositor, 1);
    lua_setfield(env, -2, "setCompositor");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaGetPlayerPosition, 1);
    lua_setfield(env, -2, "getPlayerPosition");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaGetPlayerOrientation, 1);
    lua_setfield(env, -2, "getPlayerOrientation");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaGetPlayerVelocity, 1);
    lua_setfield(env, -2, "getPlayerVelocity");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaGetSpineProjection, 1);
    lua_setfield(env, -2, "getSpineProjection");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaGetTimeStep, 1);
    lua_setfield(env, -2, "getTimeStep");

	lua_pushlightuserdata(env, this);
    lua_pushcclosure(env, &Level::luaGetPlayer, 1);
    lua_setfield(env, -2, "getPlayer");
}

int Level::luaGetSpineProjection(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	float distance = lua_tonumber(env, -1);

	const SpineProjection& player = level->player_->getPlayerProjection();
	const SpineProjection& proj = level->tube_->getSpineProjection(distance + player.distance, player.index);

	lua_newtable(env);
	env << proj.position;
	lua_setfield(env, -2, "position");
	env << proj.forward;
	lua_setfield(env, -2, "forward");

	return 1;
}

int Level::luaSetGravity(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	try {
		Vector3 gravity;
		env >> gravity;
		level->game_->getWorld()->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
	return 0;
}

int Level::luaSetCompositor(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	string compositor;

	try {
		if (lua_isstring(env, -1)) {
			env >> compositor;
		}

		if (compositor == level->compositor_) {
			return 0;
		}

		Game* game = level->game_;
		if (!level->compositor_.empty()) {
			CompositorManager::getSingleton().setCompositorEnabled(game->getWindow()->getViewport(0), level->compositor_, false);
			CompositorManager::getSingleton().removeCompositor(game->getWindow()->getViewport(0), level->compositor_);
		}
		level->compositor_ = compositor;
		if (!level->compositor_.empty()) {
			CompositorManager::getSingleton().addCompositor(game->getWindow()->getViewport(0), level->compositor_);
			CompositorManager::getSingleton().setCompositorEnabled(game->getWindow()->getViewport(0), level->compositor_, true);
		}
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
	
	return 0;
}

/** Creates an Object */
int Level::luaCreateObject(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		std::string type;
		env >> type;
#pragma warning (disable:4800)
		bool snap = lua_toboolean(env, -1);
#pragma warning (default:4800)

		// Create a new Object and add it to the list
		ObjectPtr object(new Object(level->game_, level, type, level->objectsCreated_++));
		level->objects_.push_back(object);

		env << object;
    } catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 1;
}

/** Creates an Object */
int Level::luaCreateCity(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		// Create a new City and add it to the list
		CityPtr city(new City(level->game_, level, level->objectsCreated_++));
		level->cities_.push_back(city);
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

/** Creates an Enemy */
int Level::luaCreateEnemy(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	try {
		std::string type;
		env >> type;
#pragma warning (disable:4800)
		bool snap = lua_toboolean(env, -1);
#pragma warning (default:4800)

		// Create a new Object and add it to the list
		ObjectPtr object(new Enemy(level->game_, level, type, level->objectsCreated_++));
		level->objects_.push_back(object);

		env << object;
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 1;
}

/** Creates a task */
int Level::luaCreateTask(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	if (!lua_isfunction(env, -1)) {
		lua_pushstring(env, "Not a function");
		lua_error(env);
	}
	try {
		int functionRef = lua_ref(env, -1);
		level->tasks_.push_back(ScriptTaskPtr(new ScriptTask(level->game_, functionRef))); 
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

/** Lua callback.  Get the light state */
int Level::luaGetLight(lua_State* env) {
    Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	Game* game = level->game_;

	string name;
	lua_getfield(env, -1, "name");
	env >> name;

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

	try {
		string name;
		lua_getfield(env, -1, "name");
		env >> name;

		if (!game->getSceneManager()->hasLight(name)) {
			lua_pushstring(env, "Invalid entity name");
			lua_error(env);
		}
		Light* light = game->getSceneManager()->getLight(name);
		env >> *light;
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
    
    return 0;
}

/** Lua callback.  Returns the current spine node ID. */
int Level::luaGetSpineNodeDistance(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	lua_pushinteger(env, level->player_->getPlayerProjection().distance);
    return 1;
}

/** Lua callback.  Returns the player's position vector. */
int Level::luaGetPlayerPosition(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	env << level->player_->getPosition();
    return 1;
}

int Level::luaGetPlayerVelocity(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	env << level->player_->getVelocity();
    return 1;
}

/** Lua callback.  Returns the player's quaternion. */
int Level::luaGetPlayerOrientation(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	env << level->player_->getOrientation();
    return 1;
}

/** Lua callback.  Gets the current beat as set by chuck */
int Level::luaGetBeat(lua_State* env) {
    Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	Game* game = level->game_;
    lua_pushinteger(env, game->getOscBeatListener()->getCurBeat());
    return 1;
}

int Level::luaPlaySFX(lua_State* env) {
	try {
		lua_getfield(env, -1, "id");
		int id = lua_tointeger(env, -1); // required
		lua_pop(env, 1); 

		float gain = 1; // default value
		lua_getfield(env, -1, "gain");
		if (!lua_isnil(env, -1)) {
			gain = lua_tonumber(env, -1);
		}
		lua_pop(env, 1); 

		Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
		OscSender* sender = level->game_->getOscSender();
		// send msg
		sender->beginMsg("/sfx/play");
		sender->addInt(id);
		sender->addFloat(gain);
		sender->sendMsg();
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
    return 0;
}

int Level::luaLoadSFX(lua_State* env) {
	try {
		lua_getfield(env, -1, "id");
		int id = lua_tointeger(env, -1); // required
		lua_pop(env, 1); 

		string path_name;
		lua_getfield(env, -1, "path");
		env >> path_name;

		Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
		OscSender* sender = level->game_->getOscSender();
		// send msg
		sender->beginMsg("/sfx/load");
		sender->addInt(id);
		sender->addString(path_name);
		sender->sendMsg();
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
    return 0;
}

/**
 *  Lua callback.  Sends chuck the message to stop a loop
 *  takes an id.
 */
int Level::luaStartLoop(lua_State* env) {
	try {
		lua_getfield(env, -1, "id");
		int id = lua_tointeger(env, -1); // required
		lua_pop(env, 1); 

		float gain = 1; // default value
		lua_getfield(env, -1, "gain");
		if (!lua_isnil(env, -1)) {
			gain = lua_tonumber(env, -1);
		}
		lua_pop(env, 1); 

		lua_pop(env, 1);

		// send msg
		Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
		OscSender* sender = level->game_->getOscSender();
		sender->beginMsg("/loop/start");
		sender->addInt(id);
		sender->addFloat(gain);
		sender->sendMsg();
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
    return 0;
}

/**
 *  Lua callback.  Sends chuck the message to stop a loop
 *  takes an id
 */
int Level::luaStopLoop(lua_State* env) {
	try {
		lua_getfield(env, -1, "id");
		int id = lua_tointeger(env, -1); // required
		lua_pop(env, 1); 

		lua_pop(env, 1);

		// send msg
		Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
		OscSender* sender = level->game_->getOscSender();
		sender->beginMsg("/loop/stop");
		sender->addInt(id);
		sender->sendMsg();
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
    return 0;
}

int Level::luaLoadLoop(lua_State* env) {
	try {
		// get msg
		BeatLoop beat_loop;
		env >> beat_loop;

		Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
		OscSender* sender = level->game_->getOscSender();
		// send msg
		sender->beginMsg("/loop/load");
		sender->addInt(beat_loop.id);
		sender->addString(beat_loop.path_name);
		sender->addInt(beat_loop.bpm);
		sender->addInt(beat_loop.n_beats);
		sender->sendMsg();
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
    return 0;
}

int Level::luaStartBeatServer(lua_State* env) {
	try {
		int bpm = 120; // default value
		lua_getfield(env, -1, "bpm");
		if (!lua_isnil(env, -1)) {
			bpm = lua_tointeger(env, -1);
		}
		lua_pop(env, 1); 

		Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
		OscSender* sender = level->game_->getOscSender();
		sender->beginMsg("/server/start");
		sender->addInt(bpm);
		sender->sendMsg();
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
    return 0;
}

int Level::luaStopBeatServer(lua_State* env) {
	try {
		Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
		OscSender* sender = level->game_->getOscSender();
		sender->beginMsg("/server/stop");
		sender->addInt(1);
		sender->sendMsg();
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
    return 0;
}

int Level::luaGetTimeStep(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	lua_pushnumber(env, level->game_->getTimeStep());
	return 1;
}

int Level::luaGetPlayer(lua_State* env) {
	Level* level = (Level*)lua_touserdata(env, lua_upvalueindex(1));
	env << level->player_;
	return 1;
}


ProjectilePtr Level::createProjectile(const std::string& type) {
	ProjectilePtr p(new Projectile(game_, this, type, objectsCreated_++));
	objects_.push_back(p);
	return p.get();
}
