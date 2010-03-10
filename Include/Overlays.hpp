/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

namespace Warp {

class Overlays : public GameListener  {
public:

	/** Creates a new set of overlays */
    Overlays(Game* game);

    /** Destructor */
    ~Overlays();

private:
	Overlays(const Overlays&);
    Overlays& operator=(const Overlays&);
	void onTimeStep();

	Game* game_;
	float speed_;
};

}