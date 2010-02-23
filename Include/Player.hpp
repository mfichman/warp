/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include <Bullet/btBulletDynamicsCommon.h>

namespace Warp {

class Player : public GameListener, public btMotionState {
public:
	/** Creates a new ball */
    Player(Game* game, Level* level, const std::string& name);

    /** Destructor */
    ~Player();

	/** Returns the player's current position */
    const Ogre::Vector3& getPosition() const;

	/** Gets the current spine node index */
	int getSpineNodeIndex() const;
		
private:
	Player(const Player&);
    Player& operator=(const Player&);

	// Bullet callbacks
	virtual void getWorldTransform(btTransform& transform) const;
	virtual void setWorldTransform(const btTransform& transform);

	virtual void onTimeStep();

	Game* game_;
	Level* level_;

	// Ogre scene data
	std::string name_;
	Ogre::SceneNode* node_;

	// Collision data
	std::auto_ptr<btCollisionShape> shape_;
	std::auto_ptr<btRigidBody> body_;

    // Information about position and orientation from Bullet
    btTransform transform_;
	Ogre::Vector3 position_;

    int lastSpineNodeIndex_;

};

}
