/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once 

#include <Interface.hpp>
#include <FrameListener.hpp>
#include <WindowListener.hpp>
#include <Ogre.h>
#include <OIS/OIS.h>
#include <ode/ode.h>
#include <CEGUI/CEGUI.h>
#include <OgreCEGUIRenderer.h>
#include <memory>
#include <map>
#include <utility>
#include <Bicycle.hpp>
#include <Tree.hpp>

#define TYPEWHEEL 0x0001
#define TYPETERRAIN 0x0002
#define TYPEGUARD 0x0004

namespace Criterium {
using namespace std;
using namespace boost;

class Application : public Interface {
public: 
    typedef intrusive_ptr<Application> Ptr;
   
    static Application::Ptr make();
    Ogre::Root* root() const { return root_.get(); };
    Ogre::Camera* camera() const { return camera_; }
    Ogre::SceneManager* sceneManager() const { return sceneManager_; }
    Ogre::RenderWindow* window() const { return window_; }
    OIS::Keyboard* keyboard() const { return keyboard_; }
    OIS::Mouse* mouse() const { return mouse_; }
    dWorldID world() const { return world_; }
    dSpaceID space() const { return space_; }
    dJointGroupID contactJointGroup() const { return contactJointGroup_; }
    Bicycle::Ptr bicycle() const { return bicycle_; }
    
private:
    Application();
    ~Application();
   
    void loadResources();
    void loadGraphics();
    void loadInput();
    void loadPhysics();

    auto_ptr<Ogre::Root> root_;
    Ogre::Camera* camera_;
    Ogre::SceneManager* sceneManager_;
    Ogre::Viewport* viewport_;
    Ogre::RenderWindow* window_;
    
    auto_ptr<CEGUI::OgreCEGUIRenderer> guiRenderer_;
    auto_ptr<CEGUI::System> guiSystem_;
    
    FrameListener::Ptr frameListener_;
    WindowListener::Ptr windowListener_;
    
    OIS::InputManager* inputManager_;
    OIS::Keyboard* keyboard_;
    OIS::Mouse* mouse_;
    
    dWorldID world_;
    dSpaceID space_;
    dJointGroupID contactJointGroup_;
    
    Bicycle::Ptr bicycle_;
	Tree::Ptr tree_;
};

}
