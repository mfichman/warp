/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

#include <Bullet/btBulletDynamicsCommon.h>

namespace Warp {

class DynamicTube {
public:
	/** Initializes the tube from the tube file */
	DynamicTube(Game* game, const std::string& name);

    /** Destructor, destroys the tube */
    ~DynamicTube();

	/** Returns the spine projection given the current node number */
    SpineProjection getSpineProjection(const Ogre::Vector3& v, int node_i) const;

	/** Returns the spine projection given the current node number & the distance value */
	SpineProjection getSpineProjection(float distance, int node_i) const;

private:
    DynamicTube(const DynamicTube&);
    DynamicTube& operator=(const DynamicTube&);

	void readInputFile();
	void generateRing();
	void generateStraight(float length);
	void generateCurve(const Ogre::Vector3& normal, float angle, float radius);
	void generateIndices();


	// These parameters are exclusively for vertex generation
	Ogre::Matrix4 transform_;
    std::string name_;
    float v_;
	Ogre::Vector3 lastSpinePosition_;
    float curveStep_;
    float ringRadius_;
    int ringDivisions_;
	bool tubeVisible_;

    // These variables are for the loaded scene objects
    Game* game_;
	Ogre::ManualObject* manual_;
	std::auto_ptr<btTriangleIndexVertexArray> data_;
    std::vector<Ogre::Vector3> vertices_;
	std::vector<int> indices_;

	// Physics collision detection
	std::auto_ptr<btGImpactMeshShape> shape_;
	std::auto_ptr<btCollisionObject> object_;
    btTransform position_;

	// Spine node tracking
	std::vector<SpineNode> nodes_;
};

}
