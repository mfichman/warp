/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Bicycle.hpp>
#include <Application.hpp>

using namespace Criterium;
using namespace Ogre;
using namespace std;

#define FRAMELENGTH 0.996f // meters
#define FRAMEHEIGHT 0.848f // meters
#define FRAMEWIDTH 0.035f // meters
#define FRAMEMASS 80.000f // kilograms

#define WHEELRADIUS 0.368 // meters
#define WHEELBASE 0.1108 // meters

#define CASTERANGLE 0.3988 // radians

//------------------------------------------------------------------------------
Bicycle::Ptr
Bicycle::make(Application* app) {
    return new Bicycle(app);
}


//------------------------------------------------------------------------------
Bicycle::Bicycle(Application* app) :
    app_(app),
    nextAnimation_(animCoast),
	currentAnimation_(animCoast) { 

	initBodies();
	//initGeoms();
}

//------------------------------------------------------------------------------
void
Bicycle::initBodies() {
	dMass mass;
	Entity* entity;

    // Set up OGRE scene nodes
    SceneNode* root = app_->sceneManager()->getRootSceneNode();

	SceneNode* bodyNode = root->createChildSceneNode("Bike");

	centerNode_ = bodyNode->createChildSceneNode("Center");
	centerNode_->setPosition(0.0f, -0.735f, 0.0f);

    SceneNode* frameNode = centerNode_->createChildSceneNode("Frame");
	entity = app_->sceneManager()->createEntity("Frame", "Frame.mesh");
	entity->setCastShadows(true);
    frameNode->attachObject(entity);
	frameNode->setPosition(0, 0.735, 0);

	forkNode_ = centerNode_->createChildSceneNode("Fork");
	entity = app_->sceneManager()->createEntity("Fork", "Fork.mesh");
	entity->setCastShadows(true);
    forkNode_->attachObject(entity);
	forkNode_->setPosition(0.516, 0.781, 0.000);
	
	SceneNode* frontWheelNode = forkNode_->createChildSceneNode("FrontWheel");
	entity = app_->sceneManager()->createEntity("FrontWheel", "Wheel.mesh");
	entity->setCastShadows(true);
    frontWheelNode->attachObject(entity);
	frontWheelNode->setPosition(0.177, -0.420, 0.000);
	
	SceneNode* rearWheelNode = centerNode_->createChildSceneNode("RearWheel");
	entity = app_->sceneManager()->createEntity("RearWheel", "Wheel.mesh");
	entity->setCastShadows(true);
    rearWheelNode->attachObject(entity);
	rearWheelNode->setPosition(-0.415, 0.368, 0.000);
	

	bodyNode->attachObject(app_->camera());
	app_->camera()->setPosition(-1.0, 1.0, -3.0);
	app_->camera()->lookAt(0, 0, 0);


	leanAngle_ = 0;
	turnRadius_ = 0;

	// Set up ODE bodies
	body_ = dBodyCreate(app_->world());
    dMassSetBoxTotal(&mass, FRAMEMASS, FRAMELENGTH, FRAMEHEIGHT, FRAMEWIDTH);
    dBodySetMass(body_, &mass);
	dBodySetData(body_, bodyNode);
	dBodySetMaxAngularSpeed(body_, 0);
    dBodySetMovedCallback(body_, &Bicycle::onBodyMoved);

	frontWheel_ = dBodyCreate(app_->world());
    dMassSetSphereTotal(&mass, 1, WHEELRADIUS);
    dBodySetMass(frontWheel_, &mass);
	dBodySetData(frontWheel_, frontWheelNode);
	dBodySetMovedCallback(frontWheel_, &Bicycle::onWheelMoved);

	rearWheel_ = dBodyCreate(app_->world());
    dMassSetSphereTotal(&mass, 1, WHEELRADIUS);
    dBodySetMass(rearWheel_, &mass);
	dBodySetData(rearWheel_, rearWheelNode);
	dBodySetMovedCallback(rearWheel_, &Bicycle::onWheelMoved);

}

//------------------------------------------------------------------------------
void
Bicycle::initGeoms() {
	
    // Set up ODE geoms
	geom_ = dCreateCapsule(app_->space(), 0.02, 2*0.735 - 0.04);
    dGeomSetBody(geom_, body_);
    dGeomSetCategoryBits(geom_, TYPEWHEEL);
    dGeomSetCollideBits(geom_, TYPETERRAIN);
	
	dMatrix3 rotation;
	dRFromAxisAndAngle(rotation, 1.0, 0.0, 0.0, 3.14159/2);
	dGeomSetOffsetRotation(geom_, rotation);
}

//------------------------------------------------------------------------------
Bicycle::~Bicycle() {
	/*
	dBodyDestroy(frontWheelBody_);
    dBodyDestroy(rearWheelBody_);
    dBodyDestroy(frameBody_);
    dBodyDestroy(forkBody_);
	dBodyDestroy(riderBody_);

    dGeomDestroy(frontWheelGeom_);
    dGeomDestroy(rearWheelGeom_);
    dGeomDestroy(frameGeom_);
    
    dJointDestroy(rearWheelJoint_);
    dJointDestroy(frontWheelJoint_);
    dJointDestroy(forkJoint_);
    dJointDestroy(forkMotor_);
    
    dJointDestroy(riderJoint_);
    dJointDestroy(riderMotor_);*/
}

//------------------------------------------------------------------------------
void
Bicycle::onBodyMoved(dBodyID body) {

    // Update the position of the scene node attached to this body.
    // Position is in global coordinates.
    SceneNode* node = static_cast<SceneNode*>(dBodyGetData(body));
    const dReal* pos = dBodyGetPosition(body);
    const dReal* quat = dBodyGetQuaternion(body);
    
    // N.B.: ODE orders the quat as (w, x, y, z) (so quat[0] = w)
    node->setPosition(pos[0], pos[1], pos[2]);
    node->setOrientation(quat[0], quat[1], quat[2], quat[3]);
}

//------------------------------------------------------------------------------
void
Bicycle::onWheelMoved(dBodyID body) {
    // Update the position of the scene node attached to this body.
    // Position is in global coordinates.
    SceneNode* node = static_cast<SceneNode*>(dBodyGetData(body));
    const dReal* quat = dBodyGetQuaternion(body);
    
    // N.B.: ODE orders the quat as (w, x, y, z) (so quat[0] = w)
    node->setOrientation(quat[0], quat[1], quat[2], quat[3]);
}


//------------------------------------------------------------------------------
void
Bicycle::onFrame() {
	const dReal* velocity = dBodyGetLinearVel(body_);
	float speed = dLENGTH(velocity);

	if (leanAngle_ != 0.0f) {
		// Calculate the turn radius (r = v^2/(g*tan(theta)))
		turnRadius_ = (speed*speed)/(9.81*tanf(leanAngle_));

		// Set the transformation matrix for the lean rotation
		centerNode_->setOrientation(Quaternion(Radian(leanAngle_), Vector3(1.0f, 0.0f, 0.0f)));

		// Get the transformation matrix to point the bike 
		// in the direction that it's moving
		dReal forward[3] = { velocity[0], velocity[1], velocity[2] }; // X AXIS
		dReal up[3] = { 0.0f, 1.0f, 0.0f }; // Y AXIS
		dMatrix3 rotation;
		dRFrom2Axes(rotation, forward[0], forward[1], forward[2], up[0], up[1], up[2]);
		dBodySetRotation(body_, rotation);		

		// Calculate the centripetal force (F = mv^2/r)
		float force =(FRAMEMASS*speed*speed)/turnRadius_;
		dBodyAddRelForce(body_, 0.0, 0.0, force);

		// Calculate the steer angle about the steering axis
		steerAngle_ = -20 * (WHEELBASE*cosf(leanAngle_)) / (turnRadius_*cosf(CASTERANGLE));
		forkNode_->setOrientation(Quaternion(Radian(steerAngle_), Vector3(-0.177, 0.420, 0.0f)));
	}

	// Wheel rotation
	dReal radius[3] = {0.0f, WHEELRADIUS, 0.0f};
	dReal linvel[3] = { speed, 0.0f, 0.0f };
	dReal cross[3];
	dCROSS(cross, =, radius, linvel);
	cross[0] /= WHEELRADIUS*WHEELRADIUS;
	cross[1] /= WHEELRADIUS*WHEELRADIUS;
	cross[2] /= WHEELRADIUS*WHEELRADIUS;
	dBodySetAngularVel(frontWheel_, cross[0], cross[1], cross[2]);
	dBodySetAngularVel(rearWheel_, cross[0], cross[1], cross[2]);
	
	if (app_->keyboard()->isKeyDown(OIS::KC_LEFT)) {
		leanAngle_ -= 0.005;
    }
    if (app_->keyboard()->isKeyDown(OIS::KC_RIGHT)) {
		leanAngle_ += 0.005;
    }
    if (app_->keyboard()->isKeyDown(OIS::KC_UP)) {
        dBodyAddRelForce(body_, 1000, 0, 0);
    }
    if (app_->keyboard()->isKeyDown(OIS::KC_DOWN)) {
        dBodyAddRelForce(body_, -1000, 0, 0);
    }

	if (app_->keyboard()->isKeyDown(OIS::KC_U)) {
		dBodyAddForce(body_, 0, 1000, 0);
	}

}
