/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"
#include "Collidable.hpp"

#include <Bullet/btBulletDynamicsCommon.h>

namespace Warp {

class Player : public Collidable, public GameListener, public btMotionState {
public:
	/** Creates a new ball */
    Player(Game* game, Level* level, const std::string& name);

    /** Destructor */
    ~Player();

	/** Returns the player's current position */
    const Ogre::Vector3& getPosition() const;

	/** Gets the current player spine node projection */
	const SpineProjection& getPlayerProjection() const;

	/** Returns the spine projection where enemies should spawn */
	const SpineProjection& getSpawnProjection() const;
		
private:
	Player(const Player&);
    Player& operator=(const Player&);

	// Collision callbacks
	virtual void collide(Collidable* other) { other->onCollision(this); }
	virtual void onCollision(Object* object) { std::cout << "Hit" << std::endl; }

	// Bullet callbacks
	virtual void getWorldTransform(btTransform& transform) const;
	virtual void setWorldTransform(const btTransform& transform);

	virtual void onTimeStep();
	void computeForces();
	void updateRay();

	Game* game_;
	Level* level_;

	// Ogre scene data
	std::string name_;
	Ogre::SceneNode* node_;
	Ogre::SceneNode* shipNode_;

	// Collision data
	std::auto_ptr<btCollisionShape> shape_;
	std::auto_ptr<btRigidBody> body_;

    // Information about position and orientation from Bullet
    btTransform transform_;
	Ogre::Vector3 position_;

	SpineProjection spawnProjection_;
	SpineProjection playerProjection_;

};

}
