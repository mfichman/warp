/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

namespace Warp {

/* 
 * This class uses dynamic dispatch to decide which version of onCollision()
 * gets called.
 */
class Collidable {
public:
	virtual ~Collidable() {}

	/* Called to collide this object with another */
	virtual void collide(Collidable* other) {}

	/* Called when a collision with an object occurs (enemy, or other obstacle) */
	virtual void onCollision(Object* object) {}

	/* Called when a collision with a player occurs */
	virtual void onCollision(Player* player) {}

	/* Called when a collision with a projectile occurs */
	virtual void onCollision(Projectile* projectile) {}
};


}