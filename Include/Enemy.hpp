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

private:
	static int luaTarget(lua_State* env);
	void loadScriptCallbacks();
	virtual void collide(Object* other) { other->onCollision(this); }
	virtual void onCollision(Projectile* p);

	Ogre::BillboardSet* billboards_;
	int spineNodeIndex_;
	bool selected_;
};

}