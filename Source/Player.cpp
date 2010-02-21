/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Player.hpp"
#include "Level.hpp"
#include "DynamicTube.hpp"

using namespace Warp;
using namespace Ogre;
using namespace std;

#define BALL_RADIUS 0.5f // meters
#define BALL_MASS 1.0f // kilograms

#define POSITION_SMOOTHNESS 0.60f
#define ROTATION_SMOOTHNESS 0.05f


struct Player::Impl : public Game::Listener, public btMotionState {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
    Impl(Game* game, const string& name) :
        game_(game),
        name_(name),
        position_(Vector3::ZERO),
        spineNode_i_(0)
    {
        // initialize spine node (kill later)
        spineNode_.position = Vector3::ZERO;
        spineNode_.forward = -Vector3::UNIT_Z;
        spineNode_.index = 0;

		// Set up OGRE scene nodes
		node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name_);
		node_->attachObject(game_->getSceneManager()->createEntity(name_, "Ball.mesh"));
        
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
		//body_->setPosition(btVector(0, 5, 0));
		//body_->setGravity(btVector3(0.0f, -30.0f, 0.0f));
       // body_->setCollisionFlags(body_->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        game_->getWorld()->addRigidBody(body_.get());
        //game_->getWorld()->setGravity(btVector3(0, -20, 0));

        front_ = btVector3(0, 0, 0);
        game_->addListener(this);

	}

    ~Impl() {
        game_->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name_);
        game_->getSceneManager()->destroyEntity(name_);
        game_->getWorld()->removeCollisionObject(body_.get());
        game_->removeListener(this);
    }

    /** Called by bullet to get the transform state */
    void getWorldTransform(btTransform& transform) const {
        transform = transform_;
    }

    /** Called by Bullet to update the scene node */
    void setWorldTransform(const btTransform& transform) {
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
	void onTimeStep() {

		btVector3 btposition = body_->getCenterOfMassPosition();
		btVector3 btvelocity = body_->getLinearVelocity();
		Vector3 position(btposition.x(), btposition.y(), btposition.z());
		Vector3 velocity(btvelocity.x(), btvelocity.y(), btvelocity.z());

        SpineProjection projection = game_->getLevel()->getTube()->getSpineProjection(position, spineNode_i_);
        
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

		if (distance > 7.5f) {
			float v = velocity.dotProduct(up);
			Vector3 force = ((distance - 8.0f) * 40.0f * BALL_MASS + 20.f * BALL_MASS - 10.0f * v) * up;
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
        game_->getCamera()->setOrientation(Quaternion::Slerp(ROTATION_SMOOTHNESS, game_->getCamera()->getOrientation(), Quaternion(-right, up, -forward), true));
		
		position = POSITION_SMOOTHNESS * game_->getCamera()->getPosition() + (1-POSITION_SMOOTHNESS) * position;
		game_->getCamera()->setPosition(position - forward*0.5 + up*0.25);

	}

	Game* game_;
    string name_;
    SceneNode* node_;
    btVector3 front_;
    auto_ptr<btCollisionShape> shape_;
    auto_ptr<btRigidBody> body_;
    /* information about position and orientation from bullet */
    btTransform transform_;
    Vector3 position_;
    SpineNode spineNode_;

    int spineNode_i_;

};

Player::Player(Game* game, const string& name) : impl_(new Impl(game, name)) {
}

Player::~Player() {
}

const Vector3& Player::getPosition() const {
    return impl_->position_;
}

int Player::getSpineNode() const {
    return impl_->spineNode_i_;
}
