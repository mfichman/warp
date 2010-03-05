/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"
#include "Collidable.hpp"

#include <list>
#include <boost/shared_ptr.hpp>
#include <Bullet/btBulletDynamicsCommon.h>

namespace Warp {

class Object : public Collidable, public btMotionState {
public:

	/** Creates a new script and begins executing it inside a coroutine */
    Object(Game* game, Level* level, const std::string& name, int id);

    /** Destructor */
    ~Object();

	/** Returns the scene node */
	Ogre::SceneNode* getSceneNode() { return node_; }

	/** Returns the transform */
	const btTransform& getTransform() const { return transform_; }

	/** Returns the rigid body */
	btRigidBody* getRigidBody() { return body_.get(); }

	/** Called when the object is selected */
	void select();

	/** Returns true if the object is alive */
	bool isAlive() const { return alive_; }

	/** Called every timestep by Level */
	void onTimeStep();

	/** Adds a projectile that tracks this object */
	void addProjectile(Projectile* p);

	/** Removes a projectile from this object */
	void removeProjectile(Projectile* p);

	size_t projectileCount() { return projectiles_.size(); }

private:
    Object(const Object&);
    Object& operator=(const Object&);
	void loadScriptCallbacks();

	// Collision callbacks
	virtual void collide(Collidable* other) { other->onCollision(this); }
	virtual void onCollision(Player* player) { }
	virtual void onCollision(Projectile* projectile);

	// Bullet callbacks
	virtual void getWorldTransform(btTransform& transform) const;
	virtual void setWorldTransform(const btTransform& transform);

	// Lua callbacks
	static int luaAddEntity(lua_State* env);
	static int luaSetEntity(lua_State* env);
	static int luaAddParticleSystem(lua_State* env);
	static int luaSetParticleSystem(lua_State* env);
	static int luaSet(lua_State* env);
	static int luaExplode(lua_State* env);
	static int luaDestroy(lua_State* env);
	static int luaTarget(lua_State* env);
	static int luaWarningDestroyed(lua_State* env);
	static int luaGetPosition(lua_State* env);

	void callMethod(const std::string& method);

	friend lua_State* Warp::operator>>(lua_State* env, Object& e);

	Game* game_;
	Level* level_;
	std::string type_;
	std::string name_;

	// Reference to the Lua class
	int table_; 

	// Ogre scene data
	Ogre::SceneNode* node_;
	Ogre::BillboardSet* billboards_;
	std::list<Ogre::AnimationState*> activeAnimations_;
	std::list<boost::shared_ptr<SubObject>> subObjects_;

	// Projectiles tracking this object
	std::list<Projectile*> projectiles_;

	// Physics data
	std::auto_ptr<btCompoundShape> shape_;
	std::auto_ptr<btRigidBody> body_;
	btTransform transform_;

	bool exploded_;
	bool alive_;

	int spineNodeIndex_;
};


}
