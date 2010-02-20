/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once 

#include <Overlays.hpp>
#include <memory>
#include <Ogre.h>
#include <OIS/OIS.h>
#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/btBulletCollisionCommon.h>
extern "C" { 
#include <lua/lua.h> 
}

namespace Warp {

class Overlays;
class Objects;

struct SpineNode {
    SpineNode() : index(0) {}
    Ogre::Vector3 position;
    Ogre::Vector3 forward;
    int index;
};


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

    /** Returns the physics world */
    btDynamicsWorld* getWorld() const;

    /** Returns the normalized mouse position in the range (-1, 1) */
    float getMouseNormalizedX() const;

    /** Returns the normalized mouse position in the range (-1, 1) */
    float getMouseNormalizedY() const;

	/** Adds a new event listener, which is called at a fixed interval (0.01 s) */
	void addListener(Listener* listener);

	/** Removes an event listener */
	void removeListener(Listener* listener);

    /** Sets the current spine node */
    void setSpineNode(const SpineNode& node);

	/** Gets the current spine node */
	const SpineNode& getSpineNode() const;

    void setPlayerPosition(const Ogre::Vector3& pos);

    const Ogre::Vector3& getPlayerPosition() const;

    /** Attach a new level object (call to change levels) */
    void loadLevel(std::string name);
    
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
    virtual void onCollision() {}

};
}
