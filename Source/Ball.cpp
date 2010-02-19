/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include <Ball.hpp>
#include <Objects.hpp>

using namespace Warp;
using namespace Ogre;
using namespace std;

#define BALLRADIUS 0.5f // meters
#define BALLMASS 1.0f // kilograms


struct Ball::Impl : public Game::Listener, public btMotionState {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
    Impl(Game* game, const string& name) :
        game_(game),
        name_(name) {

		// Set up OGRE scene nodes
		node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name_);
		node_->attachObject(game_->getSceneManager()->createEntity(name_, "Ball.mesh"));
        
        position_.setIdentity();
        
        position_.setOrigin(btVector3(0, -5, 5));
        shape_.reset(new btSphereShape(BALLRADIUS));

        btScalar mass(BALLMASS);
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
        transform = position_;
    }

    /** Called by Bullet to update the scene node */
    void setWorldTransform(const btTransform& transform) {
        const btQuaternion& rotation = transform.getRotation();
        node_->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
        const btVector3& position = transform.getOrigin();
        node_->setPosition(position.x(), position.y(), position.z());
        position_ = transform;
        game_->setPlayerPosition(Vector3(position.x(), position.y(), position.z()));
    }

	/** Called when a new frame is detected */
	void onTimeStep() {

		btVector3 btposition = body_->getCenterOfMassPosition();
		btVector3 btvelocity = body_->getLinearVelocity();
		Vector3 position(btposition.x(), btposition.y(), btposition.z());
		Vector3 velocity(btvelocity.x(), btvelocity.y(), btvelocity.z());

		const SpineNode& node = game_->getSpineNode();
        
        //float speed = body_->getLinearVelocity().length();
		//Vector3 forward = speed > 0.01f ? Vector3(btforward.x(), btforward.y(), btforward.z()) : Vector3::UNIT_Z;

        assert(node.forward != Vector3::ZERO);
        assert(node.position - position != Vector3::ZERO);

        // Up points toward spine node
        Vector3 forward = node.forward;
		Vector3 up = (node.position - position).normalisedCopy();//Vector3::UNIT_Y;
		
		// Need to make sure up is orthogonal to forward
        assert(up.crossProduct(forward) != Vector3::ZERO);

		
        // Project the gravity vector into the plane with "forward" as the
        // normal vector.  This forces the ball to the outside of the ring, and
        // removes and component that would make the ball move forward/backward
        up = up - (up.dotProduct(node.forward)) * node.forward;

        assert(up != Vector3::ZERO);

        up.normalise();

        Vector3 right = up.crossProduct(forward);

        // Apply gravity and hover forces:
		// We want the ship to hover at 2.0 meters above the ground
		float distance = (node.position - position).length();

		Vector3 gravity = -20.0f * BALLMASS * up;
		body_->applyCentralForce(btVector3(gravity.x, gravity.y, gravity.z));

		if (distance > 7.5f) {
			float v = velocity.dotProduct(up);
			Vector3 force = ((distance - 8.0f) * 40.0f * BALLMASS + 20.f * BALLMASS - 10.0f * v) * up;
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

        // set the camera
#define ALPHA 0.80f
#define BETA 0.05f
        //cout << "forward: " << forward << endl;
        //cout << "up: " << up << endl;
        //cout << "right: " << right << endl;

		//forward = ALPHA * game_->getCamera()->getDirection() + (1-ALPHA) * forward;
		position = ALPHA * game_->getCamera()->getPosition() + (1-ALPHA) * position;


        right.normalise();
        up.normalise();
        forward.normalise();
        
        game_->getCamera()->setOrientation(Quaternion::Slerp(BETA, game_->getCamera()->getOrientation(), Quaternion(-right, up, -forward), true));
        //game_->getCamera()->setOrientation(Quaternion(Vector3::UNIT_X, -Vector3::UNIT_Y, Vector3::UNIT_Z));
        //game_->getCamera()->setDirection(forward);

        //game_->getCamera()->lookAt(position);
		game_->getCamera()->setPosition(position - forward*0.5 + up*0.25);

	}

	Game* game_;
    string name_;
    SceneNode* node_;
    btVector3 front_;
    auto_ptr<btCollisionShape> shape_;
    auto_ptr<btRigidBody> body_;
    btTransform position_;

};

Ball::Ball(Game* game, const string& name) : impl_(new Impl(game, name)) {
}

Ball::~Ball() {
}
