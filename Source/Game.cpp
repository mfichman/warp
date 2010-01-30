/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Game.hpp>
#include <Bicycle.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <utility>
#include <OgreCEGUIRenderer.h>
#include <CEGUI/CEGUI.h>

using namespace Criterium;
using namespace Ogre;
using namespace std;

#define PHYSICSUPDATEINTERVAL	0.01f // seconds
#define PHYSICSMAXINTERVAL		0.25f // seconds


struct Game::Impl : public Ogre::WindowEventListener, Ogre::FrameListener {

	Impl() : 
		guiRenderer_(0), 
		guiSystem_(0), 
		root_(0), 
		world_(0), 
		space_(0), 
		inputManager_(0),
		keyboard_(0),
		mouse_(0) {
	}

	/** Destroys subsystems */
	~Impl() {
		objects_.clear();
		if (guiRenderer_) { delete guiRenderer_;	}
		if (guiSystem_) { delete guiSystem_; }
		if (root_) { delete root_; }
		if (world_) { dWorldDestroy(world_); }
		if (space_) { dSpaceDestroy(space_); }
		if (inputManager_) { 
			if (keyboard_) inputManager_->destroyInputObject(keyboard_);
			if (mouse_) inputManager_->destroyInputObject(mouse_);
			OIS::InputManager::destroyInputSystem(inputManager_);
		}
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
		//sceneManager_ = root_->createSceneManager("PagingLandScapeSceneManager", "Default");
		sceneManager_ = root_->createSceneManager(ST_EXTERIOR_CLOSE, "Default");
		sceneManager_->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);

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
		world_ = dWorldCreate();
		space_ = dSimpleSpaceCreate(0);
		contactJointGroup_ = dJointGroupCreate(32);

		dGeomID plane = dCreatePlane(space_, 0.0, 1.0, 0.0, -0.742);
		dGeomSetCategoryBits(plane, TYPETERRAIN);
		dGeomSetCollideBits(plane, TYPEWHEEL);
		dWorldSetGravity(world_, 0.0, -9.8, 0.0);   
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
	    
		// Prevent physics from running while game doesn't have focus
		physicsAccumulator_ += evt.timeSinceLastFrame;
		physicsAccumulator_ = std::min(physicsAccumulator_, PHYSICSMAXINTERVAL); 

			//	const OIS::MouseState& state = mouse_->getMouseState();
			//camera_->pitch(Radian(-state.Y.rel/100.0));
			//camera_->yaw(Radian(-state.X.rel/100.0));

		// Run fixed time steps using time in accumulator
		while (physicsAccumulator_ >= PHYSICSUPDATEINTERVAL) { 
	        
			// Run collision detection 
			dSpaceCollide(space_, this, &Impl::onGeomCollision);
	        
			// Step world
			dWorldStep(world_, PHYSICSUPDATEINTERVAL);
	        
			// Clean up
			dJointGroupEmpty(contactJointGroup_);
			physicsAccumulator_ -= PHYSICSUPDATEINTERVAL;

			for_each(listeners_.begin(), listeners_.end(), mem_fun(&Listener::onTimeStep));


			
			//if (keyboard_->isKeyDown(OIS::KC_UP)) {
			//	camera_->moveRelative(Vector3(0.0, 0.0, -0.5));
			//}
		}
		return true;
	}

	/** Called when to geometric objects collide */
	static void onGeomCollision(void* data, dGeomID o1, dGeomID o2) {
		Impl* impl = static_cast<Impl*>(data);

		dContactGeom geom[10];
		int num = dCollide(o1, o2, 10, geom, sizeof(dContactGeom));
		if (num <= 0) return;
	    
		// Look up the surface params for these object types
		int t1 = dGeomGetCategoryBits(o1);
		int t2 = dGeomGetCategoryBits(o2);
	    
		for (int i = 0; i < num; i++) {
			dContact contact;
			memset(&contact, 0, sizeof(dContact));
			contact.geom = geom[i];
	        
			contact.surface.mode = dContactApprox1;
			contact.surface.mu = 0.0;
	        
			// Add a contact joint
			dJointID joint = dJointCreateContact(impl->world_, impl->contactJointGroup_, &contact);
			dJointAttach(joint, dGeomGetBody(o1), dGeomGetBody(o2));
		}
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
	float physicsAccumulator_;
    dWorldID world_;
    dSpaceID space_;
    dJointGroupID contactJointGroup_;
    
	// Game objects
	list<Interface::Ptr> objects_;
	list<Listener*> listeners_;
};

Game::Game() : impl_(new Impl()) {
	impl_->root_ = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
	impl_->physicsAccumulator_ = 0;
	impl_->loadResources();
	impl_->loadGraphics();
	impl_->loadInput();
	impl_->loadPhysics();
	impl_->objects_.push_back(new Bicycle(this));
}

OIS::Keyboard* Game::getKeyboard() const { 
	return impl_->keyboard_; 
}

OIS::Mouse*	Game::getMouse() const { 
	return impl_->mouse_; 
}

dWorldID Game::getWorld() const { 
	return impl_->world_; 
}

dSpaceID Game::getSpace() const { 
	return impl_->space_; 
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

void Game::addListener(Listener* listener) { 
	impl_->listeners_.push_back(listener);
}

void Game::removeListener(Listener* listener) { 
	list<Listener*>::iterator i = find(impl_->listeners_.begin(), impl_->listeners_.end(), listener);

	if (i != impl_->listeners_.end()) {
		impl_->listeners_.erase(i);
	}
}
