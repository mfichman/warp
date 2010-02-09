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
#define FRAMERADIUS 0.5f // meters
#define FRAMEMASS 80.000f // kilograms

#define WHEELRADIUS 0.368f // meters
#define WHEELBASE 0.1108f // meters

#define CASTERANGLE 0.3988f // radians
#define MAXLEANANGLE 0.8000f // radians
#define SENSITIVITY 0.1000f 

#define ALPHA 0.1f 
#define BETA 0.02f
#define GAMMA 0.5

#define WINDCOEFF 0.3

#define FORCEACCEL 200.0f // Newtons
#define FORCEBRAKE 800.0f // Newtons
#define FORCETURN 800.0f // Newtons

struct Bicycle::Impl : public Game::Listener {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void initNodes() {

		Entity* entity;

		// Set up OGRE scene nodes
		SceneNode* root = game_->getSceneManager()->getRootSceneNode();
		bodyNode_ = root->createChildSceneNode("Bike");

		centerNode_ = bodyNode_->createChildSceneNode("Center");
		centerNode_->setPosition(0.0f, -0.5, 0.0f);

		// Bike frame
		SceneNode* frameNode = centerNode_->createChildSceneNode("Frame");
		entity = game_->getSceneManager()->createEntity("Frame", "Frame.mesh");
		frameNode->attachObject(entity);
		frameNode->setPosition(0, 0.735, 0);

		// Front bike fork
		forkNode_ = centerNode_->createChildSceneNode("Fork");
		entity = game_->getSceneManager()->createEntity("Fork", "Fork.mesh");
		forkNode_->attachObject(entity);
		forkNode_->setPosition(0.516, 0.781, 0.000);
		
		// Front wheel
		SceneNode* frontWheelNode = forkNode_->createChildSceneNode("FrontWheel");
		entity = game_->getSceneManager()->createEntity("FrontWheel", "Wheel.mesh");
		frontWheelNode->attachObject(entity);
		frontWheelNode->setPosition(0.177, -0.420, 0.000);
		
		// Rear wheel
		SceneNode* rearWheelNode = centerNode_->createChildSceneNode("RearWheel");
		entity = game_->getSceneManager()->createEntity("RearWheel", "Wheel.mesh");
		rearWheelNode->attachObject(entity);
		rearWheelNode->setPosition(-0.415, 0.368, 0.000);

		// Cyclist and animation state
		SceneNode* cyclistNode = centerNode_->createChildSceneNode("Cyclist");
		entity = game_->getSceneManager()->createEntity("Shirt", "Cyclist.mesh");
		cyclistNode->attachObject(entity);
		cyclistNode->setPosition(-0.2600, 1.020, 0.000);
		cyclistNode->setOrientation(Quaternion(Degree(90), Vector3(0.000, 1.000, 0.000)));
		cyclingState_ = entity->getAnimationState("Cycling");
		cyclingState_->setEnabled(true);
		cyclingState_->setLoop(true);

        Technique* t = entity->getSubEntity(0)->getMaterial()->getBestTechnique();
        Pass* p = t->getPass(0);
        if (p->hasVertexProgram() && p->getVertexProgram()->isSkeletalAnimationIncluded()) {

        } else {
            exit(0);
        }

        dMass mass; 
        dMassSetSphere(&mass, FRAMEMASS, FRAMERADIUS);

		// Set up ODE bodies
		body_ = dBodyCreate(game_->getWorld());
		dBodySetMass(body_, &mass);
		dBodySetData(body_, bodyNode_);
        dBodySetMaxAngularSpeed(body_, 0);
		dBodySetMovedCallback(body_, &Impl::onBodyMoved);

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

        dBodySetPosition(body_, 629.0f, 75.0f, 546.0f);

	}

	/** Initializes the geometric shape of the bicycle for collision detection */
	void initGeoms() {
		
		// Set up ODE geoms
		geom_ = dCreateSphere(game_->getSpace(), FRAMERADIUS);
		dGeomSetBody(geom_, body_);
		dGeomSetCategoryBits(geom_, TYPEWHEEL);
		dGeomSetCollideBits(geom_, TYPETERRAIN | TYPEROAD);
	}
	

	/** Called when a new frame is detected */
	void onTimeStep() {
		
		processInput();
		calculateOrientation();
        updateAnimation();
	}



	/** Update the animation of the cyclist */
	void updateAnimation() {
		// Update the cyclist animation
		if (animationSpeed_ > 0.0) {
			cyclingState_->addTime(animationSpeed_);
		}

        if (game_->getKeyboard()->isKeyDown(OIS::KC_UP)) {
			animationSpeed_ = 0.01;
		} else {
			animationSpeed_ -= 0.0004f;
		}

	}

	/** Calculate the orientation of the bike based on velocity and lean angle */
	void calculateOrientation() {
        Vector3 right = front_.crossProduct(Vector3::UNIT_Y);
        Vector3 up = right.crossProduct(front_);
        bodyNode_->setOrientation(Quaternion(front_, up, right));

        Vector3 velocity(dBodyGetLinearVel(body_));
        Vector3 force(dBodyGetForce(body_));
		float speed = velocity.length();
        
        float gravity = game_->getGravity();

        // Calculate the lean angle of the bike from gravity and centripetal force
        float leanAngle = atan2(force.dotProduct(right), gravity * FRAMEMASS);

        // Set the transformation matrix for the lean rotation
        centerNode_->setOrientation(Quaternion(Radian(leanAngle), Vector3::UNIT_X));

		if (speed > 0.001) {
			// Calculate the wheel rotation given the current velocity
			// of the bicycle
            Vector3 radius(0.0f, WHEELRADIUS, 0.0f);
            Vector3 linvel(speed, 0.0f, 0.0f);
            Vector3 cross = radius.crossProduct(linvel) / WHEELRADIUS*WHEELRADIUS;
			dBodySetAngularVel(frontWheel_, cross.x, cross.y, cross.z);
			dBodySetAngularVel(rearWheel_, cross.x, cross.y, cross.z);
		}
	}

	/** Determine the new lean angle and apply acceleration/brake forces */
	void processInput() {
        Vector3 v(dBodyGetLinearVel(body_));
        Vector3 d = v.normalisedCopy();

        // Smooth the front vector
        front_.x = ALPHA*d.x + (1-ALPHA)*front_.x;
        front_.y = BETA*d.y + (1-BETA)*front_.y;
        front_.z = ALPHA*d.z + (1-ALPHA)*front_.z;
        front_.normalise();

        // Calculate position of the camera
        Vector3 p(dBodyGetPosition(body_));
        Vector3 position = -6 * front_ + p + Vector3(0.0f, 2.0f, 0.0f);
        Vector3 look = 6 * front_ + p;
        position = (GAMMA)*position + (1-GAMMA)*game_->getCamera()->getPosition();
        game_->getCamera()->lookAt(look);
        game_->getCamera()->setPosition(position);

        // Reset the position of the object if ENTER is hit
        // Hack hack hack
        if (game_->getKeyboard()->isKeyDown(OIS::KC_RETURN)) {
            dBodySetPosition(body_, 629.0f, 75.0f, 546.0f);
            dBodySetLinearVel(body_, 0.0f, 0.0f, 0.0f);
            dBodySetAngularVel(body_, 0.0f, 0.0f, 0.0f);
            front_ = Vector3::UNIT_X;
        }


        // Calculate wind resistance from speed and unit front vector
        Vector3 fw = -d * v.squaredLength() * WINDCOEFF;
        dBodyAddForce(body_, fw.x, fw.y, fw.z);

        

        // Add forces for steering and acceleration
        if (game_->getKeyboard()->isKeyDown(OIS::KC_UP)) {
            Vector3 fa = FORCEACCEL * d;
            dBodyAddForce(body_, fa.x, fa.y, fa.z);
        }
        if (game_->getKeyboard()->isKeyDown(OIS::KC_DOWN)) {
            Vector3 fb = -FORCEBRAKE * d;
            dBodyAddForce(body_, fb.x, fb.y, fb.z);
        }
        turnForce_= game_->getMouseNormalizedX() * FORCETURN;



        Vector3 ft = turnForce_ * d.crossProduct(Vector3::UNIT_Y);
        dBodyAddForce(body_, ft.x, ft.y, ft.z);

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
		//node->setOrientation(quat[0], quat[1], quat[2], quat[3]);
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
    Ogre::SceneNode* bodyNode_;

    Vector3 front_;
    float turnForce_;
    float animationSpeed_;
};

Bicycle::Bicycle(Game* game) : impl_(new Impl()) {
	impl_->game_ = game;
    impl_->turnForce_ = 0.0f;
	impl_->initNodes();
	impl_->initGeoms();
	game->addListener(impl_.get());
}
