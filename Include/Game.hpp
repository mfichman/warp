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

#define TYPEWHEEL 0x0001
#define TYPETERRAIN 0x0002
#define TYPEGUARD 0x0004

namespace Criterium {

class Game : public Interface {
public: 
    typedef intrusive_ptr<Game> Ptr;
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

	/** Adds a new event listener */
	void addListener(Listener* listener);

	/** Removes a listener */
	void removeListener(Listener* listener);
    
private:
	std::auto_ptr<Impl> impl_;
};

class Game::Listener : public Interface {
public:
	typedef intrusive_ptr<Game::Listener> Ptr;

	/** Called when every time a new timestep is calculated */
	virtual void onTimeStep() {}
};

}