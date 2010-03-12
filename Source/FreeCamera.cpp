/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "FreeCamera.hpp"

#include "Game.hpp"

#include <OIS/OIS.h>

using namespace Warp;
using namespace Ogre;
using namespace std;

FreeCamera::FreeCamera(Game* game) :
	game_(game) {

	game_->addListener(this);
}

FreeCamera::~FreeCamera() {
	game_->removeListener(this);
}

/** Called when a new frame is started */
void FreeCamera::onTimeStep() {

    // Moves the camera around
    const OIS::MouseState& state = game_->getMouse()->getMouseState();
    game_->getCamera()->pitch(Radian(-state.Y.rel/100.0));
    game_->getCamera()->yaw(Radian(-state.X.rel/100.0));

    if (game_->getKeyboard()->isKeyDown(OIS::KC_RSHIFT)) {
	    if (game_->getKeyboard()->isKeyDown(OIS::KC_UP)) {
		    game_->getCamera()->moveRelative(Vector3(0.0, 0.0, -50));
	    } 
		if (game_->getKeyboard()->isKeyDown(OIS::KC_DOWN)) {
		    game_->getCamera()->moveRelative(Vector3(0.0, 0.0, 50));
	    }
    } else {
	    if (game_->getKeyboard()->isKeyDown(OIS::KC_UP)) {
		    game_->getCamera()->moveRelative(Vector3(0.0, 0.0, -1));
	    } 
		if (game_->getKeyboard()->isKeyDown(OIS::KC_DOWN)) {
		    game_->getCamera()->moveRelative(Vector3(0.0, 0.0, 1));
	    }
		if (game_->getKeyboard()->isKeyDown(OIS::KC_RIGHT)) {
		    game_->getCamera()->moveRelative(Vector3(1.0, 0.0, 0.0));
	    } 
		if (game_->getKeyboard()->isKeyDown(OIS::KC_LEFT)) {
		    game_->getCamera()->moveRelative(Vector3(-1.0, 0.0, 0.0));
	    }
    }
}
