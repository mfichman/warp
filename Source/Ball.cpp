/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Ball.hpp>

using namespace Criterium;
using namespace Ogre;
using namespace std;

#define BALLRADIUS 0.5f // meters
#define BALLMASS 1.0f // kilograms

struct Ball::Impl : public Game::Listener {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init() {

		dMass mass;
		dMassSetSphereTotal(&mass, BALLMASS, BALLRADIUS);

		// Set up OGRE scene nodes
		SceneNode* root = game_->getSceneManager()->getRootSceneNode();
		SceneNode* node = root->createChildSceneNode("Ball");
		node->attachObject(game_->getSceneManager()->createEntity("Ball", "Ball.mesh"));

		// Set up ODE bodies
		body_ = dBodyCreate(game_->getWorld());

		//node->attachObject(game_->getCamera());
		//game_->getCamera()->setPosition(-3.0, 1.0, 0.0);
		//game_->getCamera()->lookAt(0, 0, 0);
		
		dBodySetMass(body_, &mass);
		dBodySetData(body_, node);
		dBodySetMaxAngularSpeed(body_, 0);
		dBodySetMovedCallback(body_, &Impl::onBodyMoved);

		// Set up ODE geom
		geom_ = dCreateSphere(game_->getSpace(), BALLRADIUS);
		dGeomSetBody(geom_, body_);
		dGeomSetCategoryBits(geom_, TYPEBALL);
		dGeomSetCollideBits(geom_, TYPETERRAIN | TYPEROAD);

		dBodySetPosition(body_, 629, 75, 546);
		
	}

	Ogre::Vector3 last;

	/** Called when a new frame is detected */
	void onTimeStep() {

		const dReal* vel = dBodyGetLinearVel(body_);
		Ogre::Vector3 front(vel[0], vel[1], vel[2]);
		

		float alpha = 0.1f;
		float alphay = 0.1f;
		Vector3 front2;
		front2.x = alpha*front.x + (1-alpha)*last.x;
		front2.y = alphay*front.y + (1-alphay)*last.y;
		front2.z = alpha*front.z + (1-alpha)*last.z;
		last = front2;

		front.normalise();
		front2.normalise();

		const dReal* pos = dBodyGetPosition(body_);
		Ogre::Vector3 p(pos[0], pos[1], pos[2]);



		Ogre::Vector3 newpos = -6*front2 + p + Ogre::Vector3(0, 2, 0);
		Ogre::Vector3 newlook = 6*front2 + p;

		float alpha2 = 0.5;
		newpos = (alpha2)*newpos + (1-alpha2)*game_->getCamera()->getPosition();


		game_->getCamera()->lookAt(newlook);
		game_->getCamera()->setPosition(newpos);

		Ogre::Vector3 right = front.crossProduct(Ogre::Vector3(0, 1, 0));
		Ogre::Vector3 left = front.crossProduct(Ogre::Vector3(0, -1, 0));

		right.normalise();
		left.normalise();

		right *= 20;
		left *= 20;
		front *= 20;


		if (game_->getKeyboard()->isKeyDown(OIS::KC_RETURN)) {
			//dBodySetPosition(body_, game_->getCamera()->getPosition().x, game_->getCamera()->getPosition().y,  game_->getCamera()->getPosition().z);
			//dBodySetPosition(body_, 629, 75, 546);  


			dBodySetPosition(body_, 89.635, 51, 178.7);

			dBodySetLinearVel(body_, 0.0f, 0.0f, 0.0f);
			dBodySetAngularVel(body_, 0.0f, 0.0f, 0.0f);

			/*
			const Ogre::Vector3& p = game_->getCamera()->getPosition();
			dBodySetPosition(body_, p.x, p.y, p.z);
			dBodySetLinearVel(body_, 0.0f, 0.0f, 0.0f);
			dBodySetAngularVel(body_, 0.0f, 0.0f, 0.0f);*/
		}

		float speed2 = vel[0]*vel[0] + vel[1]*vel[1] + vel[2]*vel[2];


		dBodyAddForce(body_, -front.x*speed2*0.0005, -front.y*speed2*0.0005, -front.z*speed2*0.0005);


		if (game_->getKeyboard()->isKeyDown(OIS::KC_UP)) {
			dBodyAddForce(body_, front.x, front.y, front.z);
		}
		if (game_->getKeyboard()->isKeyDown(OIS::KC_DOWN)) {
			dBodyAddForce(body_, -front.x, -front.y, -front.z);
		}

		if (game_->getKeyboard()->isKeyDown(OIS::KC_LEFT)) {
			dBodyAddForce(body_, left.x, left.y, left.z);
		}
		if (game_->getKeyboard()->isKeyDown(OIS::KC_RIGHT)) {
			dBodyAddForce(body_, right.x, right.y, right.z);
		}
	}

	
	/** Called when the body is moved to move the corresponding scene node */
	static void onBodyMoved(dBodyID body) {

		// Update the position of the scene node attached to this body.
		// Position is in global coordinates.
		SceneNode* node = static_cast<SceneNode*>(dBodyGetData(body));
		const dReal* pos = dBodyGetPosition(body);
		const dReal* quat = dBodyGetQuaternion(body);
	    
		// N.B.: ODE orders the quat as (w, x, y, z) (so quat[0] = w)
		node->setPosition(pos[0], pos[1], pos[2]);
		node->setOrientation(quat[0], quat[1], quat[2], quat[3]);
	}

	Game* game_;
    dBodyID body_;
    dGeomID geom_;
};

Ball::Ball(Game* game) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->init();
	game->addListener(impl_.get());
}
