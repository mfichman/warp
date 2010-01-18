/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Application.hpp>
#include <WindowListener.hpp>

using namespace Criterium;
using namespace std;

//------------------------------------------------------------------------------
WindowListener::WindowListener(Application* app) : app_(app) {
    Ogre::WindowEventUtilities::addWindowEventListener(app_->window(), this);
}

//------------------------------------------------------------------------------
WindowListener::~WindowListener() {
    Ogre::WindowEventUtilities::removeWindowEventListener(app_->window(), this);
}

//------------------------------------------------------------------------------
void
WindowListener::windowResized(Ogre::RenderWindow* rw) {

}

//------------------------------------------------------------------------------
void
WindowListener::windowClosed(Ogre::RenderWindow* rw) {
    if (rw = app_->window()) {
        app_->root()->queueEndRendering();
    }
}
