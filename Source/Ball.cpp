/******************************************************************************
 * Warp: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Ball.hpp>
#include <Objects.hpp>

using namespace Warp;
using namespace Ogre;
using namespace std;

#define BALLRADIUS 0.5f // meters
#define BALLMASS 1.0f // kilograms

Ogre::Vector3 last = Ogre::Vector3(0.0f, 0.0f, -3.0f);

struct Ball::Impl : public Game::Listener {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init() {

		
		// Set up OGRE scene nodes
		SceneNode* root = game_->getSceneManager()->getRootSceneNode();
		SceneNode* node = root->createChildSceneNode("Ball");
		node->attachObject(game_->getSceneManager()->createEntity("Ball", "Ball.mesh"));

	}

    

	/** Called when a new frame is detected */
	void onTimeStep() {

		
	}

	Game* game_;
};

Ball::Ball(Game* game) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->init();
	game->addListener(impl_.get());
}

Ball::~Ball() {
}