/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Level.hpp"

#include "DynamicTube.hpp"
#include "Player.hpp"
#include "Script.hpp"
#include "Game.hpp"

#include <string>

using namespace Warp;
using namespace Ogre;
using namespace std;

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Level::Level(Game* game, const std::string& name) :
    game_(game),
    tube_(new DynamicTube(game, "Levels/" + name + ".tube")),
    script_(new Script(game, "Scripts/" + name + ".lua"))
{
	player_.reset(new Player(game, this, "Ball"));
    game_->addListener(this);
}

Level::~Level() {
	game_->removeListener(this);
}

/** Called once for each game loop */
void Level::onTimeStep() {

}
