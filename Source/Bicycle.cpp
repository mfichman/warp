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
#define WHEELWIDTH  0.5f //0.035f // meters
#define WHEELMASS 1.200f // kilograms
#define FRAMELENGTH 0.996f // meters
#define FRAMEHEIGHT 0.848f // meters
#define FRAMEWIDTH 0.035f // meters
#define FRAMEMASS 10.000f // kilograms


#define RIDERMASS 68.0f
#define RIDERRADIUS 0.30f

//------------------------------------------------------------------------------
Bicycle::Ptr
Bicycle::make(Application* app) {
    return new Bicycle(app);
}


//------------------------------------------------------------------------------
Bicycle::Bicycle(Application* app) :
    app_(app),
    nextAnimation_(animCoast),
    currentAnimation_(animCoast),
    frontWheelBody_(dBodyCreate(app_->world())),
    rearWheelBody_(dBodyCreate(app_->world())),
    frameBody_(dBodyCreate(app_->world())),
    forkBody_(dBodyCreate(app_->world())),
    //frontWheelGeom_(dCreateCylinder(app_->space(), WHEELRADIUS, WHEELWIDTH)),
    frontWheelGeom_(dCreateSphere(app_->space(), WHEELRADIUS)),
    //rearWheelGeom_(dCreateCylinder(app_->space(), WHEELRADIUS, WHEELWIDTH)),
    rearWheelGeom_(dCreateSphere(app_->space(), WHEELRADIUS)),
    frameGeom_(dCreateBox(app_->space(), FRAMELENGTH, FRAMEHEIGHT, FRAMEWIDTH)),
    frontWheelJoint_(dJointCreateHinge(app_->world(), 0)),
    rearWheelJoint_(dJointCreateHinge(app_->world(), 0)),
    forkJoint_(dJointCreateHinge(app_->world(), 0)) {

    // Attach wheels to the body
    dGeomSetBody(frontWheelGeom_, frontWheelBody_);
    dGeomSetCategoryBits(frontWheelGeom_, TYPEWHEEL);
    dGeomSetCollideBits(frontWheelGeom_, TYPETERRAIN);
    
    dGeomSetBody(rearWheelGeom_, rearWheelBody_);
    dGeomSetCategoryBits(rearWheelGeom_, TYPEWHEEL);
    dGeomSetCollideBits(rearWheelGeom_, TYPETERRAIN);
    
    dGeomSetBody(frameGeom_, frameBody_);
    dGeomSetCategoryBits(frameGeom_, TYPEWHEEL);
    dGeomSetCollideBits(frameGeom_, TYPETERRAIN);
    
    // Set the mass of the various objects
    dMass mass;
    dMassSetSphereTotal(&mass, WHEELMASS, WHEELRADIUS);
    dBodySetMass(frontWheelBody_, &mass);
    dBodySetMass(rearWheelBody_, &mass);
    dMassSetBoxTotal(&mass, FRAMEMASS, FRAMELENGTH, FRAMEHEIGHT, FRAMEWIDTH);
    dBodySetMass(frameBody_, &mass);
    dMassSetSphereTotal(&mass, 1.0f, 1.0f);
    dBodySetMass(forkBody_, &mass);
    
    // Set up OGRE scene nodes
    SceneNode* root = app_->sceneManager()->getRootSceneNode();
    frameNode_ = root->createChildSceneNode("Frame");
    frameNode_->attachObject(app_->sceneManager()->createEntity("Frame", "Frame.mesh"));
    frontWheelNode_ = root->createChildSceneNode("FrontWheel");
    frontWheelNode_->attachObject(app_->sceneManager()->createEntity("FrontWheel", "FrontWheel.mesh"));
    rearWheelNode_ = root->createChildSceneNode("RearWheel");
    rearWheelNode_->attachObject(app_->sceneManager()->createEntity("RearWheel", "RearWheel.mesh"));
    forkNode_ = root->createChildSceneNode("Fork");
    forkNode_->attachObject(app_->sceneManager()->createEntity("Fork", "Fork.mesh"));
    
    // Attach ODE bodies to the OGRE scene nodes
    dBodySetData(frontWheelBody_, frontWheelNode_);
    dBodySetMovedCallback(frontWheelBody_, &Bicycle::onBodyNodeMoved);
    dBodySetData(rearWheelBody_, rearWheelNode_);
    dBodySetMovedCallback(rearWheelBody_, &Bicycle::onBodyNodeMoved);
    dBodySetData(frameBody_, frameNode_);
    dBodySetMovedCallback(frameBody_, &Bicycle::onBodyNodeMoved);
    dBodySetData(forkBody_, forkNode_);
    dBodySetMovedCallback(forkBody_, &Bicycle::onBodyNodeMoved);
    
    
    // Set up the wheel joints
    dJointAttach(rearWheelJoint_, rearWheelBody_, frameBody_);
    dBodySetPosition(rearWheelBody_, -0.415, -0.367, 0.000);
    dJointSetHingeAnchor(rearWheelJoint_, -0.415, -0.367, 0.000);
    dJointSetHingeAxis(rearWheelJoint_, 0.0, 0.0, 1.0);
    
    dJointAttach(forkJoint_, forkBody_, frameBody_);
    dBodySetPosition(forkBody_, 0.516, 0.046, 0.000);
    dJointSetHingeAnchor(forkJoint_, 0.516, 0.046, 0.000);
    dJointSetHingeAxis(forkJoint_, -0.036, 0.124, 0.000);
    dJointSetHingeParam(forkJoint_, dParamLoStop, -3.14/4);
    dJointSetHingeParam(forkJoint_, dParamHiStop, 3.14/4);
    
    dJointAttach(frontWheelJoint_, frontWheelBody_, forkBody_);
    dBodySetPosition(frontWheelBody_, 0.693, -0.374, 0.000);
    dJointSetHingeAnchor(frontWheelJoint_, 0.693, -0.374, 0.000);
    dJointSetHingeAxis(frontWheelJoint_, 0.0, 0.0, 1.0);
   
    dWorldSetGravity(app_->world(), 0.0, -9.81, 0.0);    
    
    // Fork motor
    forkMotor_ = dJointCreateAMotor(app_->world(), 0);
    dJointAttach(forkMotor_, forkBody_, frameBody_);
    dJointSetAMotorMode(forkMotor_, dAMotorEuler);
    dJointSetAMotorAxis(forkMotor_, 0, 1, 0.000, 1.000, 0.000);
    dJointSetAMotorAxis(forkMotor_, 2, 2, 1.000, 0.000, 0.000);
    dJointSetAMotorParam(forkMotor_, dParamVel, 0);
    dJointSetAMotorParam(forkMotor_, dParamFMax, 20);
    
    
    // Rider
    riderBody_ = dBodyCreate(app_->world());
    dMassSetSphereTotal(&mass, RIDERMASS, RIDERRADIUS);
    dBodySetMass(riderBody_, &mass);
    dBodySetPosition(riderBody_, 0.000, 0.001, 0.000);
    
    riderJoint_ = dJointCreateHinge(app_->world(), 0);
    dJointAttach(riderJoint_, riderBody_, frameBody_);
    dJointSetHingeAnchor(riderJoint_, 0.000, 0.000, 0.000);
    dJointSetHingeAxis(riderJoint_, 1.000, 0.000, 0.000);
    dJointSetHingeParam(riderJoint_, dParamLoStop, -3.14/32);
    dJointSetHingeParam(riderJoint_, dParamHiStop, 3.14/32);
    
    //riderMotor_ = dJointCreateAMotor(app_->world(), 0);
    //dJointAttach(riderMotor_, riderBody_, frameBody_);
    //dJointSetAMotorMode(riderMotor_, dAMotorEuler);
    //dJointSetAMotorAxis(riderMotor_, 0, 1, 1.000, 0.000, 0.000);
    //dJointSetAMotorAxis(riderMotor_, 2, 2, 0.000, 0.000, 1.000);
    //dJointSetAMotorParam(riderMotor_, dParamVel, 0);
    //dJointSetAMotorParam(riderMotor_, dParamFMax, 2000);
    
    SceneNode* riderNode = root->createChildSceneNode("Rider");
    riderNode->attachObject(app_->sceneManager()->createEntity("Rider", "Sphere.mesh"));
    dBodySetData(riderBody_, riderNode);
    dBodySetMovedCallback(riderBody_, &Bicycle::onBodyNodeMoved);
    
    
    
    dGeomID plane = dCreatePlane(app_->space(), 0.0, 1.0, 0.0, -0.742);
    dGeomSetCategoryBits(plane, TYPETERRAIN);
    dGeomSetCollideBits(plane, TYPEWHEEL);
    
    cout << "BUILT" << endl;
}

//------------------------------------------------------------------------------
Bicycle::~Bicycle() {

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
    if (app_->keyboard()->isKeyDown(OIS::KC_LEFT)) {
    
         //dJointAddHingeTorque(riderJoint_, 2000);
         dJointAddHingeTorque(forkJoint_, 21);
        //dBodyAddRelForce(10, 
    }
    if (app_->keyboard()->isKeyDown(OIS::KC_RIGHT)) {
        //dJointAddHingeTorque(riderJoint_, -2000);
        dJointAddHingeTorque(forkJoint_, -21);
        //dBodyAddRelTorque(frameBody_, 10, 0, 0);
        //dBodyAddRelTorque(frameBody_, 50, 0, 0);
        //dBodyAddRelTorque(forkBody_, 0, -8, 0);
    }
    if (app_->keyboard()->isKeyDown(OIS::KC_UP)) {
        dBodyAddRelTorque(rearWheelBody_, 0, 0, -100);
    }
    if (app_->keyboard()->isKeyDown(OIS::KC_DOWN)) {
        dBodyAddRelTorque(rearWheelBody_, 0, 0, 100);
    }

}
