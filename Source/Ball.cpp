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

Ogre::Vector3 last = Ogre::Vector3(0.0f, 0.0f, -3.0f);

struct Ball::Impl : public Game::Listener, public btMotionState {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init() {
		// Set up OGRE scene nodes
		node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode("Ball");
		node_->attachObject(game_->getSceneManager()->createEntity("Ball", "Ball.mesh"));

        
        position_.setIdentity();
        shape_.reset(new btSphereShape(BALLRADIUS));

        btScalar mass(BALLMASS);
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        shape_->calculateLocalInertia(mass, inertia);

        btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, this, shape_.get(), inertia);
        body_.reset(new btRigidBody(rbinfo));

        game_->getWorld()->addRigidBody(body_.get());

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

        cout << node_->getPosition() << endl;
    }

	/** Called when a new frame is detected */
	void onTimeStep() {

		
	}

	Game* game_;
    SceneNode* node_;
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