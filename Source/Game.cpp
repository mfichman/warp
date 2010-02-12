/******************************************************************************
 * Warp: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Game.hpp>
#include <Objects.hpp>
#include <Script.hpp>

#include <OgreCEGUIRenderer.h>
#include <CEGUI/CEGUI.h>
extern "C" {
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


struct Game::Impl : public Ogre::WindowEventListener, Ogre::FrameListener {

	Impl() : 
		guiRenderer_(0), 
		guiSystem_(0), 
		root_(0), 
		inputManager_(0),
		keyboard_(0),
		mouse_(0),
        scriptState_(0) {
	}

	/** Destroys subsystems */
	~Impl() {
		if (guiRenderer_) { delete guiRenderer_;	}
		if (guiSystem_) { delete guiSystem_; }
		if (root_) { delete root_; }
		if (inputManager_) { 
			if (keyboard_) inputManager_->destroyInputObject(keyboard_);
			if (mouse_) inputManager_->destroyInputObject(mouse_);
			OIS::InputManager::destroyInputSystem(inputManager_);
		}
        if (scriptState_) { lua_close(scriptState_); }
	}

	/** Loads resource configuration files */
	void loadResources() {
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
	void loadGraphics() {
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
		camera_->setNearClipDistance(0.1);

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
	void loadInput() {
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
	void loadPhysics() {

	}

    /** Loads the scripting engine */
    void loadScripting() {
        scriptState_ = lua_open();
        luaL_openlibs(scriptState_);

        lua_pushlightuserdata(scriptState_, this); // Modifies an entity 
        lua_pushcclosure(scriptState_, &Impl::luaGetNode, 1);
        lua_setglobal(scriptState_, "crGetNode");

        lua_pushlightuserdata(scriptState_, this); // Modifies an entity 
        lua_pushcclosure(scriptState_, &Impl::luaSetNode, 1);
        lua_setglobal(scriptState_, "crSetNode");

        lua_pushlightuserdata(scriptState_, this); // Modifies a light
        lua_pushcclosure(scriptState_, &Impl::luaGetLight, 1);
        lua_setglobal(scriptState_, "crGetLight");

        lua_pushlightuserdata(scriptState_, this); // Modifies a light
        lua_pushcclosure(scriptState_, &Impl::luaSetLight, 1);
        lua_setglobal(scriptState_, "crSetLight");

        lua_pushlightuserdata(scriptState_, this); 
        lua_pushinteger(scriptState_, Objects::TYPE_BALL);
        lua_pushcclosure(scriptState_, &Impl::luaCreateObject, 2);
        lua_setglobal(scriptState_, "crCreateBall");

        lua_pushlightuserdata(scriptState_, this); 
        lua_pushinteger(scriptState_, Objects::TYPE_PLANE);
        lua_pushcclosure(scriptState_, &Impl::luaCreateObject, 2);
        lua_setglobal(scriptState_, "crCreatePlane");
    }

	/** Called when the main window is closed */
	void windowClosed(Ogre::RenderWindow* rw) { 
		root_->queueEndRendering();
	}

	/** Called when a frame begins */
	bool frameRenderingQueued(const Ogre::FrameEvent& evt) { 
		keyboard_->capture();
		mouse_->capture();
		

		if (keyboard_->isKeyDown(OIS::KC_ESCAPE)) {
			root_->queueEndRendering();
		}
	   
        list<Listener*>::iterator i = listeners_.begin();
        while (i != listeners_.end()) {
            Listener* listener = *i;
            i++;
            listener->onTimeStep();
        }

		return true;
	}

    /** Lua callback.  Gets the given values for the node */
    static int luaGetNode(lua_State* env) {
        Impl* impl = (Impl*)lua_touserdata(env, lua_upvalueindex(1));

        if (!lua_isstring(env, 1)) {
            lua_pushstring(env, "Expected string for node name");
            lua_error(env);
        }
        string name(lua_tostring(env, 1)); // First argument: name of the entity
        if (!impl->sceneManager_->hasSceneNode(name)) {
            lua_pushstring(env, "Invalid node name");
            lua_error(env);
        }
        SceneNode* node = impl->sceneManager_->getSceneNode(name);
        env << *node;

        return 1;
    }

    /** Lua callback.  Gets the given values for the node */
    static int luaSetNode(lua_State* env) {
        Impl* impl = (Impl*)lua_touserdata(env, lua_upvalueindex(1));

        if (!lua_isstring(env, 1)) {
            lua_pushstring(env, "Expected string for node name");
            lua_error(env);
        }
        string name(lua_tostring(env, 1)); // First argument: name of the entity
        if (!impl->sceneManager_->hasEntity(name)) {
            lua_pushstring(env, "Invalid node name");
            lua_error(env);
        }
        SceneNode* node = impl->sceneManager_->getSceneNode(name);
        env >> *node;

        return 1;
    }

    /** Lua callback.  Get the light state */
    static int luaGetLight(lua_State* env) {
        Impl* impl = (Impl*)lua_touserdata(env, lua_upvalueindex(1));
        if (!lua_isstring(env, 1)) {
            lua_pushstring(env, "Expected string for entity name");
            lua_error(env);
        }       
        string name(lua_tostring(env, 1)); // First argument: name of the light
        if (!impl->sceneManager_->hasLight(name)) {
            lua_pushstring(env, "Invalid entity name");
            lua_error(env);
        }
        Light* light = impl->sceneManager_->getLight(name);
        env << *light;

        return 1;
    }

    /** Lua callback.  Sets the given values for the light */
    static int luaSetLight(lua_State* env) {
        Impl* impl = (Impl*)lua_touserdata(env, lua_upvalueindex(1));
        if (!lua_isstring(env, 1)) {
            lua_pushstring(env, "Expected string for entity name");
            lua_error(env);
        }       
        string name(lua_tostring(env, 1)); // First argument: name of the light
        if (!impl->sceneManager_->hasLight(name)) {
            lua_pushstring(env, "Invalid entity name");
            lua_error(env);
        }
        Light* light = impl->sceneManager_->getLight(name);
        env >> *light;
        
        return 0;
    }

    /** Lua callback.  Creates an object */
    static int luaCreateObject(lua_State* env) {
        Impl* impl = (Impl*)lua_touserdata(env, lua_upvalueindex(1));
        Objects::Type type = (Objects::Type)lua_tointeger(env, lua_upvalueindex(2));
        
        string name = lua_isstring(env, 1) ? lua_tostring(env, 1) : "Object";
        int reference = lua_ref(env, LUA_REGISTRYINDEX);

        switch (type) {
            case Objects::TYPE_BALL: impl->objects_->createBall(name, reference); break;
            case Objects::TYPE_PLANE: impl->objects_->createPlane(name, reference); break;
        }

        return 0;
    }

	// Graphics objects
    Ogre::Root* root_;
    Ogre::Camera* camera_;
    Ogre::SceneManager*	sceneManager_;
    Ogre::Viewport*	viewport_;
    Ogre::RenderWindow*	window_;
    
	// GUI objects
    CEGUI::OgreCEGUIRenderer* guiRenderer_;
    CEGUI::System* guiSystem_;
    
	// Input objects
    OIS::InputManager* inputManager_;
    OIS::Keyboard* keyboard_;
    OIS::Mouse*	mouse_;
    
	// Physics objects

    
	// Game objects
	list<Listener*> listeners_;

    // Scripting objects
    lua_State* scriptState_;

    auto_ptr<Objects> objects_;
    auto_ptr<Overlays> overlays_;
};

Game::Game() : impl_(new Impl()) {
	impl_->root_ = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
	impl_->loadResources();
	impl_->loadGraphics();
	impl_->loadInput();
	impl_->loadPhysics();
    impl_->loadScripting();
    impl_->objects_.reset(new Objects(this));
    impl_->overlays_.reset(new Overlays(this));
}

Game::~Game() {

}

Objects* Game::getObjects() const {
    return impl_->objects_.get();
}

Overlays* Game::getOverlays() const {
    return impl_->overlays_.get();
}

OIS::Keyboard* Game::getKeyboard() const { 
	return impl_->keyboard_; 
}

OIS::Mouse*	Game::getMouse() const { 
	return impl_->mouse_; 
}

Ogre::Root* Game::getRoot() const { 
	return impl_->root_; 
}

Ogre::Camera* Game::getCamera() const { 
	return impl_->camera_; 

}
Ogre::SceneManager*	Game::getSceneManager() const { 
	return impl_->sceneManager_; 
}

Ogre::RenderWindow*	Game::getWindow() const { 
	return impl_->window_; 
}

lua_State* Game::getScriptState() const {
    return impl_->scriptState_;
}

float Game::getMouseNormalizedX() const {
	unsigned int width, height, depth;
	int top, left;
	unsigned int x = impl_->mouse_->getMouseState().X.abs;
	impl_->window_->getMetrics(width, height, depth, left, top);
    return (x - width/2.0f)/(width/2.0f);

}

float Game::getMouseNormalizedY() const {
	unsigned int width, height, depth;
	int top, left;
	unsigned int y = impl_->mouse_->getMouseState().Y.abs;
	impl_->window_->getMetrics(width, height, depth, left, top);
    return (y - height/2.0f)/(height/2.0f);
}

void Game::addListener(Listener* listener) { 
	impl_->listeners_.push_back(listener);
}

void Game::removeListener(Listener* listener) { 
	list<Listener*>::iterator i = find(impl_->listeners_.begin(), impl_->listeners_.end(), listener);

	if (i != impl_->listeners_.end()) {
		impl_->listeners_.erase(i);
	}
}
