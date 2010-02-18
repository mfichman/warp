/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include <Script.hpp>
#include <stdexcept>
#include "BeatLoop.hpp"

using namespace Warp;
using namespace Ogre;
using namespace std;

extern "C" {
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

struct Script::Impl : public Game::Listener {

    enum Event { WAIT_BEAT, WAIT_SPINE_NODE, SLEEP };

    /** Initializes the script */
    Impl(Game* game, const std::string& path) :
        path_(path),
        game_(game), 
        trigger_(0) {    

        lua_State* env = game_->getScriptState();

        lua_getglobal(env, "coroutine");
        lua_getfield(env, -1, "create"); // Function to be called
        lua_remove(env, -2); // Remove "coroutine" table

        // Load the script chunk
        if (luaL_loadfile(env, path_.c_str())) {
            string message(lua_tostring(env, -1));
            lua_pop(env, 1); // "coroutine.create"
            throw runtime_error("Could not load script: " + message);
        }
    
        // Call "coroutine.create(script chunk)" as the argument
        // Return value is the coroutine object
        lua_call(env, 1, 1);

         // Save a reference to the coroutine object (and pop it off the stack)
        coroutine_ = lua_ref(env, LUA_REGISTRYINDEX);

        // Check the stack
        assert(lua_gettop(env) == 0);

        // Add this object as a game listener
        game_->addListener(this);
    }

    ~Impl() {
        lua_unref(game_->getScriptState(), coroutine_);
        game_->removeListener(this);
    }

    bool hasTriggerFired() {
        if (!trigger_) return true;
        lua_State* env = game_->getScriptState();

        // Call the trigger function.  It should return a 
        // boolean on the stack.
        lua_getref(env, trigger_);
        if (lua_pcall(env, 0, 1, 0)) {
            string message(lua_tostring(env, -1));
            lua_pop(env, 1);
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
            lua_pop(env, lua_gettop(env));
            return result;
        } else {
            throw runtime_error("Trigger returned an invalid value");
        }
    }

    /** Determines if the script should be restarted in this frame */
    void onTimeStep() {
        lua_State* env = game_->getScriptState();

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
            lua_pop(env, 1);
            throw runtime_error("Error calling script function: " + message);
        }

        // Get the trigger function
        if (lua_isfunction(env, -1)) {
            // Save a handle to the trigger function
            trigger_ = luaL_ref(env, LUA_REGISTRYINDEX);
            lua_pop(env, 1);

        } else if (lua_isboolean(env, -2) && !lua_toboolean(env, -2)) {
            // Coroutine returned with an error
            string message(lua_tostring(env, -1));
            lua_pop(env, 2);
            throw runtime_error("Error calling script function: " + message);

        } else {
            // Script is complete, never run it again
            game_->removeListener(this);
            lua_pop(env, 2);
        }

        // Check the stack
        assert(lua_gettop(env) == 0);
    }

    Game* game_;

    std::string path_;
    int trigger_;
    int coroutine_;
    Event waitEvent_;
};

Script::Script(Game* game, const std::string& path) : impl_(new Impl(game, path)) {
}

Script::~Script() {

}

/** Methods for sending Ogre values to a script */
lua_State* Warp::operator<<(lua_State* env, const Ogre::Vector3& v) {
    lua_newtable(env);
    lua_pushnumber(env, v.x);
    lua_rawseti(env, -2, 1);
    lua_pushnumber(env, v.y);
    lua_rawseti(env, -2, 2);
    lua_pushnumber(env, v.z);
    lua_rawseti(env, -2, 3);
    return env;
}

lua_State* Warp::operator<<(lua_State* env, const Ogre::Quaternion& q) {
     lua_newtable(env);
     lua_pushnumber(env, q.x);
     lua_rawseti(env, -2, 1);
     lua_pushnumber(env, q.y);
     lua_rawseti(env, -2, 2);
     lua_pushnumber(env, q.z);
     lua_rawseti(env, -2, 3);
     lua_pushnumber(env, q.w);
     lua_rawseti(env, -2, 4);
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
    assert(lua_istable(env, -1));
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
    assert(lua_istable(env, -1));
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
    lua_pop(env, 1);

    return env;

}

lua_State* Warp::operator>>(lua_State* env, Ogre::SceneNode& n) {
    assert(lua_istable(env, -1));
    Vector3 position;
    Quaternion orientation;

    lua_getfield(env, -1, "position");
    if (!lua_isnil(env, -1)) {
        env >> position;
        n.setPosition(position);
    } else {
        lua_pop(env, 1);
    }
    lua_getfield(env, -1, "orientation");
    if (!lua_isnil(env, -1)) {
        env >> orientation;
        n.setOrientation(orientation);
    } else {
        lua_pop(env, 1);
    }
    lua_pop(env, 1);

    return env;

}

lua_State* Warp::operator>>(lua_State* env, Ogre::Light& l) {
    assert(lua_istable(env, -1));
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
    assert(lua_istable(env, -1));
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
    assert(lua_isstring(env, -1)); // Make sure the top of the stack is a string. -1 == top of stack
    s.assign(lua_tostring(env, -1)); // Get the string
    lua_pop(env, 1); // Pop the string value off the lua stack

    return env;
}

/** reads beat loop info from an id and table of values */
lua_State* Warp::operator>>(lua_State* env, Warp::BeatLoop & bl) {
    assert(lua_istable(env, -1));

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

    env >> bl.name;

    return env;
}
