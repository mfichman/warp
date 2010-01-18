/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once 

#include <Ogre.h>
#include <ode/ode.h>

namespace Criterium {
class Application;

class FrameListener : public Interface, public Ogre::FrameListener {
public:
    friend class Application;
    ~FrameListener();
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    virtual bool frameEnded(const Ogre::FrameEvent& evt);
    
private:
    FrameListener(Application* app);
    static void onGeomCollision(void* data, dGeomID o1, dGeomID o2);
    Application* app_;
    float physicsAccumulator_;
};

}
