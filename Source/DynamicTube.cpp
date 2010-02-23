/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "DynamicTube.hpp"

#include "Game.hpp"

#include <Ogre.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

using namespace Warp;
using namespace Ogre;
using namespace std;

DynamicTube::DynamicTube(Game* game, const string& name) :
    curveStep_(5.0f),
    ringDivisions_(16),
    ringRadius_(10.0f),
    transform_(Matrix4::IDENTITY),
    v_(0),
    name_(name),
    manual_(game->getSceneManager()->createManualObject(name)),
	game_(game) {

    // Generate the mesh data
    manual_->begin("Road", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
    readInputFile();
    generateIndices();
    manual_->end();

    // Initialize the graphical rep
	SceneNode* node = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name_);
    manual_->setCastShadows(false);
    node->attachObject(manual_);

    data_.reset(new btTriangleIndexVertexArray(indices_.size()/3, &indices_.front(), 3*sizeof(int), vertices_.size(), (btScalar*)&vertices_.front(), sizeof(Vector3)));

    // Initialize the physics object
    position_.setIdentity();
    // shape_.reset(new btBvhTriangleMeshShape(data_.get(), true));
    shape_.reset(new btGImpactMeshShape(data_.get()));
    shape_->setMargin(0.0f);
    shape_->updateBound();
    object_.reset(new btCollisionObject());
    object_->setCollisionShape(shape_.get());
	object_->setFriction(0.0f);
	object_->setRestitution(0.0f);
    game_->getWorld()->addCollisionObject(object_.get());
}
   
DynamicTube::~DynamicTube() {
    // Destroy scene node
    game_->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name_);
	// Destroy manual object
    game_->getSceneManager()->destroyManualObject(name_);
    game_->getWorld()->removeCollisionObject(object_.get());
}

/** 
 * Reads the input file (ends with .tube).  The file can contain the
 * following directives:
 * straight <length>
 * curve <normal x> <normal y> <normal z> <angle> <radius>
 * radius <ring radius>
 * divisions <ring divisions>
 * step <curve angle step>
 */
void DynamicTube::readInputFile() {
    // Create the first ring
    generateStraight(0);

    // Read the input file
    std::ifstream in((name_).c_str());

    if (!in.is_open()) throw runtime_error("Unable to open tube file " + name_);
    while (!in.eof()) {
        std::string type;
        in >> type;
        
        if (type == "straight") {
            float length;
            in >> length;
            generateStraight(length);
        } else if (type == "curve") {
            float nx, ny, nz, angle, radius;
            in >> nx >> ny >> nz >> angle >> radius;
            angle = abs(angle);
            generateCurve(Vector3(nx, ny, nz), angle, radius);
        } else if (type == "radius") {
            in >> ringRadius_;
        } else if (type == "divisions") {
            in >> ringDivisions_;
        } else if (type == "step") {
            in >> curveStep_;
        }   

        if (in.fail()) break;
    }
}

/** Generates one ring around the current spine point using the transform_ */
void DynamicTube::generateRing() {

    // Generate the spine node
    Vector3 spinePosition = transform_ * Vector3::ZERO;

    if (nodes_.size() != 0) {
        v_ += lastSpinePosition_.distance(spinePosition);
    }
    lastSpinePosition_ = spinePosition;
    nodes_.push_back(spinePosition);
    
    // Generate the ring around the node
    for (int i = 0; i < ringDivisions_; i++) {
	    float theta = 360.0f/(ringDivisions_-1)*i + 45.0f;
        float u = (float)i/(float)ringDivisions_ * 2;
        Vector3 position(cosf(Math::PI/180.0*theta)*ringRadius_, sinf(Math::PI/180.0*theta)*ringRadius_, 0.0f);
	    Vector4 normalh(-position.normalisedCopy());
	    normalh.w = 0;

	    position = transform_ * position;
	    normalh = transform_ * normalh;

	    Vector3 normal(normalh.x, normalh.y, normalh.z);
		
        // Add vertex to the visible mesh
	    manual_->position(position);
        manual_->normal(normal);
        manual_->textureCoord(u, v_ / (2 * Math::PI * ringRadius_));

        // Add vertex to the physics tri mesh
        vertices_.push_back(position);
    }
}

/** Generates a straight tube segment */
void DynamicTube::generateStraight(float length) {
    transform_ = transform_.concatenate(Matrix4::getTrans(0, 0, length));
    generateRing();
}

/** Generates a curved tube segment */
void DynamicTube::generateCurve(const Vector3& normal, float angle, float radius) {
    Vector3 right = normal.crossProduct(Vector3::UNIT_Z);
	right.normalise();

    for (float t = 0; t < angle; t += curveStep_) {
        
        transform_ = transform_.concatenate(Matrix4::getTrans(radius * right));
	    transform_ = transform_.concatenate(Matrix4(Quaternion(Degree(curveStep_), normal)));
        transform_ = transform_.concatenate(Matrix4::getTrans(-radius * right));	
	   

	    //transform_ = transform_.concatenate(Matrix4::getTrans(radius * right));
	    //transform_ = transform_.concatenate(Matrix4(Quaternion(Degree(curveStep_), normal)));
        //transform_ = transform_.concatenate(Matrix4::getTrans(-radius * right));	
        generateRing();
    }
}

/** Generates the indices for the mesh */
void DynamicTube::generateIndices() {
    for (size_t i = 0; i < nodes_.size()-1; i++) {
	    for (int j = 0; j <= ringDivisions_; j++) {

            // Add index to the visible mesh
	        manual_->index(j % ringDivisions_ + i * ringDivisions_);
	        manual_->index(j % ringDivisions_ + (i+1) * ringDivisions_);


            // j == ring division num
            // i == ring num

            // Add indices to the physical mesh    

            indices_.push_back(j % ringDivisions_ + i * ringDivisions_);
            indices_.push_back(j % ringDivisions_ + (i+1) * ringDivisions_);
            indices_.push_back((j+1) % ringDivisions_ + i * ringDivisions_);

            indices_.push_back((j+1) % ringDivisions_ + i * ringDivisions_);
            indices_.push_back(j % ringDivisions_ + (i+1) * ringDivisions_);
            indices_.push_back((j+1) % ringDivisions_ + (i+1) * ringDivisions_);
	    }
    }
}

/** Returns the spine projection given the current node number */
SpineProjection DynamicTube::getSpineProjection(const Vector3& v, int node_i) const {

    const int n_nodes = nodes_.size();

    int closest_i = node_i;
    { /* find the closest node to v near node_i */
        float minDistance = nodes_[node_i].squaredDistance(v);
        // this loop will discover the local min in distance around node_i
        while(true) {
            // try the node in front and behind:
            float nextDistance = nodes_[mod(closest_i+1, n_nodes)].squaredDistance(v);
            if (nextDistance < minDistance) {
                minDistance = nextDistance;
                closest_i = mod(closest_i + 1, n_nodes);
                continue;
            }
            float prevDistance = nodes_[mod(closest_i-1, n_nodes)].squaredDistance(v);
            if (prevDistance < minDistance) {
                minDistance = prevDistance;
                closest_i = mod(closest_i - 1, n_nodes);
                continue;
            }
            break;
        }
    }

    int prev_i;
    int next_i;

    // get the previous and next node indexes
    Vector3 forward = (nodes_[mod(closest_i + 1, n_nodes)] - nodes_[closest_i]).normalisedCopy();
    float projected_v = forward.dotProduct(v - nodes_[closest_i]);
    if (projected_v > 0) {
        prev_i = closest_i;
        next_i = mod(closest_i + 1, n_nodes);
    } else {
        prev_i = mod(closest_i - 1, n_nodes);
        next_i = closest_i;
    }

    if (node_i != prev_i) { printf("NEW NODE: %d\n", prev_i); }

    // compute location and forward vectors
    Vector3 prev = nodes_[prev_i];
    Vector3 next = nodes_[next_i];
    Vector3 prevForward = (next - prev).normalisedCopy();
    Vector3 nextForward = (nodes_[mod(next_i + 1, n_nodes)] - next).normalisedCopy();

    // projection of relative position in the direction of motion
    // divided by distance to the next node
    float alpha = prevForward.dotProduct(v - prev)/next.distance(prev);

    // interpolate
    SpineProjection result;
    result.position = (1 - alpha)*prev + (alpha)*next;
    result.forward = (1 - alpha)*prevForward + (alpha)*nextForward;
    result.forward.normalise();
	result.index = prev_i;
    return result;
}
