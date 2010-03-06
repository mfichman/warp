/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

namespace Warp {

/* 
 * This class uses dynamic dispatch to decide which version of onCollision()
 * gets called.
 */
class Collidable {
public:
	virtual ~Collidable() {}
};


}