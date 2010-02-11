/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once 

#include <Overlays.hpp>
#include <memory>
#include <Ogre.h>
#include <OIS/OIS.h>
#include <ode/ode.h>
extern "C" { 
#include <lua/lua.h> 
}

namespace Criterium {

class Overlays;
class Objects;

class Game {
public: 
	struct Impl; 
	class Listener;
    class Collidable;

	/** Creates a new game */
	Game();

    /** Destructor */
    ~Game();

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
    Objects* getObjects() const;

    /** Returns the overlays object */
    Overlays* getOverlays() const;

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

class Game::Collidable {
public:
    
    /** Called when an object collides with another object */
    virtual void onCollision(dGeomID other, dContactGeom& contact) {}

};

}