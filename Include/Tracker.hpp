/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include "SharedObject.hpp"

namespace Warp {

class Tracker : public SharedObject {
public:
	virtual ~Tracker() {}

	virtual void onTargetDelete(ObjectPtr object)=0;
};

}