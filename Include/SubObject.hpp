/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include "SharedObject.hpp"
#include <list>
#include <string>
#include <memory>
#include <Bullet/btBulletDynamicsCommon.h>

namespace Warp {

class SubObject : public btMotionState, public SharedObject {
public:
	/** Creates a new subobject */
	SubObject(Game* game, Object* parent, const std::string& name, const std::string& mesh);

	/** Destrutor */
	~SubObject();

	/** Returns the shape */
	btCollisionShape* getShape() const { return shape_.get(); }

	/** Returns scene node */
	Ogre::SceneNode* getSceneNode() const { return node_; }

	Ogre::Entity* getEntity() const { return entity_; }

	/** Explodes the subobject from is parent */
	void separateFromParent();

	/** Called per-timestep */
	void onTimeStep();

	Ogre::Vector3 getOffsets() const;

private:
	// Bullet callbacks
	virtual void getWorldTransform(btTransform& transform) const;
	virtual void setWorldTransform(const btTransform& transform);

	// Collision data
	std::auto_ptr<btCollisionShape> shape_;
	std::auto_ptr<btRigidBody> body_;
	btTransform transform_;

	Game* game_;
	Object* parent_;
	std::string name_;

	// Ogre data
	Ogre::SceneNode* node_;
	Ogre::Entity* entity_;

	bool separated_;
};

}