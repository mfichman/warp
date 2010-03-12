/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"
#include "Enemy.hpp"

namespace Warp {

class Projectile : public Object {
public:
	/** Creates a new projectile */
	Projectile(Game* game, Level* level, const std::string& type, int id);

	/** Destructor */
	virtual ~Projectile();


private:
	virtual void collide(ObjectPtr other);
	virtual void onCollision(EnemyPtr enemy);
	virtual void onCollision(PlayerPtr player);

	virtual void onTimeStep();

	Ogre::BillboardSet* billboards_;
	std::auto_ptr<btSphereShape> shape_;
	bool hit_;
	float time_;
	float immunity_;
};

}