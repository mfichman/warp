/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Interface.hpp>
#include <Ogre.h>
#include <ode/ode.h>

namespace Criterium {
using namespace std;
using namespace boost;

class Application;

class Bicycle : public Interface {
public:
    typedef intrusive_ptr<Bicycle> Ptr;
    enum Animation { animCycling, animSanding, animLeftTurn, animRightTurn, animCoast };
    
    ~Bicycle();
    
    static Bicycle::Ptr make(Application* app);
    void nextAnimation(Animation a);
    Animation nextAnimation() const { return nextAnimation_; }
    void currentAnimation(Animation a);
    Animation currentAnimation() const { return currentAnimation_; }
    void onFrame();

private:
    Bicycle(Application* app);
	void initBodies();
	void initGeoms();
	void initJoints();
	void initMotors();

    static void onBodyNodeMoved(dBodyID id);

    Application* app_;
    
    Animation nextAnimation_;
    Animation currentAnimation_;

    dBodyID frontWheelBody_;
    dBodyID rearWheelBody_;
    dBodyID frameBody_;
    dBodyID forkBody_;

    dGeomID frontWheelGeom_;
    dGeomID rearWheelGeom_;
    dGeomID frameGeom_;
    
    dJointID rearWheelJoint_;
    dJointID frontWheelJoint_;
    dJointID forkJoint_;

	dJointID leanMotor_;

	float leanAngle_;
	float turnRadius_;
};

}
