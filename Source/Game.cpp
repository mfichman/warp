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
    //sceneManager_->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
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
	params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
	params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	params.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	params.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
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
    //world_->getSolverInfo().m_splitImpulse = true;
    //world_->getSolverInfo().m_splitImpulsePenetrationThreshold = 00.0f;
    //world_->getSolverInfo().m_maxErrorReduction = 0.0f;
    //world_->getSolverInfo().m_numIterations = 20;
    //world_->getSolverInfo().m_restitution = 0.0f;
    //world_->getSolverInfo().m_globalCfm = 1000.0f;
    //world_->getSolverInfo().m_erp = 1.00f;
    //world_->getSolverInfo().m_erp2 = 1.00f;
    world_->setGravity(btVector3(0, -2, 0));
	world_->setInternalTickCallback(&Game::onTick, this, true);
	world_->setWorldUserInfo(this);
    btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher_);

}

/** Loads a script */
void Game::loadScript(const std::string& name) {
	lua_State* env = scriptState_;
	if (luaL_dofile(env, name.c_str())) {
        string message(lua_tostring(env, -1));
        lua_pop(env, 2);
        throw runtime_error("ScriptTask error: " + message);
    }
}

/** Loads the scripting engine */
void Game::loadScripting() {
    scriptState_ = lua_open();
    luaL_openlibs(scriptState_);
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


	physicsAccumulator_ += evt.timeSinceLastFrame;

	if (keyboard_->isKeyDown(OIS::KC_ESCAPE)) {
		root_->queueEndRendering();
	}
/*
	if (keyboard_->isKeyDown(OIS::KC_S)) {
		world_->stepSimulation(evt.timeSinceLastFrame / 4.0, 3);
	} else {
		world_->stepSimulation(evt.timeSinceLastFrame, 3);
	}
	*/
	list<GameListener*>::iterator i = listeners_.begin();
	while (i != listeners_.end()) {
		GameListener* listener = *i;
		i++;
		listener->onTimeStep();
	}
	return true;
}

void Game::onTick(btDynamicsWorld* world, btScalar step) {
	world->clearForces();

	// Check for collisions
	int nmanifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < nmanifolds; i++) {
		btPersistentManifold* manifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* a = static_cast<btCollisionObject*>(manifold->getBody0());
		btCollisionObject* b = static_cast<btCollisionObject*>(manifold->getBody1());

		Object* ca = static_cast<Object*>(a->getUserPointer());
		Object* cb = static_cast<Object*>(b->getUserPointer());

		// Perform double dynamic dispatch
		if (ca && cb) {
			ca->collide(cb);
			cb->collide(ca);
		}
	}

	Game* game = static_cast<Game*>(world->getWorldUserInfo());
	list<GameListener*>::iterator i = game->listeners_.begin();
	while (i != game->listeners_.end()) {
		GameListener* listener = *i;
		i++;
		listener->onTimeStep();
	}
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

OscBeatListener* Game::getOscBeatListener() const {
	return oscListener_;
}

OscSender* Game::getOscSender() const {
	return oscSender_;
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
	// Make sure the old level is released first!
	level_.reset();
	level_.reset(new Level(this, name));
}	