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
		dBodySetMass(body_, &mass);
		dBodySetData(body_, node);
		dBodySetMaxAngularSpeed(body_, 0);
		dBodySetMovedCallback(body_, &Impl::onBodyMoved);

		// Set up ODE geom
		geom_ = dCreateSphere(game_->getSpace(), BALLRADIUS);
		dGeomSetBody(geom_, body_);
		dGeomSetCategoryBits(geom_, TYPEBALL);
		dGeomSetCollideBits(geom_, TYPETERRAIN | TYPEROAD);		
	}
}