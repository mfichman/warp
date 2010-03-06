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
	/** Creates a new ball */
    Player(Game* game, Level* level, const std::string& name, int id);

    /** Destructor */
    virtual ~Player();

	/** Gets the current player spine node projection */
	const SpineProjection& getPlayerProjection() const;

	/** Returns the spine projection where enemies should spawn */
	const SpineProjection& getSpawnProjection() const;

	/** Call to update the player */
	virtual void onTimeStep();
		
private:
	Player(const Player&);
    Player& operator=(const Player&);

	virtual void collide(Object* other) { other->onCollision(this); }

	// Bullet callbacks
	virtual void setWorldTransform(const btTransform& transform);

	void computeForces();
	void updateRay();

	SpineProjection spawnProjection_;
	SpineProjection playerProjection_;

	Ogre::Vector3 right_;
	Ogre::Vector3 up_;
	Ogre::Vector3 forward_;

};

}
