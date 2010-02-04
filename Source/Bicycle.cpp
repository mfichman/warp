/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Bicycle.hpp>

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
#define MAXLEANANGLE 0.8000 // radians
#define SENSITIVITY 0.1000 

bool enable = false;

struct Bicycle::Impl : public Game::Listener {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void initNodes() {

		dMass mass; Entity* entity;

		// Set up OGRE scene nodes
		SceneNode* root = game_->getSceneManager()->getRootSceneNode();
		SceneNode* bodyNode = root->createChildSceneNode("Bike");

		centerNode_ = bodyNode->createChildSceneNode("Center");
		centerNode_->setPosition(0.0f, -0.735f, 0.0f);

		// Bike frame
		SceneNode* frameNode = centerNode_->createChildSceneNode("Frame");
		entity = game_->getSceneManager()->createEntity("Frame", "Frame.mesh");
		entity->setCastShadows(true);
		frameNode->attachObject(entity);
		frameNode->setPosition(0, 0.735, 0);

		// Front bike fork
		forkNode_ = centerNode_->createChildSceneNode("Fork");
		entity = game_->getSceneManager()->createEntity("Fork", "Fork.mesh");
		entity->setCastShadows(true);
		forkNode_->attachObject(entity);
		forkNode_->setPosition(0.516, 0.781, 0.000);
		
		// Front wheel
		SceneNode* frontWheelNode = forkNode_->createChildSceneNode("FrontWheel");
		entity = game_->getSceneManager()->createEntity("FrontWheel", "Wheel.mesh");
		entity->setCastShadows(true);
		frontWheelNode->attachObject(entity);
		frontWheelNode->setPosition(0.177, -0.420, 0.000);
		
		// Rear wheel
		SceneNode* rearWheelNode = centerNode_->createChildSceneNode("RearWheel");
		entity = game_->getSceneManager()->createEntity("RearWheel", "Wheel.mesh");
		entity->setCastShadows(true);
		rearWheelNode->attachObject(entity);
		rearWheelNode->setPosition(-0.415, 0.368, 0.000);

		// Cyclist and animation state
		/*SceneNode* cyclistNode = centerNode_->createChildSceneNode("Cyclist");
		entity = game_->getSceneManager()->createEntity("Shirt", "Cyclist.mesh");
		entity->setCastShadows(true);
		cyclistNode->attachObject(entity);
		cyclistNode->setPosition(-0.2600, 1.020, 0.000);
		cyclistNode->setOrientation(Quaternion(Degree(90), Vector3(0.000, 1.000, 0.000)));
		cyclingState_ = entity->getAnimationState("Cycling");
		cyclingState_->setEnabled(true);
		cyclingState_->setLoop(true);*/
		
		bodyNode->attachObject(game_->getCamera());
		game_->getCamera()->setPosition(-3.0, 1.0, 0.0);
		game_->getCamera()->lookAt(0, 0, 0);

		leanAngle_ = 0;
		turnRadius_ = 0;

		// Set up ODE bodies
		body_ = dBodyCreate(game_->getWorld());
		dMassSetBoxTotal(&mass, FRAMEMASS, FRAMELENGTH, FRAMEHEIGHT, FRAMEWIDTH);
		dBodySetMass(body_, &mass);
		dBodySetData(body_, bodyNode);
		//dBodySetMaxAngularSpeed(body_, 0);
		dBodySetMovedCallback(body_, &Impl::onBodyMoved);
		dBodySetLinearVel(body_, 5.0f, 0.0f, 0.0f);

		frontWheel_ = dBodyCreate(game_->getWorld());
		dMassSetSphereTotal(&mass, 1, WHEELRADIUS);
		dBodySetMass(frontWheel_, &mass);
		dBodySetData(frontWheel_, frontWheelNode);
		dBodySetMovedCallback(frontWheel_, &Impl::onWheelMoved);

		rearWheel_ = dBodyCreate(game_->getWorld());
		dMassSetSphereTotal(&mass, 1, WHEELRADIUS);
		dBodySetMass(rearWheel_, &mass);
		dBodySetData(rearWheel_, rearWheelNode);
		dBodySetMovedCallback(rearWheel_, &Impl::onWheelMoved);

		dBodySetPosition(body_, 29, 34, 194.5);
	}

	/** Initializes the geometric shape of the bicycle for collision detection */
	void initGeoms() {
		
		// Set up ODE geoms
		geom_ = dCreateCapsule(game_->getSpace(), 0.2, 2*0.735 - 0.4);
		dGeomSetBody(geom_, body_);
		dGeomSetCategoryBits(geom_, TYPEWHEEL);
		dGeomSetCollideBits(geom_, TYPETERRAIN | TYPEROAD);
		
		dMatrix3 rotation;
		dRFromAxisAndAngle(rotation, 1.0, 0.0, 0.0, 3.14159/2);
		dGeomSetOffsetRotation(geom_, rotation);
	}

	

	/** Called when a new frame is detected */
	void onTimeStep() {
		
		processInput();
		//updateAnimation();

		if (enable) {
		calculateOrientation();
		}
	}

	float time;

	/** Update the animation of the cyclist */
	void updateAnimation() {
		// Update the cyclist animation
		if (time > 0.0) {
			cyclingState_->addTime(time);
		}

		if (!game_->getMouse()->getMouseState().buttonDown(OIS::MB_Left)) {
			time = 0.01;
		} else {
			time -= 0.0004f;
		}

	}

	/** Calculate the orientation of the bike based on velocity and lean angle */
	void calculateOrientation() {

		const dReal* velocity = dBodyGetLinearVel(body_);
		float speed = dLENGTH(velocity);

		if (leanAngle_ != 0.0f) {
			// Calculate the turn radius (r = v^2/(g*tan(theta)))
			turnRadius_ = (speed*speed)/(9.81*tanf(leanAngle_));

			// Calculate a small wobble when the bike is at high velocity and lean
			time_ += abs(0.01 * speed * leanAngle_);
			if (time_ > 6.28) {
				time_ -= 6.28;
			}
			float deviation = 0.01*cosf(time_);

			// Set the transformation matrix for the lean rotation
			centerNode_->setOrientation(Quaternion(Radian(leanAngle_ + deviation), Vector3(1.0f, 0.0f, 0.0f)));

			if (speed > 0.001) {
				// Get the transformation matrix to point the bike 
				// in the direction that it's moving
				dReal forward[3] = { velocity[0], velocity[1], velocity[2] }; // X AXIS
				dReal up[3] = { 0.0f, 1.0f, 0.0f }; // Y AXIS: Change to normal vector
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
		}

		if (speed > 0.001) {
			// Calculate the wheel rotation given the current velocity
			// of the bicycle
			dReal radius[3] = {0.0f, WHEELRADIUS, 0.0f};
			dReal linvel[3] = { speed, 0.0f, 0.0f };
			dReal cross[3];
			dCROSS(cross, =, radius, linvel);
			cross[0] /= WHEELRADIUS*WHEELRADIUS;
			cross[1] /= WHEELRADIUS*WHEELRADIUS;
			cross[2] /= WHEELRADIUS*WHEELRADIUS;
			dBodySetAngularVel(frontWheel_, cross[0], cross[1], cross[2]);
			dBodySetAngularVel(rearWheel_, cross[0], cross[1], cross[2]);
		}
	}

	/** Determine the new lean angle and apply acceleration/brake forces */
	void processInput() {
		// Get the new lean angle from the position of the getMouse on the screen
		unsigned int width, height, depth;
		int top, left;
		unsigned int x = game_->getMouse()->getMouseState().X.abs;
		game_->getWindow()->getMetrics(width, height, depth, left, top);
		float steering = MAXLEANANGLE*(x - width/2.0f)/(width/2.0f);
		leanAngle_ = leanAngle_*SENSITIVITY + (1-SENSITIVITY)*steering;

		if (game_->getKeyboard()->isKeyDown(OIS::KC_RETURN)) {
			enable = !enable;
		}


		dBodySetAngularVel(body_, 0, steering, 0);
	
		// Accelerate or brake the bike using input
		if (game_->getKeyboard()->isKeyDown(OIS::KC_UP)) {
			dBodyAddRelForce(body_, 1000, 0, 0);
		}
		if (game_->getKeyboard()->isKeyDown(OIS::KC_DOWN)) {
			dBodyAddRelForce(body_, -1000, 0, 0);
		}

		if (game_->getKeyboard()->isKeyDown(OIS::KC_U)) {
			dBodyAddForce(body_, 0, 1000, 0);
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

	/** Called when a wheel rotates to move the corresponding scene node */
	static void onWheelMoved(dBodyID body) {
		// Update the position of the scene node attached to this body.
		// Position is in global coordinates.
		SceneNode* node = static_cast<SceneNode*>(dBodyGetData(body));
		const dReal* quat = dBodyGetQuaternion(body);
	    
		// N.B.: ODE orders the quat as (w, x, y, z) (so quat[0] = w)
		node->setOrientation(quat[0], quat[1], quat[2], quat[3]);
	}

	Game* game_;

	Ogre::AnimationState* cyclingState_;

    dBodyID body_;
    dGeomID geom_;
	dBodyID frontWheel_;
	dBodyID rearWheel_;

	Ogre::SceneNode* centerNode_;
	Ogre::SceneNode* forkNode_;

	float leanAngle_;
	float turnRadius_;
	float steerAngle_;
	float time_;
};

Bicycle::Bicycle(Game* game) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->time_ = 0;
	impl_->initNodes();
	impl_->initGeoms();
	game->addListener(impl_.get());
}
