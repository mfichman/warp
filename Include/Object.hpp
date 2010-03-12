/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include "Tracker.hpp"
#include <list>
#include <set>
#include <Bullet/btBulletDynamicsCommon.h>

namespace Warp {

class Object : public btMotionState, public Tracker {
public:

	/** Creates a new script and begins executing it inside a coroutine */
    Object(Game* game, Level* level, const std::string& name, int id);

    /** Destructor */
    virtual ~Object();

	/** Sets the object's target */
	void setTarget(ObjectPtr target);

	/** Sets the position of this object */
	void setPosition(const Ogre::Vector3& p);

	/** Sets the quaternion for this object */
	void setOrientation(const Ogre::Quaternion& q);

	/** Sets the velocity */
	void setVelocity(const Ogre::Vector3& velocity);

	/** Returns true if the object is alive */
	bool isAlive() const { return alive_; }

	/** Returns the scene node */
	Ogre::SceneNode* getSceneNode() { return node_; }

	/** Returns the position of this object */
	Ogre::Vector3 getPosition() const;

	/** Returns the quaternion for this object */
	Ogre::Quaternion getOrientation() { return node_->getOrientation(); }

	/** Returns the velocity of the object */
	Ogre::Vector3 getVelocity();

	/** Returns the transform */
	const btTransform& getTransform() const { return transform_; }

	/** Returns the rigid body */
	btRigidBody* getRigidBody() { return body_.get(); }

	/** Returns the number of objects tracking this object */
	size_t getTrackerCount() { return trackers_.size(); }

	/** Adds a projectile that tracks this object */
	void addTracker(TrackerPtr p);

	/** Removes a projectile from this object */
	void removeTracker(TrackerPtr p);

	/* Called to collide this object with another */
	virtual void collide(ObjectPtr other) {}

	/* Called when a collision with an object occurs (enemy, or other obstacle) */
	virtual void onCollision(EnemyPtr enemy) {}

	/* Called when a collision with a player occurs */
	virtual void onCollision(PlayerPtr player) {}

	/* Called when a collision with a projectile occurs */
	virtual void onCollision(ProjectilePtr projectile) { }

	/** Called every timestep by Level */
	virtual void onTimeStep();

	/** Called when the target is deleted */
	virtual void onTargetDelete(ObjectPtr target);

	/** Returns the object name */
	const std::string& getName() const { return name_; }

	/** Whatever */
	void clearAllTrackers();

protected:
	void callMethod(const std::string& method);
	static int luaWarningDestroyed(lua_State* env);

	Game* game_;
	Level* level_;

	std::string type_;
	std::string name_;

	// Physics data
	std::auto_ptr<btCompoundShape> shape_;
	std::auto_ptr<btRigidBody> body_;
	btTransform transform_;
	float mass_;

	// Ogre scene data
	Ogre::SceneNode* node_;
	std::list<Ogre::AnimationState*> activeAnimations_;
	std::list<SubObjectPtr> subObjects_;

	
	// Reference to the Lua class
	int table_; 
		
	ObjectPtr target_;

private:
    Object(const Object&);
    Object& operator=(const Object&);
	void loadScriptCallbacks();

	// Bullet callbacks
	virtual void getWorldTransform(btTransform& transform) const;
	virtual void setWorldTransform(const btTransform& transform);

	// Lua callbacks
	static int luaAddEntity(lua_State* env);
	static int luaSetEntity(lua_State* env);
	static int luaAddParticleSystem(lua_State* env);
	static int luaSetParticleSystem(lua_State* env);
	static int luaExplode(lua_State* env);
	static int luaDestroy(lua_State* env);
	static int luaGetPosition(lua_State* env);
	static int luaSetPosition(lua_State* env);
	static int luaGetVelocity(lua_State* env);
	static int luaSetVelocity(lua_State* env);
	static int luaGetOrientation(lua_State* env);
	static int luaSetOrientation(lua_State* env);
	static int luaCreateMissile(lua_State* env);
	static int luaGetTarget(lua_State* env);
	static int luaSetTarget(lua_State* env);

	friend lua_State* Warp::operator<<(lua_State* env, ObjectPtr e);

	bool exploded_;
	bool alive_;

	// Projectiles tracking this object
	std::set<TrackerPtr> trackers_;
};

}
