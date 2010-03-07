/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"
#include "Object.hpp"


namespace Warp {

class Enemy : public Object {
public:

	Enemy(Game* game, Level* level, const std::string& name, int id);

	virtual ~Enemy();

	/** Called when the object is selected */
	void setSelected(bool selected);

	/** Called if the object is finished */
	bool isHitCountMaxed() { return hitCount_ >= hitPoints_; }

	/** Increase hit count */
	void incHitCount() { hitCount_++; }

private:
	static int luaTarget(lua_State* env);
	void loadScriptCallbacks();
	virtual void setWorldTransform(const btTransform& transform);

	virtual void collide(Object* other) { other->onCollision(this); }
	virtual void onCollision(Projectile* p);
	virtual void onCollision(Player* p);

	Ogre::BillboardSet* billboards_;
	int spineNodeIndex_;
	bool selected_;
	int hitPoints_;
	int hitCount_;
	int finalHitCount_;
};

}