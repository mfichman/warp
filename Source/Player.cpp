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
using namespace boost;

#define POSITION_SMOOTHNESS 0.60f
#define ROTATION_SMOOTHNESS 0.05f

#define SPAWN_DISTANCE 500.0f
#define MAX_COOLDOWN 0.125f;

#define IS_KEY_DOWN(key) game_->getKeyboard()->isKeyDown(key)

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Player::Player(Game* game, Level* level, const string& name, int id) :
	Object(game, level, name, id),
	cooldown_(0.0f),
	shields_(100.0f),
	points_(0),
	throttle_(TH_NORMAL) {

    Vector3 position(0, 245, 5);
	setPosition(position);

    playerProjection_ = level_->getTube()->getSpineProjection(position, playerProjection_.index);
    // set the camera behind the player
    Vector3 forward = playerProjection_.forward;
    Vector3 up = Vector3::UNIT_Y;
    game->getCamera()->setPosition(position - forward*5.0 + up*1);
    game->getCamera()->setDirection(forward);
}

Player::~Player() {
}

/** Called by Bullet to update the scene node */
void Player::setWorldTransform(const btTransform& transform) {
	
    // Set local info
    transform_ = transform;

	// BEGIN TODO
	const btVector3& btposition = transform.getOrigin();
	const btVector3& btvelocity = body_->getLinearVelocity();
	const Vector3 position(btposition.x(), btposition.y(), btposition.z());
	const Vector3 velocity(btvelocity.x(), btvelocity.y(), btvelocity.z());  

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
    // removes any component that would make the ball move forward/backward
    up = up - (up.dotProduct(projection.forward)) * projection.forward;
    assert(up != Vector3::ZERO);
    up.normalise();
    Vector3 left = up.crossProduct(forward);
	// END TODO
	
	left.normalise();
    up.normalise();
    forward.normalise(); 

	node_->setOrientation(Quaternion(-left, up, -forward));
	node_->setPosition(position);

    Vector3 target_position = (position - forward*2.0 + up*1.0);
    //Quaternion target_orientation = Quaternion(-left, up, -forward);
    Camera* camera = game_->getCamera();

    // interpolate the camera component-wise because interpolating the quaternion
    //  will do it based on angles rather than axes.
    Vector3 cam_right = camera->getDerivedRight();
    Vector3 cam_up = camera->getDerivedUp();
    Vector3 cam_forward = camera->getDerivedDirection();
#define CAM_ALPHA .9    
    camera->setPosition((1.0 - CAM_ALPHA) * target_position + CAM_ALPHA * camera->getPosition());
    Vector3 new_right = (1.0 - CAM_ALPHA) * -left + CAM_ALPHA * cam_right;
    Vector3 new_up = (1.0 - CAM_ALPHA) * up + CAM_ALPHA * cam_up;
    Vector3 new_forward = (1.0 - CAM_ALPHA) * forward + CAM_ALPHA * cam_forward;
    camera->setOrientation(Quaternion(new_right, new_up, -new_forward));
	//camera->setOrientation(Quaternion(-left, up, -forward));
	//camera->setPosition(target_position);
    //cout << velocity.dotProduct(forward);
}

/** Called on each physics time step */
void Player::onTimeStep() {
	Object::onTimeStep();
	computeForces();
	updateRay();
	fireMissiles();
}

void Player::fireMissiles() {
	if (cooldown_ <= 0.0f && targets_.size() > 0) {
		list<EnemyPtr>::iterator i = targets_.begin();
		ProjectilePtr p = level_->createProjectile("Photon");
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


	// Get the spawn location
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
	if (IS_KEY_DOWN(OIS::KC_UP) || IS_KEY_DOWN(OIS::KC_W)) {
		if (throttle_ != TH_UP) {
			callMethod("onThrottleUp");
			throttle_ = TH_UP;
		}
		body_->applyCentralForce(50*btVector3(forward.x, forward.y, forward.z));
	} else if (IS_KEY_DOWN(OIS::KC_DOWN) || IS_KEY_DOWN(OIS::KC_S)) {
		if (throttle_ != TH_DOWN) {
			callMethod("onThrottleDown");
			throttle_ = TH_DOWN;
		}
		body_->applyCentralForce(-30*btVector3(forward.x, forward.y, forward.z));
	} else {
		if (throttle_ != TH_NORMAL) {
			callMethod("onThrottleNormal");
			throttle_ = TH_NORMAL;
		}
	}

	if (IS_KEY_DOWN(OIS::KC_RIGHT) || IS_KEY_DOWN(OIS::KC_D)) {
		body_->applyCentralForce(-40*btVector3(right.x, right.y, right.z));
    }                               
	
	if (IS_KEY_DOWN(OIS::KC_LEFT) || IS_KEY_DOWN(OIS::KC_A)) {
		body_->applyCentralForce(40*btVector3(right.x, right.y, right.z));
	}
    

    // there's always a force pushing forward
    body_->applyCentralForce(40*btVector3(forward.x, forward.y, forward.z));

    // and a drag force proportional to the velocity
    float forward_vel = forward.dotProduct(velocity);
    float forward_drag = .03 * forward_vel * forward_vel;
    if (forward_vel > 0) forward_drag *= -1; // make sure force is opposite

    body_->applyCentralForce(btVector3(forward_drag * forward.x,
                                       forward_drag * forward.y,
                                       forward_drag * forward.z));
    float side_vel = right.dotProduct(velocity);
    float side_drag = -2 * side_vel;
    body_->applyCentralForce(btVector3(side_drag * right.x,
                                       side_drag * right.y,
                                       side_drag * right.z));

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
					targets_.push_back(e);
				}
			}
		}
	}
}

void Player::onTargetDelete(ObjectPtr object) {

	for (list<EnemyPtr>::iterator i = targets_.begin(); i != targets_.end();) {
		EnemyPtr enemy = dynamic_pointer_cast<Enemy>(object);
		if ((*i) == enemy) {
			i = targets_.erase(i);
		} else {
			i++;
		}
	}
}

const SpineProjection& Player::getPlayerProjection() const {
    return playerProjection_;
}

const SpineProjection& Player::getSpawnProjection(float distance) const {
	return spawnProjection_;//level_->getTube()->getSpineProjection(playerProjection_.distance + distance, playerProjection_.index);

}

void Player::onCollision(EnemyPtr enemy) { 
	callMethod("onEnemyHit"); 
	if (shields_ > 0) {
		shields_ -= 10;
		if (shields_ <= 0) {
			callMethod("onKilled");
		} 
	}
}

void Player::onCollision(ProjectilePtr projectile) { 
	callMethod("onProjectileHit"); 
	if (shields_ > 0) {
		shields_ -= 10;
		if (shields_ <= 0) {
			callMethod("onKilled");
		} 
	}
}


