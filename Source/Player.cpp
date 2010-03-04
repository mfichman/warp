/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Player.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Object.hpp"
#include "DynamicTube.hpp"

#include <OIS/OIS.h>

using namespace Warp;
using namespace Ogre;
using namespace std;

#define BALL_RADIUS 0.5f // meters
#define BALL_MASS 1.0f // kilograms

#define POSITION_SMOOTHNESS 0.60f
#define ROTATION_SMOOTHNESS 0.05f

#define SPAWN_DISTANCE 150.0f

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Player::Player(Game* game, Level* level, const string& name) :
    game_(game),
	level_(level),
    name_(name),
    position_(Vector3::ZERO)
{
	// Set up OGRE scene nodes
	node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name_);
	shipNode_ = node_->createChildSceneNode(name_ + ".ShipNode");
	shipNode_->attachObject(game_->getSceneManager()->createEntity(name_, "Dagger.mesh"));
    
    transform_.setIdentity();
    
    transform_.setOrigin(btVector3(0, -5, 5));
    shape_.reset(new btSphereShape(BALL_RADIUS));

    btScalar mass(BALL_MASS);
    btVector3 inertia(0.0f, 0.0f, 0.0f);
    shape_->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, this, shape_.get(), inertia);
    body_.reset(new btRigidBody(rbinfo));
	body_->setFriction(0.0f);
	body_->setRestitution(0.0f);

    game_->getWorld()->addRigidBody(body_.get());
    game_->addListener(this);

}

Player::~Player() {
	node_->removeAndDestroyChild(shipNode_->getName());
    game_->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name_);
    game_->getSceneManager()->destroyEntity(name_);
    game_->getWorld()->removeCollisionObject(body_.get());
    game_->removeListener(this);
}

/** Called by bullet to get the transform state */
void Player::getWorldTransform(btTransform& transform) const {
    transform = transform_;
}

/** Called by Bullet to update the scene node */
void Player::setWorldTransform(const btTransform& transform) {
    // get info from bullet
    const btQuaternion& rotation = transform.getRotation();
    const btVector3& position = transform.getOrigin();
    // apply to scene node
    node_->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    node_->setPosition(position.x(), position.y(), position.z());
    // set local info
    transform_ = transform;
    position_ = Vector3(position.x(), position.y(), position.z());
}

/** Called when a new frame is detected */
void Player::onTimeStep() {
	computeForces();
	updateRay();
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

	Vector3 gravity = -20.0f * BALL_MASS * up;
	body_->applyCentralForce(btVector3(gravity.x, gravity.y, gravity.z));

	if (distance > 5.5f) {
		float v = velocity.dotProduct(up);
		Vector3 force = ((distance - 6.0f) * 40.0f * BALL_MASS + 20.f * BALL_MASS - 10.0f * v) * up;
		body_->applyCentralForce(btVector3(force.x, force.y, force.z));
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


    right.normalise();
    up.normalise();
    forward.normalise();   
	shipNode_->setOrientation(Quaternion(-right, up, -forward));
    game_->getCamera()->setOrientation(Quaternion::Slerp(ROTATION_SMOOTHNESS, game_->getCamera()->getOrientation(), Quaternion(-right, up, -forward), true));
	
	position = POSITION_SMOOTHNESS * game_->getCamera()->getPosition() + (1-POSITION_SMOOTHNESS) * position;
	game_->getCamera()->setPosition(position - forward*2.0 + up*0.5);
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
				Object* obj = static_cast<Object*>(body->getUserPointer());
				if (obj) {
					obj->select();
				}
			}
		}
	}
}

const Vector3& Player::getPosition() const {
    return position_;
}

const SpineProjection& Player::getPlayerProjection() const {
    return playerProjection_;
}

const SpineProjection& Player::getSpawnProjection() const {
	return spawnProjection_;
}


