/******************************************************************************
 * Warp: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Ball.hpp>
#include <Objects.hpp>

using namespace Warp;
using namespace Ogre;
using namespace std;

#define BALLRADIUS 0.5f // meters
#define BALLMASS 1000.0f // kilograms

static bool Callback(btManifoldPoint& cp, const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1)
{
    cp.m_appliedImpulse *= 00.0f;
    cp.m_combinedRestitution = -1.0f;
    cp.m_combinedFriction = 0.0f;
cout << "here" << endl;
    return true;
}

Ogre::Vector3 last = Ogre::Vector3(0.0f, 0.0f, -3.0f);

struct Ball::Impl : public Game::Listener, public btMotionState {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init() {
		// Set up OGRE scene nodes
		node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode("Ball");
		node_->attachObject(game_->getSceneManager()->createEntity("Ball", "Ball.mesh"));

        //gContactAddedCallback = Callback;
        
        position_.setIdentity();
        
        position_.setOrigin(btVector3(0, 0, 40));
        shape_.reset(new btSphereShape(BALLRADIUS));

        btScalar mass(BALLMASS);
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        shape_->calculateLocalInertia(mass, inertia);

        btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, this, shape_.get(), inertia);
        body_.reset(new btRigidBody(rbinfo));
		body_->setFriction(0.0f);
		body_->setRestitution(0.0f);
		body_->setGravity(btVector3(0.0f, -30.0f, 0.0f));
       // body_->setCollisionFlags(body_->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        game_->getWorld()->addRigidBody(body_.get());
        game_->getWorld()->setGravity(btVector3(0, -20, 0));

        front_ = btVector3(0, 0, 0);

	}

    ~Impl() {
        game_->getWorld()->removeCollisionObject(body_.get());
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
    }

	/** Called when a new frame is detected */
	void onTimeStep() {

		btVector3 btforward = body_->getLinearVelocity().normalized();
		btVector3 btposition = body_->getCenterOfMassPosition();
		float speed = body_->getLinearVelocity().length();

		Vector3 forward = speed > 0.01f ? Vector3(btforward.x(), btforward.y(), btforward.z()) : Vector3::UNIT_Z;
		Vector3 up = Vector3::UNIT_Y;
		Vector3 right = up.crossProduct(forward);
		Vector3 position(btposition.x(), btposition.y(), btposition.z());
		position -= forward*3.0f;
        position += Vector3(0, 2, 0);

#define ALPHA 0.90f
		forward = ALPHA * game_->getCamera()->getDirection() + (1-ALPHA) * forward;
		position = ALPHA * game_->getCamera()->getPosition() + (1-ALPHA) * position;

        game_->getCamera()->setDirection(forward);
		game_->getCamera()->setPosition(position);


		if (game_->getKeyboard()->isKeyDown(OIS::KC_RIGHT)) {
			body_->applyCentralForce(-20000*btVector3(right.x, right.y, right.z));
        }
		
		if (game_->getKeyboard()->isKeyDown(OIS::KC_LEFT)) {
			body_->applyCentralForce(20000*btVector3(right.x, right.y, right.z));
		}
        

		if (game_->getKeyboard()->isKeyDown(OIS::KC_UP)) {
			body_->applyCentralForce(20000*btforward);
		}
		if (game_->getKeyboard()->isKeyDown(OIS::KC_DOWN)) {
			body_->applyCentralForce(-20000*btforward);
		}
	}

	Game* game_;
    SceneNode* node_;
    btVector3 front_;
    auto_ptr<btCollisionShape> shape_;
    auto_ptr<btRigidBody> body_;
    btTransform position_;

};

Ball::Ball(Game* game) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->init();
	game->addListener(impl_.get());
}

Ball::~Ball() {
}