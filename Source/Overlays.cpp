/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Overlays.hpp"

#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"

using namespace Warp;
using namespace Ogre;
using namespace std;

static String currFps = "Current FPS: ";
static String avgFps = "Average FPS: ";
static String bestFps = "Best FPS: ";
static String worstFps = "Worst FPS: ";
static String tris = "Triangle Count: ";
static String batches = "Batch Count: ";

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Overlays::Overlays(Game* game) : 
	game_(game),
	speed_(0.0f) {

    game_->addListener(this);

    // The overlays are created via script, we just reference it here
	Overlay* debug = OverlayManager::getSingleton().getByName("Warp/Debug");
	debug->show();

	Overlay* hud = OverlayManager::getSingleton().getByName("Warp/HUD");
	hud->show();
}

/** Destroys the overlays and hides them */
Overlays::~Overlays() {
	Overlay* go = OverlayManager::getSingleton().getByName("Warp/GameOver");
	go->hide();
	game_->removeListener(this);
}

void Overlays::setShowGameOver(bool show) {
	Overlay* go = OverlayManager::getSingleton().getByName("Warp/GameOver");
	if (show) {
		go->show();
	} else {
		go->hide();
	}
}

/** Called when a new frame is detected */
void Overlays::onTimeStep() {
	

	const RenderTarget::FrameStats& stats = game_->getWindow()->getStatistics();
	OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Warp/AverageFps");
	guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
	OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Warp/NumTris");
	guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));
	OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Warp/NumBatches");
	guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));
	
	float shields = game_->getLevel()->getPlayer()->getShields();
	float speed = game_->getLevel()->getPlayer()->getVelocity().length();
	int points = game_->getLevel()->getPlayer()->getPoints();
	speed_ = 0.6f * speed_ + 0.4f * speed;

	OverlayElement* guiShields = OverlayManager::getSingleton().getOverlayElement("Warp/Shields");
	guiShields->setCaption("Shields: " + StringConverter::toString(shields));
	OverlayElement* guiSpeed = OverlayManager::getSingleton().getOverlayElement("Warp/Speed");
	guiSpeed->setCaption("Speed: " + StringConverter::toString(speed_, 2, 0) + " m/s");
	OverlayElement* guiPoints = OverlayManager::getSingleton().getOverlayElement("Warp/Points");
	guiPoints->setCaption("Points: " + StringConverter::toString(points));

	//guiDbg->setCaption(mDebugText);
}
