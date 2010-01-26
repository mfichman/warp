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

    static void onBodyMoved(dBodyID id);
	static void onWheelMoved(dBodyID id);

    Application* app_;
    
    Animation nextAnimation_;
    Animation currentAnimation_;

    dBodyID body_;
    dGeomID geom_;
	dBodyID frontWheel_;
	dBodyID rearWheel_;

	Ogre::SceneNode* centerNode_;
	Ogre::SceneNode* forkNode_;

	float leanAngle_;
	float turnRadius_;
	float steerAngle_;
};

}
