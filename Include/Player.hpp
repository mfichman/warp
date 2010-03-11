/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"
#include "Object.hpp"

#include <Bullet/btBulletDynamicsCommon.h>

namespace Warp {

class Player : public Object {
public:
	enum Throttle { TH_UP, TH_DOWN, TH_NORMAL };

	/** Creates a new ball */
    Player(Game* game, Level* level, const std::string& name, int id);

    /** Destructor */
    virtual ~Player();

	/** Gets the current player spine node projection */
	const SpineProjection& getPlayerProjection() const;

	/** Returns the spine projection where enemies should spawn */
	const SpineProjection& getSpawnProjection(float distance) const;

	/** Call to update the player */
	virtual void onTimeStep();

	/** Return shields */
	float getShields() const { return shields_; }

	/** Returns number of points */
	int getPoints() const { return points_; }

	/** Add points */
	void addPoints(int points) { points_ += points; }
		
private:
	Player(const Player&);
    Player& operator=(const Player&);

	virtual void collide(ObjectPtr other) { other->onCollision(this); }
	virtual void onCollision(EnemyPtr enemy);
	virtual void onCollision(ProjectilePtr projectile);

	// Bullet callbacks
	virtual void setWorldTransform(const btTransform& transform);

	// Called when a target is delted
	virtual void onTargetDelete(ObjectPtr object);

	void computeForces();
	void updateRay();
	void fireMissiles();

	SpineProjection spawnProjection_;
	SpineProjection playerProjection_;

	Ogre::Vector3 right_;
	Ogre::Vector3 up_;
	Ogre::Vector3 forward_;
	std::list<EnemyPtr> targets_;
	float cooldown_;
	float shields_;
	int points_;
	Throttle throttle_;
};

}
