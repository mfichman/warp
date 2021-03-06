/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once 

#include "Warp.hpp"
#include <Bullet/btBulletDynamicsCommon.h>

#include <memory>
#include <list>

namespace Warp {

class Game : public Ogre::WindowEventListener, public Ogre::FrameListener {
public: 

	/** Creates a new game */
	Game();

    /** Destructor */
    ~Game();

	/** Returns the keyboard state object */
    OIS::Keyboard* getKeyboard() const;

	/** Returns the mouse state object */
    OIS::Mouse*	getMouse() const;

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

    /** Returns the physics world */
    btDynamicsWorld* getWorld() const;

	/** Returns the OSC listener */
	OscBeatListener* getOscBeatListener() const;

	/** Returns the OSC sender */
	OscSender* getOscSender() const;

    /** Returns the normalized mouse position in the range (-1, 1) */
    float getMouseNormalizedX() const;

    /** Returns the normalized mouse position in the range (-1, 1) */
    float getMouseNormalizedY() const;

	/** Adds a new event listener, which is called at a fixed interval (0.01 s) */
	void addListener(GameListener* listener);

	/** Removes an event listener */
	void removeListener(GameListener* listener);

	/** Sets the current level by name */
	void setLevel(const std::string& name);

	/** Set screen */
	void setScreen(const std::string& name);

	/** Returns the current level */
	Level* getLevel() const { return level_.get(); }

	/** Returns the physics timestep used */
	float getTimeStep() { return 0.01f; }
    
private:
	Game(const Game&);
    Game& operator=(const Game&);

	void loadScripting();
    void loadResources();
    void loadGraphics();
    void loadInput();
    void loadPhysics();
    void loadOsc();
	void loadScript(const std::string& name);

	// Ogre callbacks
	virtual void windowClosed(Ogre::RenderWindow* rw);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// Graphics objects
    Ogre::Root* root_;
    Ogre::Camera* camera_;
    Ogre::SceneManager*	sceneManager_;
    Ogre::Viewport*	viewport_;
    Ogre::RenderWindow*	window_;
    
    
	// Input objects
    OIS::InputManager* inputManager_;
    OIS::Keyboard* keyboard_;
    OIS::Mouse*	mouse_;
    
	// Physics objects
    btBroadphaseInterface* broadphase_;
    btCollisionDispatcher* dispatcher_;
    btConstraintSolver* solver_;
    btDefaultCollisionConfiguration* collisionConfiguration_;
    btDiscreteDynamicsWorld* world_;
    float physicsAccumulator_;
    
	// Game objects
	std::list<GameListener*> listeners_;

    // Scripting objects
    lua_State* scriptState_;

	// Osc object
	OscSender* oscSender_;
	OscBeatListener* oscListener_;

	// Current level
	std::auto_ptr<Level> level_;
	std::auto_ptr<Screen> screen_;

	bool resetEvent_;
	float gravity_;
};
}
