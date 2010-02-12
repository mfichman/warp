/******************************************************************************
 * Warp: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <PickingRay.hpp>
#include <Game.hpp>
#include <Objects.hpp>

using namespace Warp;
using namespace Ogre;
using namespace std;

struct PickingRay::Impl : public Game::Listener {

    void init() {

    }

    void onTimeStep() {

        // Create the picking ray

    }


    Game* game_;
};

PickingRay::PickingRay(Game* game) : impl_(new Impl()) {
    impl_->game_ = game;
    impl_->game_->addListener(impl_.get());
    impl_->init();
}

PickingRay::~PickingRay() {

}