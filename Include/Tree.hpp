/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <Interface.hpp>
#include <Ogre.h>

namespace Criterium {
using namespace std;
using namespace boost;

class Application;

class Tree : public Interface {
public:
    typedef intrusive_ptr<Tree> Ptr;
    
    ~Tree();
    
    static Tree::Ptr make(Application* app);

private:
    Tree(Application* app);
};

}
