/******************************************************************************
 * Warp: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once

#include <memory>
#include <Game.hpp>

namespace Warp {

class FreeCamera  {
public:
	struct Impl;

	/** Creates a new ball */
    FreeCamera(Game* game);

    /** Destructor */
    ~FreeCamera();

private:
    std::auto_ptr<Impl> impl_;
};

}