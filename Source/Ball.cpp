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
#define BALLMASS 1.0f // kilograms

#define ALPHA 0.1f 
#define BETA 0.02f
#define GAMMA 0.5

Ogre::Vector3 last = Ogre::Vector3(0.0f, 0.0f, -3.0f);

struct Ball::Impl : public Game::Listener, public btMotionState {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init() {
		// Set up OGRE scene nodes
		node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode("Ball");
		node_->attachObject(game_->getSceneManager()->createEntity("Ball", "Ball.mesh"));

        
        position_.setIdentity();
        
        position_.setOrigin(btVector3(0, 0, 40));
        shape_.reset(new btSphereShape(BALLRADIUS));

        btScalar mass(BALLMASS);
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        shape_->calculateLocalInertia(mass, inertia);

        btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, this, shape_.get(), inertia);
        body_.reset(new btRigidBody(rbinfo));

        game_->getWorld()->addRigidBody(body_.get());

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
        /*
        float speed = body_->getLinearVelocity().length();
        btVector3 d = body_->getLinearVelocity().normalized();
       

        if (speed > 0.01) {

            // Smooth the front vector
            front_ = btVector3(ALPHA*d.x() + (1-ALPHA)*front_.x(), BETA*d.y() + (1-BETA)*front_.y(), ALPHA*d.z() + (1-ALPHA)*front_.z());
            front_.normalize();

            // Calculate the position of the camera
            btVector3 p = body_->getCenterOfMassPosition();
            btVector3 position = -6 * d + p + btVector3(0.0f, 2.0f, 0.0f);
            btVector3 look = 6 * d + p;
            Vector3 oldpos = game_->getCamera()->getPosition();
            position = (GAMMA)*position + (1-GAMMA)*btVector3(oldpos.x, oldpos.y, oldpos.z);
            game_->getCamera()->setPosition(position.x(), position.y(), position.z());
            game_->getCamera()->lookAt(look.x(), look.y(), look.z());

            // Reset the position of the object if ENTER is hit
            // Hack hack hack
            if (game_->getKeyboard()->isKeyDown(OIS::KC_RETURN)) {
                body_->setCenterOfMassTransform(btTransform(btQuaternion::getIdentity(), btVector3(0, 0, 0)));
                body_->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
                front_ = btVector3(0, 0, 1);
            }

            // Add forces for steering and acceleration
            if (game_->getKeyboard()->isKeyDown(OIS::KC_UP)) {
                body_->applyCentralForce(d * 10);
	        }
	        if (game_->getKeyboard()->isKeyDown(OIS::KC_DOWN)) {
                body_->applyCentralForce(-d * 10);
	        }
            

            float turnForce = game_->getMouseNormalizedX() * 2;
	        body_->applyCentralForce(turnForce * d.cross(btVector3(0, 1, 0)));
        }
        */
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