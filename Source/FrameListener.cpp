/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Application.hpp>
#include <FrameListener.hpp>
#include <algorithm>

using namespace Criterium;
using namespace std;

#define PHYSICSUPDATEINT 0.01f
#define MAXINTERVAL 0.25f

//------------------------------------------------------------------------------
FrameListener::FrameListener(Application* app) : 
    app_(app),
    physicsAccumulator_(0) {
    
    app_->root()->addFrameListener(this);
}

//------------------------------------------------------------------------------
FrameListener::~FrameListener() {
    app_->root()->removeFrameListener(this);
}

//------------------------------------------------------------------------------
bool 
FrameListener::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    app_->keyboard()->capture();
    app_->mouse()->capture();

    if (app_->keyboard()->isKeyDown(OIS::KC_ESCAPE)) {
        app_->root()->queueEndRendering();
    }
    
    physicsAccumulator_ += evt.timeSinceLastFrame;
    physicsAccumulator_ = std::min(physicsAccumulator_, MAXINTERVAL); // Prevent physics from running while app doesn't have focus
    while (physicsAccumulator_ >= PHYSICSUPDATEINT) { // Run fixed time steps using time in accumulator
    
        app_->bicycle()->onFrame();
        
        // Run collision detection 
        dSpaceCollide(app_->space(), app_, &FrameListener::onGeomCollision);
        
        // Step world
        dWorldStep(app_->world(), PHYSICSUPDATEINT);
        
        // Clean up
        dJointGroupEmpty(app_->contactJointGroup());
        physicsAccumulator_ -= PHYSICSUPDATEINT;
    }

    return true;
}
    
//------------------------------------------------------------------------------
bool 
FrameListener::frameEnded(const Ogre::FrameEvent& evt) {
    return true;
}

//------------------------------------------------------------------------------
void
FrameListener::onGeomCollision(void* data, dGeomID o1, dGeomID o2) {
    dContactGeom geom[10];
    Application* app = static_cast<Application*>(data);

    int num = dCollide(o1, o2, 10, geom, sizeof(dContactGeom));
    if (num <= 0) return;
    
    // Look up the surface params for these object types
    int t1 = dGeomGetCategoryBits(o1);
    int t2 = dGeomGetCategoryBits(o2);
    
    for (int i = 0; i < num; i++) {
        dContact contact;
		memset(&contact, 0, sizeof(dContact));
        contact.geom = geom[i];
        
		contact.surface.mode = dContactApprox1 | dContactFDir1 | dContactSlip2;
        contact.surface.mu = 0.0;
       // contact.surface.slip2 = 0.003;; * v; // Tire slip
        
        /*if (t1 == TYPEWHEEL && t2 == TYPETERRAIN) {
            const dReal* r = dBodyGetRotation(dGeomGetBody(o1));
            const dReal* vel = dBodyGetLinearVel(dGeomGetBody(o1));
            Ogre::Vector3 front(-r[2], -r[6], -r[10]);
            Ogre::Vector3 up(r[1], r[5], r[9]);
            
            float v = sqrtf(vel[0]*vel[0] + vel[1]*vel[1] + vel[2]*vel[2]);


			((Ogre::Vector3&)contact.fdir1) = front;
        }*/
        
        // Add a contact joint
        dJointID joint = dJointCreateContact(app->world(), app->contactJointGroup(), &contact);
        dJointAttach(joint, dGeomGetBody(o1), dGeomGetBody(o2));
    
    }


}




