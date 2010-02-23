/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Enemy.hpp"

#include "Game.hpp"

using namespace Warp;
using namespace Ogre;
using namespace std;

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Enemy::Enemy(Game* game, const string& name) :
    game_(game)
{
    game_->addListener(this);

}

Enemy::~Enemy() {
    game_->removeListener(this);
}

/** Called when the game updates */
void Enemy::onTimeStep() {

}