/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include <LevelManager.hpp>

using namespace Warp;
using namespace Ogre;
using namespace std;
using namespace std::tr1;

struct LevelManager::Impl : public Game::Listener {
	
	/** Initializes the level manager */
	LevelManager(Game* game) : 
		game_(game) {


	}

	static int luaLoadLevel(lua_State* env) {
		
	}

	Game* game_;
	auto_ptr<Level> currentLevel_;
};

LevelManager::LevelManager(Game* game) {
}

LevelManager::~LevelManager() {
}