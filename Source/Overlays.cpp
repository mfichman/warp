/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Overlays.hpp>
#include <CEGUI/CEGUI.h>

using namespace Criterium;
using namespace Ogre;
using namespace std;

static String currFps = "Current FPS: ";
static String avgFps = "Average FPS: ";
static String bestFps = "Best FPS: ";
static String worstFps = "Worst FPS: ";
static String tris = "Triangle Count: ";
static String batches = "Batch Count: ";

struct Overlays::Impl : public Game::Listener {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init() {
		debug_ = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
		debug_->show();
		
	}

	/** Called when a new frame is detected */
	void onTimeStep() {

		OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats& stats = game_->getWindow()->getStatistics();
		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

		OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));
		OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
		guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));
		OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		//guiDbg->setCaption(mDebugText);
	}

	Game* game_;
	Ogre::Overlay* debug_;

};

Overlays::Overlays(Game* game) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->init();
	game->addListener(impl_.get());
}