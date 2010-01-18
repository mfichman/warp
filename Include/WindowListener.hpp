/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Ogre.h>

namespace Criterium {
class Application;

class WindowListener : public Interface, public Ogre::WindowEventListener {
public:
    friend class Application;
    ~WindowListener();
    virtual void windowResized(Ogre::RenderWindow* rw);
    virtual void windowClosed(Ogre::RenderWindow* rw);
    
private:
    WindowListener(Application* app);
    Application* app_;
};

}
