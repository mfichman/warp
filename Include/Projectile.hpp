/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"
#include "Collidable.hpp"
#include <Bullet/btBulletDynamicsCommon.h>

namespace Warp {

class Projectile : public Collidable, public btMotionState {
public:
	/** Creates a new projectile */
	Projectile(Game* game, const std::string& material, int id, const Ogre::Vector3& pos, Object* target);

	/** Destructor */
	~Projectile();

	/* Called when the tracked object changes position */
	void onTargetMovement(const Ogre::Vector3& newPosition);

	/* Called when the tracked object is removed from memory */
	void onTargetDelete();

	/** Returns true if the object is alive */
	bool isAlive() const { return alive_; }

	/** Called on timestep */
	void onTimeStep();

private:
	// Bullet callbacks
	virtual void getWorldTransform(btTransform& transform) const;
	virtual void setWorldTransform(const btTransform& transform);

	// Collision callbacks
	virtual void collide(Collidable* other) { other->onCollision(this); }
	virtual void onCollision(Object* object);

	void deactivatePhysics();

	Game* game_;

	// Collision data
	std::auto_ptr<btCollisionShape> shape_;
	std::auto_ptr<btRigidBody> body_;
	btTransform transform_;
	bool alive_;

	Ogre::SceneNode* node_;
	Ogre::BillboardSet* billboards_;
	std::string name_;
	Object* target_;
	float time_;
};

}