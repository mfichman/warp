/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Bicycle.hpp>
#include <Application.hpp>

using namespace Criterium;
using namespace Ogre;
using namespace std;

#define WHEELRADIUS 0.368f // meters
#define WHEELWIDTH  0.08f //0.035f // meters
#define WHEELMASS 1.200f // kilograms
#define FRAMELENGTH 0.996f // meters
#define FRAMEHEIGHT 0.848f // meters
#define FRAMEWIDTH 0.035f // meters
#define FRAMEMASS 10.000f // kilograms


#define RIDERMASS 200.0f
#define RIDERRADIUS 0.01f

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
	initGeoms();
	initJoints();
}

//------------------------------------------------------------------------------
void
Bicycle::initBodies() {
	dMass mass;
	Entity* entity;

    // Set up OGRE scene nodes
    SceneNode* root = app_->sceneManager()->getRootSceneNode();
    SceneNode* frameNode = root->createChildSceneNode("Frame");
	entity = app_->sceneManager()->createEntity("Frame", "Frame.mesh");
	entity->setCastShadows(true);
    frameNode->attachObject(entity);
	//frameNode->setScale(100, 100, 100);
	
	SceneNode* frontWheelNode = root->createChildSceneNode("FrontWheel");
	entity = app_->sceneManager()->createEntity("FrontWheel", "Wheel.mesh");
	entity->setCastShadows(true);
    frontWheelNode->attachObject(entity);
	
	SceneNode* rearWheelNode = root->createChildSceneNode("RearWheel");
	entity = app_->sceneManager()->createEntity("RearWheel", "Wheel.mesh");
	entity->setCastShadows(true);
    rearWheelNode->attachObject(entity);
	
	SceneNode* forkNode = root->createChildSceneNode("Fork");
	entity = app_->sceneManager()->createEntity("Fork", "Fork.mesh");
	entity->setCastShadows(true);
    forkNode->attachObject(entity);

	frameNode->attachObject(app_->camera());
	app_->camera()->setPosition(-5.0, 1.0, 0.0);
	app_->camera()->lookAt(0, 0, 0);


	// Set up ODE bodies
	frontWheelBody_ = dBodyCreate(app_->world());
    dMassSetSphereTotal(&mass, WHEELMASS, WHEELRADIUS);
    dBodySetMass(frontWheelBody_, &mass);
	dBodySetData(frontWheelBody_, frontWheelNode);
    dBodySetMovedCallback(frontWheelBody_, &Bicycle::onBodyNodeMoved);
	dBodySetPosition(frontWheelBody_, 0.693, -0.374, 0.000);

	rearWheelBody_ = dBodyCreate(app_->world());
	dMassSetSphereTotal(&mass, WHEELMASS, WHEELRADIUS);
    dBodySetMass(rearWheelBody_, &mass);
	dBodySetData(rearWheelBody_, rearWheelNode);
    dBodySetMovedCallback(rearWheelBody_, &Bicycle::onBodyNodeMoved);
	dBodySetPosition(rearWheelBody_, -0.415, -0.367, 0.000);

	frameBody_ = dBodyCreate(app_->world());
    dMassSetBoxTotal(&mass, FRAMEMASS, FRAMELENGTH, FRAMEHEIGHT, FRAMEWIDTH);
    dBodySetMass(frameBody_, &mass);
	dBodySetData(frameBody_, frameNode);
    dBodySetMovedCallback(frameBody_, &Bicycle::onBodyNodeMoved);

	forkBody_ = dBodyCreate(app_->world());
    dMassSetSphereTotal(&mass, 1.0f, 1.0f);
    dBodySetMass(forkBody_, &mass);
	dBodySetData(forkBody_, forkNode);
    dBodySetMovedCallback(forkBody_, &Bicycle::onBodyNodeMoved);
	dBodySetPosition(forkBody_, 0.516, 0.046, 0.000);
}

//------------------------------------------------------------------------------
void
Bicycle::initGeoms() {
	
    // Set up ODE geoms
	frontWheelGeom_ = dCreateCylinder(app_->space(), WHEELRADIUS, 0.02);
    dGeomSetBody(frontWheelGeom_, frontWheelBody_);
    dGeomSetCategoryBits(frontWheelGeom_, TYPEWHEEL);
    dGeomSetCollideBits(frontWheelGeom_, TYPETERRAIN);
    
	rearWheelGeom_ = dCreateCylinder(app_->space(), WHEELRADIUS, 0.04);
	dGeomSetBody(rearWheelGeom_, rearWheelBody_);
    dGeomSetCategoryBits(rearWheelGeom_, TYPEWHEEL);
    dGeomSetCollideBits(rearWheelGeom_, TYPETERRAIN);
    
	frameGeom_ = dCreateBox(app_->space(), FRAMELENGTH, FRAMEHEIGHT, FRAMEWIDTH);
    dGeomSetBody(frameGeom_, frameBody_);
    dGeomSetCategoryBits(frameGeom_, 0);
    dGeomSetCollideBits(frameGeom_, 0);
}

//------------------------------------------------------------------------------
void
Bicycle::initJoints() {
  
    // Set up ODE joints 
	frontWheelJoint_ = dJointCreateHinge(app_->world(), 0);
    dJointAttach(frontWheelJoint_, frontWheelBody_, forkBody_);
    dJointSetHingeAnchor(frontWheelJoint_, 0.693, -0.374, 0.000);
    dJointSetHingeAxis(frontWheelJoint_, 0.0, 0.0, 1.0);

	rearWheelJoint_ = dJointCreateHinge(app_->world(), 0);
    dJointAttach(rearWheelJoint_, rearWheelBody_, frameBody_);
    dJointSetHingeAnchor(rearWheelJoint_, -0.415, -0.367, 0.000);
    dJointSetHingeAxis(rearWheelJoint_, 0.0, 0.0, 1.0);
    
	forkJoint_ = dJointCreateHinge(app_->world(), 0);
    dJointAttach(forkJoint_, forkBody_, frameBody_);
    dJointSetHingeAnchor(forkJoint_, 0.516, 0.046, 0.000);
    dJointSetHingeAxis(forkJoint_, -0.036, 0.124, 0.000);
    dJointSetHingeParam(forkJoint_, dParamLoStop, -3.14/4);
    dJointSetHingeParam(forkJoint_, dParamHiStop, 3.14/4); 
	dJointSetHingeParam(forkJoint_, dParamStopERP, 0.00001);
	dJointSetHingeParam(forkJoint_, dParamStopCFM, 0.00001);
}

//------------------------------------------------------------------------------
void
Bicycle::initMotors() {

	leanMotor_ = dJointCreateAMotor(app_->world(), 0);
	dJointAttach(leanMotor_, frameBody_, 0);
	dJointSetHingeAnchor(leanMotor_, 0.000, 0.000, 0.000);
	dJointSetHingeAxis(leanMotor_, 1.000, 0.000, 0.000);
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
Bicycle::onBodyNodeMoved(dBodyID body) {

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
Bicycle::onFrame() {
	//dQuaternion rotation;
	//dQuaternion pure;
	//dQuaternion temp;

	//const dReal* quat = dBodyGetQuaternion(frameBody_);
	//dReal pure[4] = {0.0f, 1.0f, 0.0f, 0.0f};
	//dReal temp[4];

	//dQMultiply0(temp, quat, pure);
	//dQMultiply2(pure, temp, quat);

	//dMatrix3 rotation;
	//dRFromAxisAndAngle(rotation, pure[1], pure[2], pure[3], -0.4);
	//dBodySetRotation(frameBody_, rotation);

	//dQMultiply0(temp)


	//const dReal* velocity = dBodyGetLinearVel(frameBody_);
    //float speed = dLENGTH(velocity);

	//float leanAngle_ = atan2f(speed*speed, 9.81*turnRadius_);

	//dMatrix3 rotation;
	//dRFromAxisAndAngle(rotation, velocity[0], velocity[1], velocity[2], 0.4);
	//dBodySetRotation(frameBody_, rotation);
	//dBodySetAngularVel(frameBody_, 0, 0, 0);

	
	if (app_->keyboard()->isKeyDown(OIS::KC_LEFT)) {
         //dJointAddHingeTorque(forkJoint_, 10);
         //dJointAddSliderForce(riderJoint_, -2000);
    }
    if (app_->keyboard()->isKeyDown(OIS::KC_RIGHT)) {
        //dJointAddHingeTorque(forkJoint_, -10);
        //dJointAddSliderForce(riderJoint_, 2000);
    }
    if (app_->keyboard()->isKeyDown(OIS::KC_UP)) {
        dBodyAddRelTorque(rearWheelBody_, 0, 0, -100);
    }
    if (app_->keyboard()->isKeyDown(OIS::KC_DOWN)) {
        dBodyAddRelTorque(rearWheelBody_, 0, 0, 100);
    }

}
