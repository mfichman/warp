/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "City.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"

#include <sstream>
#include <Ogre.h>
#include <OgreTerrainSceneManager.h>

#define BUILDING_MAX_HEIGHT 100.0f
#define BUILDING_MIN_HEIGHT 30.0f
#define BUILDING_FOOTPRINT 12.0f
#define CITY_RADIUS 70.0f

using namespace Ogre;
using namespace Warp;
using namespace std;

City::City(Game* game, Level* level, int id) :
	game_(game),
	level_(level),
	numVertices_(0),
	alive_(true) {

	SpineProjection spawn = level_->getPlayer()->getSpawnProjection(1000);

	ostringstream os;
	os << "City" << id;
	name_ = os.str();

	Vector3 right = spawn.forward.crossProduct(Vector3::UNIT_Y);
	spawn.position.y = 0;

	if (Math::RangeRandom(-1, 1) < 0) {
		spawn.position += Math::RangeRandom(80, 100)*right;
	} else {
		spawn.position -= Math::RangeRandom(80, 100)*right;
	}

	
	cout << "Created city at " << spawn.position << endl;

	node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name_);
	node_->setPosition(spawn.position);

	manual_ = game->getSceneManager()->createManualObject(name_);
	manual_->begin("City", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	manual_->setCastShadows(true);
	generateBuildings();
	manual_->end();

	node_->attachObject(manual_);	
}

City::~City() {
	game_->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name_);
	game_->getSceneManager()->destroyManualObject(name_);
}

void City::generateBuildings() {

	for (int i = 0; i < 15; i++) {
		float theta = Math::RangeRandom(0, 2*Math::PI);
		float radius = Math::RangeRandom(0, CITY_RADIUS);
		float x = radius*cosf(theta);
		float z = radius*sinf(theta);
		generateBuilding(x, z);
	}
}

void City::generateBuilding(float x, float z) {
	float height = 2* Math::RangeRandom(BUILDING_MIN_HEIGHT, BUILDING_MAX_HEIGHT)/pow(x*x + z*z, 0.4f);
	height = min(height, BUILDING_MAX_HEIGHT);

	float min = BUILDING_FOOTPRINT/3.0;
	float max = BUILDING_FOOTPRINT/2.0;
	float edge = -Math::RangeRandom(min, max);
	float topedge = edge;// + 2.0f;

	float v = 1.0f; //height/(2*edge);
	float u = (2*edge)/height;

	typedef float texcoord_t[2];
	texcoord_t texcoords[30] = {
		{ 0, 0 },
		{ 0, v },
		{ u, 0 },
		{ 0, v },
		{ u, v },
		{ u, 0 },

		{ 0, 0 },
		{ u, 0 },
		{ 0, v },
		{ 0, v },
		{ u, 0 },
		{ u, v },

		{ 0, 0 },
		{ 0, v },
		{ u, 0 },
		{ 0, v },
		{ u, v },
		{ u, 0 },

		{ 0, 0 },
		{ u, 0 },
		{ 0, v },
		{ 0, v },
		{ u, 0 },
		{ u, v },

		{ 0, 0 },
		{ 1, 1 },
		{ 1, 0 },
		{ 0, 1 },
		{ 1, 1 },
		{ 0, 0 }
	};

	Vector3 positions[30] = { 
		Vector3(x-edge, 0, z-edge),
		Vector3(x-edge, height, z-topedge),
		Vector3(x+edge, 0, z-edge),
		Vector3(x-edge, height, z-topedge),
		Vector3(x+edge, height, z-topedge),
		Vector3(x+edge, 0, z-edge),

		Vector3(x-edge, 0, z+edge),
		Vector3(x+edge, 0, z+edge),
		Vector3(x-edge, height, z+topedge),
		Vector3(x-edge, height, z+topedge),
		Vector3(x+edge, 0, z+edge),
		Vector3(x+edge, height, z+topedge),

		Vector3(x+edge, 0, z-edge),
		Vector3(x+edge, height, z-topedge),
		Vector3(x+edge, 0, z+edge),
		Vector3(x+edge, height, z-topedge),
		Vector3(x+edge, height, z+topedge),
		Vector3(x+edge, 0, z+edge),

		Vector3(x-edge, 0, z-edge),
		Vector3(x-edge, 0, z+edge),
		Vector3(x-edge, height, z-topedge),
		Vector3(x-edge, height, z-topedge),
		Vector3(x-edge, 0, z+edge),
		Vector3(x-edge, height, z+topedge),

		Vector3(x-topedge, height, z-topedge),
		Vector3(x+topedge, height, z+topedge),
		Vector3(x+topedge, height, z-topedge),
		Vector3(x-topedge, height, z+topedge),
		Vector3(x+topedge, height, z+topedge),
		Vector3(x-topedge, height, z-topedge)
	};

	Vector3 normals[5] = {
		Vector3(0, 0, -1),
		Vector3(0, 0, 1),
		Vector3(1, 0, 0),
		Vector3(-1, 0, 0),
		Vector3(0, 1, 0)
	};

	TerrainSceneManager* manager = static_cast<TerrainSceneManager*>(game_->getSceneManager());

	float heights[4] = {
		manager->getHeightAt(x-edge+node_->getPosition().x, z-edge+node_->getPosition().z),
		manager->getHeightAt(x-edge+node_->getPosition().x, z+edge+node_->getPosition().z),
		manager->getHeightAt(x+edge+node_->getPosition().x, z+edge+node_->getPosition().z),
		manager->getHeightAt(x+edge+node_->getPosition().x, z-edge+node_->getPosition().z)
	};

	float heightOffset = FLT_MAX;
	for (int i = 0; i < 4; i++) {
		if (heights[i] < heightOffset) {
			heightOffset = heights[i];
		}
	}

	for (int i = 0; i < 30; i++) {
		manual_->position(positions[i] + Vector3(0, heightOffset, 0));
		manual_->textureCoord(texcoords[i][0], texcoords[i][1]);
		manual_->normal(normals[i/6]);
	}
	for (int i = 0; i < 30; i++) {
		manual_->index(numVertices_++);
	}
}

void City::onTimeStep() {

	const SpineProjection proj = level_->getPlayer()->getPlayerProjection();
	Vector3 to = proj.position - node_->getPosition();

	// If the players forward vector is facing away from us, then the object is
	// out of the field of view so this object should be deleted.  N.B.:
	// there is probably a better way to do this using the distance along the
	// spine node path.
	if (to.dotProduct(proj.forward) > 0 && to.squaredLength() > CITY_RADIUS) {
		alive_ = false;
	}
}