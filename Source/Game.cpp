/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Game.hpp"

#include "Level.hpp"
#include "Screen.hpp"
#include "Player.hpp"
#include "OscBeatListener.hpp"
#include "OscSender.hpp"
#include "ScriptTask.hpp"

#include <Ogre.h>
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
#define OSC_SEND_PORT 6449
#define OSC_LISTEN_PORT 7000
#define MAX_TIME_LAG 0.2f

Game::Game() : 
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
	resetEvent_(false),
	gravity_(-4.0f) {

#ifdef WARP_RELEASE
	root_ = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
#endif
#ifdef WARP_DEBUG
#pragma message("Using debug build") 
	cout << "Loading debug plugins" << endl;
	root_ = new Root("plugins.debug.cfg", "ogre.cfg", "ogre.log");
#endif

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
	screen_.reset();

	oscSender_->beginMsg("/kill");
	oscSender_->addInt(1);
	oscSender_->sendMsg();

	// Destroy the level before tearing down the physics and graphics
	// engines
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
    sceneManager_->setShadowFarDistance(100.0f);

	// Create the main camera
	camera_ = sceneManager_->createCamera("Camera");
	camera_->setNearClipDistance(0.5);
	camera_->setFarClipDistance(400);

	// Initialize resources
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
    
	// Initialize viewport
	viewport_ = window_->addViewport(camera_);
	viewport_->setBackgroundColour(ColourValue(0.7f, 0.7f, 0.9f));
	camera_->setAspectRatio(Real(viewport_->getActualWidth())/Real(viewport_->getActualHeight()));
    
	// CEGUI Setup
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
	//world_->setInternalTickCallback(&Game::onTick, this, true);
	world_->setWorldUserInfo(this);
    btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher_);

}

/** Loads a script */
void Game::loadScript(const std::string& name) {
	lua_State* env = scriptState_;
	StackCheck check(env);
	if (luaL_dofile(env, name.c_str())) {
        string message(lua_tostring(env, -1));
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
	oscSender_ = new OscSender(OSC_SEND_PORT);
	oscListener_ = new OscBeatListener(OSC_LISTEN_PORT);
}

/** Called when the main window is closed */
void Game::windowClosed(RenderWindow* rw) { 
	root_->queueEndRendering();
}

/** Called when a frame begins */
bool Game::frameRenderingQueued(const FrameEvent& evt) { 

	physicsAccumulator_ += evt.timeSinceLastFrame;
	physicsAccumulator_ = min(physicsAccumulator_, MAX_TIME_LAG);
	while (physicsAccumulator_ >= getTimeStep()) {
		keyboard_->capture();
		mouse_->capture();

		if (keyboard_->isKeyDown(OIS::KC_T)) {
			world_->stepSimulation(getTimeStep() / 4.0f, 0);
		} else {
			world_->stepSimulation(getTimeStep(), 0);
		}

		// Check for collisions
		int nmanifolds = world_->getDispatcher()->getNumManifolds();
		for (int i = 0; i < nmanifolds; i++) {
			btPersistentManifold* manifold = world_->getDispatcher()->getManifoldByIndexInternal(i);
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

		std::list<GameListener*>::iterator i = listeners_.begin();
		while (i != listeners_.end()) {
			GameListener* listener = *i;
			i++;
			listener->onTimeStep();
		}
		physicsAccumulator_ -= getTimeStep();
	}

	// Hack hack hack
	if (getKeyboard()->isKeyDown(OIS::KC_R) && !resetEvent_) {
		resetEvent_ = true;
		std::cout << "Reset" << endl;
		setLevel("Level1");
    } else if (getKeyboard()->isKeyDown(OIS::KC_F) && !resetEvent_) {
		resetEvent_ = true;
		std::cout << "my Reset" << endl;
		setLevel("Level1");
	} else {
		resetEvent_ = false;
	}

	if (keyboard_->isKeyDown(OIS::KC_ESCAPE)) {
		root_->queueEndRendering();
	}

	return true;
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
	std::list<GameListener*>::iterator i = find(listeners_.begin(), listeners_.end(), listener);

	if (i != listeners_.end()) {
		listeners_.erase(i);
	}
}

void Game::setLevel(const string& name) {
	// Make sure the old level is released first!
	cout << "Setting level: " << name << endl;
	screen_.reset();
	level_.reset();
	level_.reset(new Level(this, name));
}	

void Game::setScreen(const string& name) {
	cout << "Setting screen: " << name << endl;
	level_.reset();
	screen_.reset();
	screen_.reset(new Screen(this, name));
}
