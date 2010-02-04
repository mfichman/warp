/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Cylinder.hpp>

using namespace Criterium;
using namespace Ogre;
using namespace std;

#define CYLINDERRADIUS 0.5f // meters
#define CYLINDERLENGTH 1.0f // meters
#define CYLINDERMASS 100.0f // kilograms

struct Cylinder::Impl : public Game::Listener {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init() {

		dMass mass;
		dMassSetCylinderTotal(&mass, CYLINDERMASS, 3, CYLINDERRADIUS, CYLINDERLENGTH);

		// Set up OGRE scene nodes
		SceneNode* root = game_->getSceneManager()->getRootSceneNode();
		SceneNode* node = root->createChildSceneNode("Cylinder");
		node->attachObject(game_->getSceneManager()->createEntity("Cylinder", "Cylinder.mesh"));

		// Set up ODE bodies
		body_ = dBodyCreate(game_->getWorld());
		
		dBodySetMass(body_, &mass);
		dBodySetData(body_, node);
		dBodySetMovedCallback(body_, &Impl::onBodyMoved);

		// Set up ODE geom
		geom_ = dCreateCylinder(game_->getSpace(), CYLINDERRADIUS, CYLINDERLENGTH);
		dGeomSetBody(geom_, body_);
		dGeomSetCategoryBits(geom_, TYPEBALL);
		dGeomSetCollideBits(geom_, TYPETERRAIN | TYPEROAD);
		dGeomSetData(geom_, &object_);

		dBodySetPosition(body_, 0, 10, 0);
		
	}

	/** Called when a new frame is detected */
	void onTimeStep() {

		if (game_->getKeyboard()->isKeyDown(OIS::KC_PGUP)) {
			dBodyAddRelTorque(body_, 0, 0, 100);
		}
		if (game_->getKeyboard()->isKeyDown(OIS::KC_PGDOWN)) {
			dBodyAddRelTorque(body_, 0, 0, -100);
		}

		Ogre::Vector3 n = 10 * object_.normal;
		if (game_->getKeyboard()->isKeyDown(OIS::KC_LEFT)) {
			dBodyAddTorque(body_, n.x, n.y, n.z);
		}
		if (game_->getKeyboard()->isKeyDown(OIS::KC_RIGHT)) {
			dBodyAddTorque(body_, -n.x, -n.y, -n.z);
		}

		if (game_->getKeyboard()->isKeyDown(OIS::KC_RETURN)) {
			const Ogre::Vector3& p = game_->getCamera()->getPosition();
			dBodySetPosition(body_, p.x, p.y, p.z);
			dBodySetLinearVel(body_, 0.0f, 0.0f, 0.0f);
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
	Game::Object object_;
};

Cylinder::Cylinder(Game* game) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->init();
	game->addListener(impl_.get());
}
