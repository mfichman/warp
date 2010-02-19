/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include <Level.hpp>
#include <DynamicTube.hpp>
#include <Objects.hpp>
#include <Script.hpp>
#include <list>
#include <map>
#include <memory>

using namespace Warp;
using namespace Ogre;
using namespace std;
using namespace std::tr1;

struct Level::Impl : public Game::Listener {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
    Impl(Game* game, const std::string& name) :
        game_(game),
        tube_(new DynamicTube(game, "Levels/" + name + ".tube")),
        script_(new Script(game, "Scripts/" + name + ".lua")) {

        game_->addListener(this);
    }

    ~Impl() {
        game_->removeListener(this);
    }

    /** Called when the game updates */
    void onTimeStep() {

    }

    Game* game_;
    auto_ptr<DynamicTube> tube_;
    auto_ptr<Script> script_;

    //map<string, shared_ptr<Enemy>> enemies_;
};

Level::Level(Game* game, const std::string& name) : impl_(new Impl(game, name)) {
}

Level::~Level() {
}
