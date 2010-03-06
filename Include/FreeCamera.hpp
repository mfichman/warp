/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

namespace Warp {

class FreeCamera : public GameListener  {
public:
	/** Creates a new ball */
    FreeCamera(Game* game);

    /** Destructor */
    ~FreeCamera();

private:
	FreeCamera(const FreeCamera&);
    FreeCamera& operator=(const FreeCamera&);
	virtual void onTimeStep();

	Game* game_;
};

}