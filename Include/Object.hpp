/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include <list>
#include <boost/shared_ptr.hpp>
#include <Bullet/btBulletDynamicsCommon.h>

namespace Warp {

class Object : public btMotionState {
public:

	/** Creates a new script and begins executing it inside a coroutine */
    Object(Game* game, Level* level, const std::string& name, int id);

    /** Destructor */
    virtual ~Object();

	/** Sets the object's target */
	void setTarget(Object* target);

	/** Sets the position of this object */
	void setPosition(const Ogre::Vector3& p);

	/** Sets the speed of the object */
	void setSpeed(float speed) { speed_ = speed; }

	/** Returns true if the object is alive */
	bool isAlive() const { return alive_; }

	/** Returns the scene node */
	Ogre::SceneNode* getSceneNode() { return node_; }

	/** Returns the position of this object */
	Ogre::Vector3 getPosition() { return node_->getPosition(); }

	/** Returns the transform */
	const btTransform& getTransform() const { return transform_; }

	/** Returns the rigid body */
	btRigidBody* getRigidBody() { return body_.get(); }

	/** Returns the number of objects tracking this object */
	size_t getTrackerCount() { return trackers_.size(); }

	/** Adds a projectile that tracks this object */
	void addTracker(Object* p);

	/** Removes a projectile from this object */
	void removeTracker(Object* p);

	/* Called to collide this object with another */
	virtual void collide(Object* other) {}

	/* Called when a collision with an object occurs (enemy, or other obstacle) */
	virtual void onCollision(Enemy* enemy) { }

	/* Called when a collision with a player occurs */
	virtual void onCollision(Player* player) { }

	/* Called when a collision with a projectile occurs */
	virtual void onCollision(Projectile* projectile) { }

	/** Called every timestep by Level */
	virtual void onTimeStep();

	/** Called when the target is deleted */
	void onTargetDelete();

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
	std::list<boost::shared_ptr<SubObject>> subObjects_;

	
	// Reference to the Lua class
	int table_; 
		
	Object* target_;

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
	static int luaSet(lua_State* env);
	static int luaExplode(lua_State* env);
	static int luaDestroy(lua_State* env);
	static int luaGetPosition(lua_State* env);

	friend lua_State* Warp::operator>>(lua_State* env, Object& e);

	bool exploded_;
	bool alive_;

	// Projectiles tracking this object
	std::list<Object*> trackers_;

	float speed_;
};

}
