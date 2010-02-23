/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Game.hpp"

#include "Level.hpp"
#include "Player.hpp"

#include "OscBeatListener.hpp"
#include "OscSender.hpp"
#include <Ogre.h>
#include <OgreCEGUIRenderer.h>
#include <CEGUI/CEGUI.h>
#include <OIS/OIS.h>
#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/btBulletCollisionCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
extern "C" {
#include <lua/lua.h> 
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}
#include <string>
#include <iostream>
#include <algorithm>
#include <utility>

using namespace Warp;
using namespace Ogre;
using namespace std;

#define PHYSICSUPDATEINTERVAL	0.01f // seconds
#define PHYSICSMAXINTERVAL		0.25f // seconds

// for OSC interface to chuck
#define SEND_PORT 6449
#define LISTEN_PORT 7000

Game::Game() : 
	guiRenderer_(0), 
	guiSystem_(0), 
	inputManager_(0),
	keyboard_(0),
	mouse_(0),
    scriptState_(0),
    collisionConfiguration_(0),
    dispatcher_(0),
    broadphase_(0),
    solver_(0),
    world_(0),
    physicsAccumulator_(0.0f),
    root_(new Root("plugins.cfg", "ogre.cfg", "ogre.log")) {

    loadScripting();
    loadResources();
    loadGraphics();
    loadInput();
    loadPhysics();
    loadOsc();
}

/** Destroys subsystems */
Game::~Game() {
	level_.reset();

	// Destroy the level before tearing down the physics and graphics
	// engines
	if (guiRenderer_) { delete guiRenderer_;	}
	if (guiSystem_) { delete guiSystem_; }
	if (root_) { delete root_; }
	if (inputManager_) { 
		if (keyboard_) inputManager_->destroyInputObject(keyboard_);
		if (mouse_) inputManager_->destroyInputObject(mouse_);
		OIS::InputManager::destroyInputSystem(inputManager_);
	}
    if (scriptState_) { lua_close(scriptState_); }
    
    if (world_) { delete world_; }
    if (solver_) { delete solver_; }
    if (broadphase_) { delete broadphase_; }
    if (dispatcher_) { delete dispatcher_; }
    if (collisionConfiguration_) { delete collisionConfiguration_; }        
}

/** Loads resource configuration files */
void Game::loadResources() {
	ConfigFile config;
	config.load("resources.cfg");
    
	ConfigFile::SectionIterator s = config.getSectionIterator();
	while (s.hasMoreElements()) {
		String section = s.peekNextKey();
		ConfigFile::SettingsMultiMap* settings = s.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i) {
			String type = i->first;
			String arch = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(arch, type, section);
		} 
	} 
}

/** Loads the OGRE renderer and opens the main window */
void Game::loadGraphics() {
	// Set up the render system
	// || !root_->restoreConfig()
	if (!root_->showConfigDialog()) {
		throw Exception(52, "User cancelled the config dialog.", "");
	}
    
	// Create render window
	window_ = root_->initialise(true);

	// Create scene manager
	sceneManager_ = root_->createSceneManager(ST_EXTERIOR_CLOSE, "Default");
    sceneManager_->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
    //sceneManager_->setShadowFarDistance(10.0f);

	// Create the main camera
	camera_ = sceneManager_->createCamera("Camera");
	camera_->setNearClipDistance(0.5);
	camera_->setFarClipDistance(400);

	// Initialize resources
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
    
	// Initialize viewport
	viewport_ = window_->addViewport(camera_);
	viewport_->setBackgroundColour(ColourValue(0.7, 0.7, 0.9));
	camera_->setAspectRatio(Real(viewport_->getActualWidth())/Real(viewport_->getActualHeight()));
    
	// CEGUI Setup
	guiRenderer_ = new CEGUI::OgreCEGUIRenderer(window_, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, sceneManager_);
	guiSystem_ = new CEGUI::System(guiRenderer_);
	root_->addFrameListener(this);
	Ogre::WindowEventUtilities::addWindowEventListener(window_, this);
	
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

/** Loads the input manager */
void Game::loadInput() {
	// Set up the input system
	size_t windowHandle = 0;
	std::ostringstream windowName;
	OIS::ParamList params;
	window_->getCustomAttribute("WINDOW", &windowHandle);
	windowName << windowHandle;
	params.insert(make_pair(string("WINDOW"), windowName.str()));
	inputManager_ = OIS::InputManager::createInputSystem(params);
    
	// Setup the keyboard
	try {
		keyboard_ = static_cast<OIS::Keyboard*>(inputManager_->createInputObject(OIS::OISKeyboard, true));
		mouse_ = static_cast<OIS::Mouse*>(inputManager_->createInputObject(OIS::OISMouse, true));

		unsigned int width, height, depth;
		int top, left;
		window_->getMetrics(width, height, depth, left, top);
		const OIS::MouseState& ms = mouse_->getMouseState();
		ms.width = width;
		ms.height = height;
	
	} catch (const OIS::Exception& ex) {
		throw Exception(42, ex.eText, "");
	}
}

/** Loads the physics world and collision space */
void Game::loadPhysics() {
    collisionConfiguration_ = new btDefaultCollisionConfiguration();
    dispatcher_ = new btCollisionDispatcher(collisionConfiguration_);
    broadphase_ = new btDbvtBroadphase();
    solver_ = new btSequentialImpulseConstraintSolver();
    world_ = new btDiscreteDynamicsWorld(dispatcher_, broadphase_, solver_, collisionConfiguration_);
    world_->getSolverInfo().m_splitImpulse = true;
    world_->getSolverInfo().m_splitImpulsePenetrationThreshold = 00.0f;
    //world_->getSolverInfo().m_maxErrorReduction = 0.0f;
    //world_->getSolverInfo().m_numIterations = 20;
    world_->getSolverInfo().m_restitution = 0.0f;
    world_->getSolverInfo().m_globalCfm = 1000.0f;
    world_->getSolverInfo().m_erp = 1.00f;
    world_->getSolverInfo().m_erp2 = 1.00f;
    world_->setGravity(btVector3(0, 0, 0));
    btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher_);

}

/** Loads the scripting engine */
void Game::loadScripting() {
    scriptState_ = lua_open();
    luaL_openlibs(scriptState_);

    lua_pushlightuserdata(scriptState_, this); // Modifies an entity 
    lua_pushcclosure(scriptState_, &Game::luaGetNode, 1);
    lua_setglobal(scriptState_, "wGetNode");

    lua_pushlightuserdata(scriptState_, this); // Modifies an entity 
    lua_pushcclosure(scriptState_, &Game::luaSetNode, 1);
    lua_setglobal(scriptState_, "wSetNode");

    lua_pushlightuserdata(scriptState_, this); // Modifies a light
    lua_pushcclosure(scriptState_, &Game::luaGetLight, 1);
    lua_setglobal(scriptState_, "wGetLight");

    lua_pushlightuserdata(scriptState_, this); // Modifies a light
    lua_pushcclosure(scriptState_, &Game::luaSetLight, 1);
    lua_setglobal(scriptState_, "wSetLight");

	lua_pushlightuserdata(scriptState_, this); // Returns the current spine node
    lua_pushcclosure(scriptState_, &Game::luaGetSpineNodeId, 1);
    lua_setglobal(scriptState_, "wGetSpineNodeId");

    lua_pushlightuserdata(scriptState_, this); // Gets the current beat
    lua_pushcclosure(scriptState_, &Game::luaGetBeat, 1);
    lua_setglobal(scriptState_, "wGetBeat");

    lua_pushlightuserdata(scriptState_, this); // tell chuck to enqueue loop
    lua_pushcclosure(scriptState_, &Game::luaQueueStartLoop, 1);
    lua_setglobal(scriptState_, "wQueueStartLoop");

    lua_pushlightuserdata(scriptState_, this); // tell chuck to enqueue loop
    lua_pushcclosure(scriptState_, &Game::luaStartBeatServer, 1);
    lua_setglobal(scriptState_, "wStartBeatServer");

    if (luaL_dofile(scriptState_, "Scripts/Warp.lua")) {
        string message(lua_tostring(scriptState_, -1));
        lua_pop(scriptState_, 2);
        throw runtime_error("Script error: " + message);
    }
}

/** load up osc interaction */
void Game::loadOsc() {
	// initialize sender
	oscSender_ = new OscSender(SEND_PORT);
	oscListener_ = new OscBeatListener(LISTEN_PORT);
}

/** Called when the main window is closed */
void Game::windowClosed(RenderWindow* rw) { 
	root_->queueEndRendering();
}

/** Called when a frame begins */
bool Game::frameRenderingQueued(const FrameEvent& evt) { 
	keyboard_->capture();
	mouse_->capture();

	if (keyboard_->isKeyDown(OIS::KC_ESCAPE)) {
		root_->queueEndRendering();
	}

	// Prevent physics from running while game doesn't have focus
	physicsAccumulator_ += evt.timeSinceLastFrame;
	physicsAccumulator_ = std::min(physicsAccumulator_, PHYSICSMAXINTERVAL); 

    while (physicsAccumulator_ >= PHYSICSUPDATEINTERVAL) { 
        list<GameListener*>::iterator i = listeners_.begin();
        while (i != listeners_.end()) {
            GameListener* listener = *i;
            i++;
            listener->onTimeStep();
        }

        // Step the world using the fixed timestep
        world_->stepSimulation(PHYSICSUPDATEINTERVAL, 0);

        physicsAccumulator_ -= PHYSICSUPDATEINTERVAL;
    }

	return true;
}



/** Lua callback.  Gets the given values for the node */
int Game::luaGetNode(lua_State* env) {
    Game* game = (Game*)lua_touserdata(env, lua_upvalueindex(1));

    if (!lua_isstring(env, 1)) {
        lua_pushstring(env, "Expected string for node name");
        lua_error(env);
    }
    string name(lua_tostring(env, 1)); // First argument: name of the entity
    if (!game->sceneManager_->hasSceneNode(name)) {
        lua_pushstring(env, "Invalid node name");
        lua_error(env);
    }
    SceneNode* node = game->sceneManager_->getSceneNode(name);
    env << *node;

    return 1;
}

/** Lua callback.  Gets the given values for the node */
int Game::luaSetNode(lua_State* env) {
    Game* game = (Game*)lua_touserdata(env, lua_upvalueindex(1));

    if (!lua_isstring(env, 1)) {
        lua_pushstring(env, "Expected string for node name");
        lua_error(env);
    }
    string name(lua_tostring(env, 1)); // First argument: name of the entity
    if (!game->sceneManager_->hasEntity(name)) {
        lua_pushstring(env, "Invalid node name");
        lua_error(env);
    }
    SceneNode* node = game->sceneManager_->getSceneNode(name);
    env >> *node;

    return 1;
}

/** Lua callback.  Get the light state */
int Game::luaGetLight(lua_State* env) {
    Game* game = (Game*)lua_touserdata(env, lua_upvalueindex(1));
    if (!lua_isstring(env, 1)) {
        lua_pushstring(env, "Expected string for entity name");
        lua_error(env);
    }       
    string name(lua_tostring(env, 1)); // First argument: name of the light
    if (!game->sceneManager_->hasLight(name)) {
        lua_pushstring(env, "Invalid entity name");
        lua_error(env);
    }
    Light* light = game->sceneManager_->getLight(name);
    env << *light;

    return 1;
}

/** Lua callback.  Sets the given values for the light */
int Game::luaSetLight(lua_State* env) {
    Game* game = (Game*)lua_touserdata(env, lua_upvalueindex(1));
    if (!lua_isstring(env, 1)) {
        lua_pushstring(env, "Expected string for entity name");
        lua_error(env);
    }       
    string name(lua_tostring(env, 1)); // First argument: name of the light
    if (!game->sceneManager_->hasLight(name)) {
        lua_pushstring(env, "Invalid entity name");
        lua_error(env);
    }
    Light* light = game->sceneManager_->getLight(name);
    env >> *light;
    
    return 0;
}

/** Lua callback.  Returns the current spine node ID. */
int Game::luaGetSpineNodeId(lua_State* env) {
	Game* game = (Game*)lua_touserdata(env, lua_upvalueindex(1));
	if (game->level_.get()) {
		lua_pushinteger(env, game->level_->getPlayer()->getSpineNodeIndex());
	} else {
		lua_pushnil(env);
	}
    return 1;
}

/** Lua callback.  Creates an object */
int Game::luaCreateObject(lua_State* env) {
    Game* game = (Game*)lua_touserdata(env, lua_upvalueindex(1));


    return 0;
}

/** Lua callback.  Gets the current beat as set by chuck */
int Game::luaGetBeat(lua_State* env) {
    Game* game = (Game*)lua_touserdata(env, lua_upvalueindex(1));
    lua_pushinteger(env, game->oscListener_->getCurBeat());
    return 1;
}

/** Lua callback.  Sends chuck the message to start a loop
 *  It gives a string id for the loop and the pathname to find it
 */
int Game::luaQueueStartLoop(lua_State* env) {
    // get msg
    BeatLoop beat_loop;

    env >> beat_loop;
    Game* game = (Game*)lua_touserdata(env, lua_upvalueindex(1));

    // send msg
    OscSender* sender = game->oscSender_;
    sender->beginMsg("/loop/start");
    sender->addString(beat_loop.name);
    sender->addString(beat_loop.path_name);
    sender->addInt(beat_loop.bpm);
    sender->addInt(beat_loop.n_beats);
    sender->sendMsg();
    return 1;
}

int Game::luaStartBeatServer(lua_State* env) {
    Game* game = (Game*)lua_touserdata(env, lua_upvalueindex(1));
    OscSender* sender = game->oscSender_;

    int bpm = 120; // default value
    lua_getfield(env, -1, "bpm");
    if (!lua_isnil(env, -1)) {
        bpm = lua_tointeger(env, -1);
    }
    lua_pop(env, 1); 
    sender->beginMsg("/server/start");
    sender->addInt(bpm);
    sender->sendMsg();
    return 1;
}

OIS::Keyboard* Game::getKeyboard() const { 
	return keyboard_; 
}

OIS::Mouse*	Game::getMouse() const { 
	return mouse_; 
}

Ogre::Root* Game::getRoot() const { 
	return root_; 
}

Ogre::Camera* Game::getCamera() const { 
	return camera_; 

}
Ogre::SceneManager*	Game::getSceneManager() const { 
	return sceneManager_; 
}

Ogre::RenderWindow*	Game::getWindow() const { 
	return window_; 
}

lua_State* Game::getScriptState() const {
    return scriptState_;
}

btDynamicsWorld* Game::getWorld() const {
    return world_;
}

float Game::getMouseNormalizedX() const {
	unsigned int width, height, depth;
	int top, left;
	unsigned int x = mouse_->getMouseState().X.abs;
	window_->getMetrics(width, height, depth, left, top);
    return (x - width/2.0f)/(width/2.0f);

}

float Game::getMouseNormalizedY() const {
	unsigned int width, height, depth;
	int top, left;
	unsigned int y = mouse_->getMouseState().Y.abs;
	window_->getMetrics(width, height, depth, left, top);
    return (y - height/2.0f)/(height/2.0f);
}

void Game::addListener(GameListener* listener) { 
	listeners_.push_back(listener);
}

void Game::removeListener(GameListener* listener) { 
	list<GameListener*>::iterator i = find(listeners_.begin(), listeners_.end(), listener);

	if (i != listeners_.end()) {
		listeners_.erase(i);
	}
}

void Game::setLevel(const string& name) {
	level_.reset(new Level(this, name));
}	