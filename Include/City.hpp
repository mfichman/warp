/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include "SharedObject.hpp"

namespace Warp {

class City : public SharedObject {
public:

	//! Creates a new city
	City(Game* game, Level* level, int id);

	//! Destroys the city
	virtual ~City();

	//! Called each timestep
	void onTimeStep();

	//! True if alive
	bool isAlive() { return alive_; }

private:
	void generateBuildings();
	void generateBuilding(float x, float y);
	void generateIndices();

	Game* game_;
	Level* level_;
	Ogre::SceneNode* node_;
	Ogre::ManualObject* manual_;
	std::string name_;
	size_t numVertices_;
	bool alive_;
};

}