/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "ScriptTask.hpp"
#include "Game.hpp"

#include <stdexcept>
#include <boost/filesystem/operations.hpp>
#include <Bullet/btBulletDynamicsCommon.h>
extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

using namespace Warp;
using namespace Ogre;
using namespace std;
using namespace boost::filesystem;

/** Initializes the script task from a file */
ScriptTask::ScriptTask(Game* game, const std::string& path) :
    game_(game), 
    trigger_(0),
	alive_(true) {    

    lua_State* env = game_->getScriptState();
	StackCheck check(env);

    // Load the script chunk as a function
    if (luaL_loadfile(env, path.c_str())) {
        string message(lua_tostring(env, -1));
        throw runtime_error("Could not load script: " + message);
	}

	int functionRef = lua_ref(env, LUA_REGISTRYINDEX);
	init(functionRef);
}

/** Initializes the script task from a reference to the "thread" function */
ScriptTask::ScriptTask(Game* game, int functionRef) :
    game_(game), 
    trigger_(0),
	alive_(true) {

	init(functionRef);
}

void ScriptTask::init(int functionRef) {
	lua_State* env = game_->getScriptState();

    lua_getglobal(env, "coroutine");
    lua_getfield(env, -1, "create"); // Function to be called
    lua_remove(env, -2); // Remove "coroutine" table

	// Get the task function
	lua_getref(env, functionRef);

    // Call "coroutine.create(script chunk)" as the argument
    // Return value is the coroutine object
    lua_call(env, 1, 1);

     // Save a reference to the coroutine object (and pop it off the stack)
    coroutine_ = lua_ref(env, LUA_REGISTRYINDEX);
}

ScriptTask::~ScriptTask() {
    lua_unref(game_->getScriptState(), coroutine_);
    if (trigger_) lua_unref(game_->getScriptState(), trigger_);
}

bool ScriptTask::hasTriggerFired() {
    if (!trigger_) return true;
    lua_State* env = game_->getScriptState();
	StackCheck check(env);

    // Call the trigger function.  It should return a 
    // boolean on the stack.
    lua_getref(env, trigger_);
    if (lua_pcall(env, 0, 1, 0)) {
        string message(lua_tostring(env, -1));
        throw runtime_error("Error calling trigger: " + message);
    }

    // Check the return value of the trigger function.
    // True means the trigger fired, so the script will 
    // start up again.  False means we have to wait until
    // later
    if (lua_isboolean(env, -1)) {
#pragma warning(disable:4800)
        bool result = lua_toboolean(env, -1);
#pragma warning(default:4800)
        return result;
    } else {
        throw runtime_error("Trigger returned an invalid value");
    }
}

/** Determines if the script should be restarted in this frame */
void ScriptTask::onTimeStep() {
	if (!alive_) return;

    lua_State* env = game_->getScriptState();
	StackCheck check(env);

    // Check trigger
    if (!hasTriggerFired()) return;

    // Get the coroutine.resume() Lua function
    lua_getglobal(env, "coroutine");
    lua_getfield(env, -1, "resume"); // Function to be called
    lua_remove(env, -2); // Remove "coroutine" table
    lua_getref(env, coroutine_); // Get reference to the coroutine object

    // Resume the corutine
    if (lua_pcall(env, 1, 2, 0)) {
        string message(lua_tostring(env, -1));
        throw runtime_error("Error calling script function: " + message);
    }

    // Get the trigger function
    if (lua_isfunction(env, -1)) {
        // Save a handle to the trigger function
        trigger_ = luaL_ref(env, LUA_REGISTRYINDEX);

    } else if (lua_isboolean(env, -2) && !lua_toboolean(env, -2)) {
        // Coroutine returned with an error
        string message(lua_tostring(env, -1));
        throw runtime_error("Error calling script function: " + message);

    } else {
        // ScriptTask is complete, never run it again
        alive_ = false;
    }
}

/** Methods for sending Ogre values to a script */
lua_State* Warp::operator<<(lua_State* env, const Ogre::Vector3& v) {
	lua_getglobal(env, "Vector");
	lua_getfield(env, -1, "new");
	lua_getglobal(env, "Vector");
    lua_newtable(env);
    lua_pushnumber(env, v.x);
    lua_rawseti(env, -2, 1);
    lua_pushnumber(env, v.y);
    lua_rawseti(env, -2, 2);
    lua_pushnumber(env, v.z);
    lua_rawseti(env, -2, 3);

	lua_call(env, 2, 1);
	
    return env;
}

lua_State* Warp::operator<<(lua_State* env, const Ogre::Quaternion& q) {
	lua_getglobal(env, "Quaternion");
	lua_getfield(env, -1, "new");
	lua_getglobal(env, "Quaternion");
    lua_newtable(env);
    lua_pushnumber(env, q.x);
    lua_rawseti(env, -2, 1);
    lua_pushnumber(env, q.y);
    lua_rawseti(env, -2, 2);
    lua_pushnumber(env, q.z);
    lua_rawseti(env, -2, 3);
    lua_pushnumber(env, q.w);
    lua_rawseti(env, -2, 4);

	lua_call(env, 2, 1);

    return env;
}

lua_State* Warp::operator<<(lua_State* env, const Ogre::SceneNode& n) {
    lua_newtable(env);

    env << n.getPosition();
    lua_setfield(env, -2, "position");
    env << n.getOrientation();
    lua_setfield(env, -2, "orientation");
    env << Ogre::Vector3::ZERO;
    lua_setfield(env, -2, "linearvel");
    env << Ogre::Vector3::ZERO;
    lua_setfield(env, -2, "angularvel");       

    return env;
}

lua_State* Warp::operator<<(lua_State* env, const Ogre::Light& l) {
    lua_newtable(env);
    env << l.getPosition();
    lua_setfield(env, -2, "position");
    env << l.getDirection();
    lua_setfield(env, -2, "direction");
    env << l.getDiffuseColour();
    lua_setfield(env, -2, "diffuse");
    env << l.getDiffuseColour();
    lua_setfield(env, -2, "specular");
    switch (l.getType()) {
        case Light::LT_POINT: lua_pushstring(env, "point"); break;
        case Light::LT_DIRECTIONAL: lua_pushstring(env, "directional"); break;
        case Light::LT_SPOTLIGHT: lua_pushstring(env, "spotlight"); break;
    }
    lua_setfield(env, -2, "type");

    return env;

}

lua_State* Warp::operator<<(lua_State* env, const Ogre::ColourValue& c) {
    lua_pushnumber(env, c.r);
    lua_rawseti(env, -2, 1);
    lua_pushnumber(env, c.b);
    lua_rawseti(env, -2, 2);
    lua_pushnumber(env, c.g);
    lua_rawseti(env, -2, 3);

    return env;
}


/** Methods for reading Ogre from a script */
lua_State* Warp::operator>>(lua_State* env, Ogre::Vector3& v) {
	if (!lua_istable(env, -1)) {
		throw runtime_error("Invalid argument: expected vector");
	}
    lua_rawgeti(env, -1, 1);
    v.x = lua_tonumber(env, -1);
    lua_pop(env, 1);
    lua_rawgeti(env, -1, 2);
    v.y = lua_tonumber(env, -1);
    lua_pop(env, 1);
    lua_rawgeti(env, -1, 3);
    v.z = lua_tonumber(env, -1);
    lua_pop(env, 1);
    lua_pop(env, 1);

    return env;

}

lua_State* Warp::operator>>(lua_State* env, Ogre::Quaternion& q) {
    if (!lua_istable(env, -1)) {
		throw runtime_error("Invalid argument: expected quaternion");
	}

	lua_rawgeti(env, -1, 1);
	q.x = lua_tonumber(env, -1);
	lua_pop(env, 1);
	lua_rawgeti(env, -1, 2);
	q.y = lua_tonumber(env, -1);
	lua_pop(env, 1);
	lua_rawgeti(env, -1, 3);
	q.z = lua_tonumber(env, -1);
	lua_pop(env, 1);
	lua_rawgeti(env, -1, 4);
	q.w = lua_tonumber(env, -1);
	lua_pop(env, 1);

    return env;
}

lua_State* Warp::operator>>(lua_State* env, Ogre::SceneNode& n) {
    if (!lua_istable(env, -1)) {
		throw runtime_error("Invalid argument: expected table");
	}
    
    lua_getfield(env, -1, "position");
    if (!lua_isnil(env, -1)) {
		Vector3 position;
        env >> position;
        n.setPosition(position);
    } else {
        lua_pop(env, 1);
    }
    lua_getfield(env, -1, "orientation");
    if (!lua_isnil(env, -1)) {
		Quaternion orientation;
        env >> orientation;
        n.setOrientation(orientation);
    } else {
        lua_pop(env, 1);
    }
    lua_pop(env, 1);

    return env;

}

lua_State* Warp::operator>>(lua_State* env, btRigidBody& n) {
    if (!lua_istable(env, -1)) {
		throw runtime_error("Invalid argument: expected table");
	}
    
    lua_getfield(env, -1, "position");
    if (!lua_isnil(env, -1)) {
		Vector3 position;
        env >> position;

		btQuaternion q = n.getOrientation();
		btVector3 v(position.x, position.y, position.z);
        n.setCenterOfMassTransform(btTransform(q, v));
    } else {
        lua_pop(env, 1);
    }
    lua_getfield(env, -1, "orientation");
    if (!lua_isnil(env, -1)) {
		Quaternion o;
        env >> o;

        btQuaternion q(o.x, o.y, o.z, o.w);
		btVector3 v = n.getCenterOfMassPosition();
        n.setCenterOfMassTransform(btTransform(q, v));
    } else {
        lua_pop(env, 1);
    }
    lua_pop(env, 1);

    return env;
}

lua_State* Warp::operator>>(lua_State* env, Ogre::Entity& e) {
    if (!lua_istable(env, -1)) {
		throw runtime_error("Invalid argument: expected table");
	}

    lua_pop(env, 1);

    return env;

}

lua_State* Warp::operator>>(lua_State* env, Ogre::Light& l) {
    if (!lua_istable(env, -1)) {
		throw runtime_error("Invalid argument: expected table");
	}
    Vector3 position, direction;
    ColourValue diffuse, specular;

    lua_getfield(env, -1, "position");
    if (!lua_isnil(env, -1)) {
        env >> position;
        l.setPosition(position);
    } else {
        lua_pop(env, 1);
    }

    lua_getfield(env, -1, "direction");
    if (!lua_isnil(env, -1)) {
        env >> direction;
        l.setDirection(direction);
    } else {
        lua_pop(env, 1);
    }

    lua_getfield(env, -1, "diffuse");
    if (!lua_isnil(env, -1)) {
        env >> diffuse;
        l.setDiffuseColour(diffuse);
    } else {
        lua_pop(env, 1);
    }

    lua_getfield(env, -1, "specular");
    if (!lua_isnil(env, -1)) {
        env >> specular;
        l.setSpecularColour(specular);
    } else {
        lua_pop(env, 1);
    }

    lua_getfield(env, -1, "type");
    if (lua_isstring(env, -1)) {
        string type(lua_tostring(env, -1));

        if (type == "point") {
            l.setType(Light::LT_POINT);
        } else if (type == "directional") {
            l.setType(Light::LT_DIRECTIONAL);
        } else if (type == "spotlight") {
            l.setType(Light::LT_SPOTLIGHT);
        }
    }
    lua_pop(env, 1);
    lua_pop(env, 1);
    
    return env;
}


lua_State* Warp::operator>>(lua_State* env, Ogre::ColourValue& c) {
    if (!lua_istable(env, -1)) {
		throw runtime_error("Invalid argument: expected table");
	}
    lua_rawgeti(env, -1, 1);
    c.r = lua_tonumber(env, -1);
    lua_pop(env, 1);
    lua_rawgeti(env, -1, 2);
    c.g = lua_tonumber(env, -1);
    lua_pop(env, 1);
    lua_rawgeti(env, -1, 3);
    c.b = lua_tonumber(env, -1);
    lua_pop(env, 1);
    lua_pop(env, 1);

    return env;

}

/** This is a convenience method for reading into a string from lua */
lua_State* Warp::operator>>(lua_State* env, std::string& s) {
    if (!lua_isstring(env, -1)) {
		throw runtime_error("Invalid argument: expected string");
	}
    s.assign(lua_tostring(env, -1)); // Get the string
    lua_pop(env, 1); // Pop the string value off the lua stack

    return env;
}

/** Reads beat loop info from an id and table of values */
lua_State* Warp::operator>>(lua_State* env, Warp::BeatLoop & bl) {
    if (!lua_istable(env, -1)) {
		throw runtime_error("Invalid argument: expected table");
	}

	lua_getfield(env, -1, "id");
    bl.id = lua_tointeger(env, -1);
    lua_pop(env, 1); // id must not be nil, so this will throw an exception if it is

    lua_getfield(env, -1, "path");
    if (!lua_isnil(env, -1)) {
        env >> bl.path_name;
    } else {
        lua_pop(env, 1);
    }
  
    lua_getfield(env, -1, "bpm");
    if (!lua_isnil(env, -1)) {
        bl.bpm = lua_tointeger(env, -1);
    }
    lua_pop(env, 1); 
   
    lua_getfield(env, -1, "n_beats");
    if (!lua_isnil(env, -1)) {
        bl.n_beats = lua_tointeger(env, -1); 
    }
    lua_pop(env, 1); 
    // pop off the table
    lua_pop(env, 1); 

    return env;
}

void Warp::loadScript(lua_State* env, const std::string& name) {
	if (luaL_dofile(env, name.c_str())) {
        string message(lua_tostring(env, -1));
        lua_pop(env, 2);
        throw runtime_error("ScriptTask error: " + message);
    }
}

void Warp::loadScriptFolder(lua_State* env, const std::string& dir) {
	static const string ext(".lua");
	path scripts(dir);
	directory_iterator end;
	for (directory_iterator i(scripts); i != end; ++i) {
		if (!is_directory(*i)) {
			size_t match = i->filename().rfind(ext);

			// Make sure the file ends in .lua
			if (match + ext.length() == i->filename().length()) {
				loadScript(env, i->path().file_string());
			}
		}
	}
}

StackCheck::StackCheck(lua_State* env) {
	top_ = lua_gettop(env);
	env_ = env;
}

StackCheck::~StackCheck() {
	lua_settop(env_, top_);
}
