/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once 

#include <Interface.hpp>
#include <memory>
#include <Ogre.h>
#include <OIS/OIS.h>
#include <ode/ode.h>
extern "C" { 
#include <lua/lua.h> 
}

#define TYPEWHEEL 0x0001
#define TYPETERRAIN 0x0002
#define TYPEGUARD 0x0004
#define TYPEBALL 0x0008
#define TYPEROAD 0x0010

namespace Criterium {

class Game {
public: 
	struct Impl; 
	class Listener;

	/** Creates a new game */
	Game();

	/** Returns the keyboard state object */
    OIS::Keyboard* getKeyboard() const;

	/** Returns the mouse state object */
    OIS::Mouse*	getMouse() const;

	/** Returns the ODE physics world */
    dWorldID getWorld() const;

	/** Returns the ODE collision space */
    dSpaceID getSpace() const;

	/** Returns the OGRE root renderer object */
    Ogre::Root*	getRoot() const;

	/** Returns the primary camera for the main viewport */
    Ogre::Camera* getCamera() const;

	/** Returns the scene manager object */
    Ogre::SceneManager*	getSceneManager() const;

	/** Returns the main window */
    Ogre::RenderWindow*	getWindow() const;

    /** Returns the main script engine handle */
    lua_State* getScriptState() const;

    /** Returns the object factory */
    Factory* getObjectFactory() const;

    /** Returns the gravity constant */
    float getGravity() const;

    /** Returns the normalized mouse position in the range (-1, 1) */
    float getMouseNormalizedX() const;

    /** Returns the normalized mouse position in the range (-1, 1) */
    float getMouseNormalizedY() const;

	/** Adds a new event listener, which is called at a fixed interval (0.01 s) */
	void addListener(Listener* listener);

	/** Removes an event listener */
	void removeListener(Listener* listener);
    
private:
	std::auto_ptr<Impl> impl_;
};

class Game::Listener {
public:

	/** Called for every physics timestep (fixed at every 0.01 s) */
	virtual void onTimeStep() {}
};

}