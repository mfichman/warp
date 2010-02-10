/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Script.hpp>
#include <stdexcept>

using namespace Criterium;
using namespace Ogre;
using namespace std;

extern "C" {
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

struct Script::Impl : public Game::Listener {


    /** Initializes the script */
    void init() {
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
    }

    ~Impl() {
        lua_unref(game_->getScriptState(), coroutine_);
    }

    /** Determines if the script should be restarted in this frame */
    void onTimeStep() {
        if (framesToWait_ < 0) {
            lua_State* env = game_->getScriptState();

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

            // Get the time the coroutine will be run next
            if (lua_isnumber(env, -1)) {
                framesToWait_ = lua_tointeger(env, -1);
            } else if (!lua_toboolean(env, -2)) {
                string message(lua_tostring(env, -1));
                lua_pop(env, 2);
                throw runtime_error("Error calling script function: " + message);

            } else {
                // Script is complete, never run it again
                game_->removeListener(this);
            }
            lua_pop(env, 2);

            // Check the stack
            assert(lua_gettop(env) == 0);

        } else {
            framesToWait_--;
        }

    }

    Game* game_;

    std::string path_;
    int framesToWait_;
    int coroutine_;
};

Script::Script(Game* game, const std::string& path) : impl_(new Impl()) {
    impl_->path_ = path;
    impl_->game_ = game;
    impl_->framesToWait_ = 0;
    impl_->init();
    impl_->game_->addListener(impl_.get());
}


/** Methods for sending Ogre and ODE values to a script */
lua_State* Criterium::operator<<(lua_State* env, const Ogre::Vector3& v) {
    lua_newtable(env);
    lua_pushnumber(env, v.x);
    lua_rawseti(env, -2, 1);
    lua_pushnumber(env, v.y);
    lua_rawseti(env, -2, 2);
    lua_pushnumber(env, v.z);
    lua_rawseti(env, -2, 3);
    return env;
}

lua_State* Criterium::operator<<(lua_State* env, const Ogre::Quaternion& q) {
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

lua_State* Criterium::operator<<(lua_State* env, const Ogre::SceneNode& n) {
    dBodyID body = any_cast<dBodyID>(n.getUserAny());
    lua_newtable(env);
    if (body) {
        env << Ogre::Vector3(dBodyGetPosition(body));
        lua_setfield(env, -2, "position");
        env << Ogre::Quaternion((Real*)dBodyGetQuaternion(body));
        lua_setfield(env, -2, "orientation");
        env << Ogre::Vector3(dBodyGetLinearVel(body));
        lua_setfield(env, -2, "linearvel");
        env << Ogre::Vector3(dBodyGetAngularVel(body));
        lua_setfield(env, -2, "angularvel");        
    } else {
        env << n.getPosition();
        lua_setfield(env, -2, "position");
        env << n.getOrientation();
        lua_setfield(env, -2, "orientation");
        env << Ogre::Vector3::ZERO;
        lua_setfield(env, -2, "linearvel");
        env << Ogre::Vector3::ZERO;
        lua_setfield(env, -2, "angularvel");       
    }

    return env;
}

lua_State* Criterium::operator<<(lua_State* env, const Ogre::Light& l) {
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

lua_State* Criterium::operator<<(lua_State* env, const Ogre::ColourValue& c) {
    lua_pushnumber(env, c.r);
    lua_rawseti(env, -2, 1);
    lua_pushnumber(env, c.b);
    lua_rawseti(env, -2, 2);
    lua_pushnumber(env, c.g);
    lua_rawseti(env, -2, 3);

    return env;
}


/** Methods for reading Ogre and ODE values from a script */
lua_State* Criterium::operator>>(lua_State* env, Ogre::Vector3& v) {
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

lua_State* Criterium::operator>>(lua_State* env, Ogre::Quaternion& q) {
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

lua_State* Criterium::operator>>(lua_State* env, Ogre::SceneNode& n) {
    assert(lua_istable(env, -1));
    dBodyID body = any_cast<dBodyID>(n.getUserAny());
    Vector3 position, linearvel, angularvel;
    Quaternion orientation;

    if (body) {
        lua_getfield(env, -1, "position");
        if (!lua_isnil(env, -1)) {
            env >> position;
            dBodySetPosition(body, position.x, position.y, position.z);
        } else {
            lua_pop(env, 1);
        }
        lua_getfield(env, -1, "orientation");
        if (!lua_isnil(env, -1)) {
            env >> orientation;
            dBodySetQuaternion(body, (dReal*)&orientation);
        } else {
            lua_pop(env, 1);
        }
        lua_getfield(env, -1, "linearvel");
        if (!lua_isnil(env, -1)) {
            env >> linearvel;
            dBodySetLinearVel(body, linearvel.x, linearvel.y, linearvel.z);
        } else {
            lua_pop(env, 1);
        }
        lua_getfield(env, -1, "angularvel");
        if (!lua_isnil(env, -1)) {
            env >> angularvel;
            dBodySetAngularVel(body, angularvel.x, angularvel.y, angularvel.z);
        }  else {
            lua_pop(env, 1);
        }     
    } else {
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
    }
    lua_pop(env, 1);

    return env;

}

lua_State* Criterium::operator>>(lua_State* env, Ogre::Light& l) {
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


lua_State* Criterium::operator>>(lua_State* env, Ogre::ColourValue& c) {
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