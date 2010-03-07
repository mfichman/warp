/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Player.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Enemy.hpp"
#include "Object.hpp"
#include "Projectile.hpp"
#include "DynamicTube.hpp"

#include <OIS/OIS.h>

using namespace Warp;
using namespace Ogre;
using namespace std;

#define POSITION_SMOOTHNESS 0.60f
#define ROTATION_SMOOTHNESS 0.05f

#define SPAWN_DISTANCE 500.0f
#define MAX_COOLDOWN 0.125f;

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Player::Player(Game* game, Level* level, const string& name, int id) :
	Object(game, level, name, id),
	cooldown_(0.0f) {

	setPosition(Vector3(0, 295, 5));
}

Player::~Player() {
}

/** Called by Bullet to update the scene node */
void Player::setWorldTransform(const btTransform& transform) {
	
	// BEGIN TODO
	const btVector3& btposition = transform.getOrigin();
	const btVector3& btvelocity = body_->getLinearVelocity();
	Vector3 position(btposition.x(), btposition.y(), btposition.z());
	Vector3 velocity(btvelocity.x(), btvelocity.y(), btvelocity.z());    

	// Get the player location
    playerProjection_ = level_->getTube()->getSpineProjection(position, playerProjection_.index);

	// Get the spawn location
	spawnProjection_ = level_->getTube()->getSpineProjection(playerProjection_.distance + SPAWN_DISTANCE, spawnProjection_.index);

	const SpineProjection& projection = playerProjection_;
    assert(projection.forward != Vector3::ZERO);
    assert(projection.position - position != Vector3::ZERO);

    // Up points toward spine node
    Vector3 forward = projection.forward;
	Vector3 up = (projection.position - position).normalisedCopy();//Vector3::UNIT_Y;
	
	// Need to make sure up is orthogonal to forward
    assert(up.crossProduct(forward) != Vector3::ZERO);
	
    // Project the gravity vector into the plane with "forward" as the
    // normal vector.  This forces the ball to the outside of the ring, and
    // removes and component that would make the ball move forward/backward
    up = up - (up.dotProduct(projection.forward)) * projection.forward;
    assert(up != Vector3::ZERO);
    up.normalise();
    Vector3 right = up.crossProduct(forward);
	// END TODO
	
	right.normalise();
    up.normalise();
    forward.normalise(); 
	
	node_->setOrientation(Quaternion(-right, up, -forward));
	node_->setPosition(btposition.x(), btposition.y(), btposition.z());
	game_->getCamera()->setPosition(position - forward*8.0 + up*1.7);
	game_->getCamera()->setOrientation(Quaternion(-right, up, -forward));
}

/** Called when a new frame is detected */
void Player::onTimeStep() {
	Object::onTimeStep();
	computeForces();
	updateRay();
	fireMissiles();
}

void Player::fireMissiles() {
	if (cooldown_ <= 0.0f && targets_.size() > 0) {
		set<Enemy*>::iterator i = targets_.begin();
		Projectile* p = level_->createProjectile("Photon");
		p->setTarget(*i);
		p->setPosition(forward_ + getPosition());
		targets_.erase(i);
		cooldown_ = MAX_COOLDOWN;
	} else if (cooldown_ > 0.0f) {
		cooldown_ -= TIME_STEP;
	}
}


void Player::computeForces() {
	btVector3 btposition = body_->getCenterOfMassPosition();
	btVector3 btvelocity = body_->getLinearVelocity();
	Vector3 position(btposition.x(), btposition.y(), btposition.z());
	Vector3 velocity(btvelocity.x(), btvelocity.y(), btvelocity.z());

	// Get the player location
    playerProjection_ = level_->getTube()->getSpineProjection(position, playerProjection_.index);

	// Get the spawn location
	spawnProjection_ = level_->getTube()->getSpineProjection(playerProjection_.distance + SPAWN_DISTANCE, spawnProjection_.index);

	const SpineProjection& projection = playerProjection_;
    assert(projection.forward != Vector3::ZERO);
    assert(projection.position - position != Vector3::ZERO);

    // Up points toward spine node
    Vector3 forward = projection.forward;
	Vector3 up = (projection.position - position).normalisedCopy();//Vector3::UNIT_Y;
	
	// Need to make sure up is orthogonal to forward
    assert(up.crossProduct(forward) != Vector3::ZERO);
	
    // Project the gravity vector into the plane with "forward" as the
    // normal vector.  This forces the ball to the outside of the ring, and
    // removes and component that would make the ball move forward/backward
    up = up - (up.dotProduct(projection.forward)) * projection.forward;
    assert(up != Vector3::ZERO);
    up.normalise();
    Vector3 right = up.crossProduct(forward);

    // Apply gravity and hover forces:
	// We want the ship to hover at 2.0 meters above the ground
	float distance = (projection.position - position).length();

	float friction = 10.0f;
	float centralForce = 40.0f;
	if (distance > 5.5f) {
		float v = velocity.dotProduct(up);
		Vector3 force = ((distance - 6.0f) * centralForce * mass_ - friction * v) * up;
		body_->applyCentralForce(btVector3(force.x, force.y, force.z));
	} else {
		Vector3 gravity = -20.0f * mass_ * up;
		body_->applyCentralForce(btVector3(gravity.x, gravity.y, gravity.z));
	}

    // Apply user control forces
	if (game_->getKeyboard()->isKeyDown(OIS::KC_RIGHT)) {
		body_->applyCentralForce(-20*btVector3(right.x, right.y, right.z));
    }                               
	
	if (game_->getKeyboard()->isKeyDown(OIS::KC_LEFT)) {
		body_->applyCentralForce(20*btVector3(right.x, right.y, right.z));
	}
    
	if (game_->getKeyboard()->isKeyDown(OIS::KC_UP)) {
		body_->applyCentralForce(20*btVector3(forward.x, forward.y, forward.z));
	}
	if (game_->getKeyboard()->isKeyDown(OIS::KC_DOWN)) {
		body_->applyCentralForce(-20*btVector3(forward.x, forward.y, forward.z));
	}

	forward_ = forward.normalisedCopy();
	right_ = right.normalisedCopy();
	up_ = up.normalisedCopy();

	right.normalise();
    up.normalise();
    forward.normalise(); 
}

void Player::updateRay() {

	const OIS::MouseState& state = game_->getMouse()->getMouseState();
	if (state.buttonDown(OIS::MB_Right) || state.buttonDown(OIS::MB_Left)) {

		// Create picking ray and test for the closest btRigidBody
		float scrx = (float)state.X.abs/game_->getWindow()->getWidth();
		float scry = (float)state.Y.abs/game_->getWindow()->getHeight();
		Ray ray = game_->getCamera()->getCameraToViewportRay(scrx, scry);

		btVector3 from(ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z);
		btVector3 to(ray.getPoint(1000).x, ray.getPoint(1000).y, ray.getPoint(1000).z);

		btCollisionWorld::ClosestRayResultCallback callback(from, to);
		game_->getWorld()->rayTest(from, to, callback);
		if (callback.hasHit()) {
			btRigidBody* body = btRigidBody::upcast(callback.m_collisionObject);
			if (body) {
				Object* o = static_cast<Object*>(body->getUserPointer());
				Enemy* e = dynamic_cast<Enemy*>(o);
				if (e && !e->isHitCountMaxed()) {
					
					e->addTracker(this);
					e->setSelected(true);
					e->incHitCount();
					targets_.insert(e);
				}
			}
		}
	}
}

void Player::onTargetDelete(Object* object) {
	targets_.erase(static_cast<Enemy*>(object));
}

const SpineProjection& Player::getPlayerProjection() const {
    return playerProjection_;
}

const SpineProjection& Player::getSpawnProjection() const {
	return spawnProjection_;
}


