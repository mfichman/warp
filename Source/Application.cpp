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
    root_->initialise(true, "Criterium");
    
    // Initialize resources
    TextureManager::getSingleton().setDefaultNumMipmaps(5);
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    
    // Create scene manager
    sceneManager_ = root_->createSceneManager(ST_EXTERIOR_CLOSE, "Default");
    camera_ = sceneManager_->createCamera("Camera");
    camera_->setPosition(Vector3(0, 0, 5));
    camera_->lookAt(Vector3(0, 0, 0));
    camera_->setNearClipDistance(0.1);
    
    window_ = root_->getAutoCreatedWindow();
    viewport_ = window_->addViewport(camera_);
    
    // CEGUI Setup
    guiRenderer_ = auto_ptr<CEGUI::OgreCEGUIRenderer>(new CEGUI::OgreCEGUIRenderer(window_, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, sceneManager_));
    guiSystem_ = auto_ptr<CEGUI::System>(new CEGUI::System(guiRenderer_.get()));
    
    frameListener_ = new FrameListener(this);
    windowListener_ = new WindowListener(this);
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
}

//------------------------------------------------------------------------------
int
main(int argc, char** argv) {
    try {
        dInitODE();
        Application::Ptr app = Application::make(); 
        app->sceneManager()->setAmbientLight(ColourValue(1, 1, 1));
        app->sceneManager()->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
        
        dSurfaceParameters surface;
        
        memset(&surface, 0, sizeof(dSurfaceParameters));
        app->surfaceParameters(surface, TYPEWHEEL, TYPETERRAIN);        
        
        Bicycle::Ptr bicycle = Bicycle::make(app.get());
        
        app->bicycle(bicycle);
        
        app->root()->startRendering();
        
    } catch (Exception& ex) {
        cerr << "Exception: " << ex.getFullDescription() << endl;
    }


    return 0;

}

//------------------------------------------------------------------------------
const dSurfaceParameters& 
Application::surfaceParameters(int type1, int type2) {
    pair<int, int> key = (type1 < type2) ? make_pair(type1, type2) : make_pair(type2, type1);    
    return surfaceParameters_[key];
}

//------------------------------------------------------------------------------
void 
Application::surfaceParameters(const dSurfaceParameters& c, int type1, int type2) {
    pair<int, int> key = (type1 < type2) ? make_pair(type1, type2) : make_pair(type2, type1);
    surfaceParameters_.insert(make_pair(key, c));
}



