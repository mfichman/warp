/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Application.hpp>
#include <FrameListener.hpp>
#include <WindowListener.hpp>
#include <Bicycle.hpp>
#include <string>
#include <iostream>

using namespace Criterium;
using namespace Ogre;
using namespace std;

//------------------------------------------------------------------------------
Application::Ptr
Application::make() {
    return new Application();
}

//------------------------------------------------------------------------------
Application::Application() :
    root_(new Root("plugins.cfg", "ogre.cfg", "ogre.log")),
    camera_(0),
    sceneManager_(0),
    viewport_(0),
    window_(0),
    inputManager_(0),
    keyboard_(0),
    mouse_(0),
    guiRenderer_(0),
    guiSystem_(0),
    world_(0),
    space_(0) {
    
    loadResources();
    loadGraphics();
    loadInput();
    loadPhysics();

	bicycle_ = Bicycle::make(this);
	//tree_ = Tree::make(this);
}

//------------------------------------------------------------------------------
Application::~Application() {
    if (world_) {
        dWorldDestroy(world_);
    }
    
    if (space_) {
        dSpaceDestroy(space_);
    }


    if (inputManager_) { 
        if (keyboard_) inputManager_->destroyInputObject(keyboard_);
        if (mouse_) inputManager_->destroyInputObject(mouse_);
        OIS::InputManager::destroyInputSystem(inputManager_);
    }
}

//------------------------------------------------------------------------------
void
Application::loadResources() {
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

//------------------------------------------------------------------------------
void
Application::loadGraphics() {
    // Set up the render system
    // || !root_->restoreConfig()
    if (!root_->showConfigDialog()) {
        throw Exception(52, "User cancelled the config dialog.", "Application::application()");
    }
    
    // Create render window
    window_ = root_->initialise(true);

	// Create scene manager
    sceneManager_ = root_->createSceneManager(ST_GENERIC, "Default");
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
    guiRenderer_ = auto_ptr<CEGUI::OgreCEGUIRenderer>(new CEGUI::OgreCEGUIRenderer(window_, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, sceneManager_));
    guiSystem_ = auto_ptr<CEGUI::System>(new CEGUI::System(guiRenderer_.get()));
    
    frameListener_ = new FrameListener(this);
    windowListener_ = new WindowListener(this);

	
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//------------------------------------------------------------------------------
void
Application::loadInput() {
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
    } catch (const OIS::Exception& ex) {
        throw Exception(42, ex.eText, "Application::application()");
    }
}

//------------------------------------------------------------------------------
void
Application::loadPhysics() {
    world_ = dWorldCreate();
    space_ = dSimpleSpaceCreate(0);
    contactJointGroup_ = dJointGroupCreate(32);

    dGeomID plane = dCreatePlane(space_, 0.0, 1.0, 0.0, -0.742);
    dGeomSetCategoryBits(plane, TYPETERRAIN);
    dGeomSetCollideBits(plane, TYPEWHEEL);
	//dWorldSetGravity(world_, 0.0, -9.8, 0.0);   
}

//------------------------------------------------------------------------------
int
main(int argc, char** argv) {
    try {
        dInitODE();
        Application::Ptr app = Application::make(); 
        

		Plane plane(Vector3::UNIT_Y, -0.742);
		MeshManager::getSingleton().createPlane("Ground",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500, 1500, 200, 200, true, 1, 1500, 1500, Vector3::UNIT_Z);

		Light* light = app->sceneManager()->createLight("Light");
		light->setType(Light::LT_DIRECTIONAL);
		light->setDiffuseColour(ColourValue(0.75, 0.75, 0.75));
		light->setSpecularColour(ColourValue(0.75, 0.75, 0.75));
		light->setDirection(Vector3( 0, -1, 1 )); 


		app->sceneManager()->setAmbientLight(ColourValue(0.6f, 0.6f, 0.6f));

		Entity* entity = app->sceneManager()->createEntity("Ground", "Ground");
		entity->setMaterialName("Examples/Rockwall");
		entity->setCastShadows(false);
		app->sceneManager()->getRootSceneNode()->createChildSceneNode()->attachObject(entity);

		//Plane* plane = (Plane*)app->sceneManager()->createEntity("Plane", SceneManager::PT_PLANE);
		//plane->redefine(Vector3(0, 1, 0), Vector3(0, 0, 0));


		app->root()->startRendering();
        
    } catch (Exception& ex) {
        cerr << "Exception: " << ex.getFullDescription() << endl;
    }


    return 0;

}