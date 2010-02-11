/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Objects.hpp>
#include <Script.hpp>
extern "C" {
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

using namespace Criterium;
using namespace Ogre;
using namespace std;

struct Objects::Impl {

    ~Impl() {}

    /** Called when the body is moved to move the corresponding scene node */
	static void onBodyMoved(dBodyID body) {

		// Update the position of the scene node attached to this body.
		// Position is in global coordinates.
		SceneNode* node = static_cast<SceneNode*>(dBodyGetData(body));
		const dReal* pos = dBodyGetPosition(body);
		const dReal* quat = dBodyGetQuaternion(body);
	    
		// N.B.: ODE orders the quat as (w, x, y, z) (so quat[0] = w)
		node->setPosition(pos[0], pos[1], pos[2]);
		node->setOrientation(quat[0], quat[1], quat[2], quat[3]);
	}

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

    dMass massDist;
    dMassSetSphereTotal(&massDist, mass, radius);

    SceneNode* node = impl_->game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name);
    node->attachObject(impl_->game_->getSceneManager()->createEntity(name, "Ball.mesh"));
    node->setScale(2*radius, 2*radius, 2*radius);

    dBodyID body = dBodyCreate(impl_->game_->getWorld());
    dBodySetMass(body, &massDist);
    dBodySetData(body, node);
    dBodySetMovedCallback(body, &Impl::onBodyMoved);

    dGeomID geom = dCreateSphere(impl_->game_->getSpace(), radius);
    dGeomSetBody(geom, body);
    dGeomSetCategoryBits(geom, TYPE_BALL);
    dGeomSetCollideBits(geom, TYPE_ANY);

    node->setUserAny(Any(body));
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
   
    dGeomID geom = dCreatePlane(impl_->game_->getSpace(), normal.x, normal.y, normal.z, depth);
    dGeomSetCategoryBits(geom, TYPE_PLANE);
    dGeomSetCollideBits(geom, TYPE_ANY);
}