/******************************************************************************
 * Warp: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Objects.hpp>
#include <Script.hpp>
extern "C" {
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

using namespace Warp;
using namespace Ogre;
using namespace std;

struct Objects::Impl {

    ~Impl() {}

    Game* game_;
};

Objects::Objects(Game* game) : impl_(new Impl()) {
    impl_->game_ = game;
}

Objects::~Objects() {

}

void Objects::createBall(const std::string& name, int table) {
    lua_State* env = impl_->game_->getScriptState();
    lua_getref(env, table);

    lua_getfield(env, -1, "radius");
    float radius = lua_isnumber(env, -1) ? lua_tonumber(env, -1) : 1.0f;
    lua_pop(env, 1);
    lua_getfield(env, -1, "mass");
    float mass = lua_isnumber(env, -1) ? lua_tonumber(env, -1) : 1.0f;
    lua_pop(env, 1);
    lua_pop(env, 1);


    SceneNode* node = impl_->game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name);
    node->attachObject(impl_->game_->getSceneManager()->createEntity(name, "Ball.mesh"));
    node->setScale(2*radius, 2*radius, 2*radius);

}

void Objects::createPlane(const std::string& name, int table) {
    lua_State* env = impl_->game_->getScriptState();
    lua_getref(env, table);

    lua_getfield(env, -1, "normal");
    Vector3 normal = Vector3::UNIT_Y;
    if (lua_istable(env, -1)) {
        env >> normal;
    } else {
        lua_pop(env, 1);
    }
    lua_getfield(env, -1, "depth");
    float depth = lua_isnumber(env, -1) ? lua_tonumber(env, -1) : 0.0f;
    lua_pop(env, 1);
    lua_getfield(env, -1, "width");
    float width = lua_isnumber(env, -1) ? lua_tonumber(env, -1) : 1000.0f;
    lua_pop(env, 1);
    lua_getfield(env, -1, "height");
    float height = lua_isnumber(env, -1) ? lua_tonumber(env, -1) : 1000.0f;
    lua_pop(env, 1);
    lua_pop(env, 1);

    Plane plane(normal, -depth);
    MeshManager::getSingleton().createPlane(name, "Default", plane, width, height, 100, 100, true, 1, 100, 100, Vector3::UNIT_Z);

    SceneNode* node = impl_->game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name);
    Entity* entity = impl_->game_->getSceneManager()->createEntity(name, name);
    entity->setMaterialName("Asphalt");
    node->attachObject(entity);
}