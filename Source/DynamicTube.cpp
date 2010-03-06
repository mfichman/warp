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
	game_(game),
	tubeVisible_(true) {

    // Generate the mesh data
    manual_->begin("Road", Ogre::RenderOperation::OT_TRIANGLE_LIST);
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
	object_->setUserPointer(0);
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

    // Read the input file
    std::ifstream in((name_).c_str());
	bool first = true;

    if (!in.is_open()) throw runtime_error("Unable to open tube file " + name_);
    while (!in.eof()) {
        std::string type;
        in >> type;
        
        if (type == "straight") {
			if (first) generateStraight(1);
            float length;
            in >> length;
            generateStraight(length);
        } else if (type == "curve") {
			if (first) generateStraight(1);
            float nx, ny, nz, angle, radius;
            in >> nx >> ny >> nz >> angle >> radius;
            angle = abs(angle);
            generateCurve(Vector3(nx, ny, nz), angle, radius);
        } else if (type == "radius") {
            in >> ringRadius_;
        } else if (type == "divisions") {
            in >> ringDivisions_;
			ringDivisions_++;
        } else if (type == "step") {
            in >> curveStep_;
		} else if (type == "visible") {
			in >> tubeVisible_;
		}

        if (in.fail()) break;
    }
}

/** Generates one ring around the current spine point using the transform_ */
void DynamicTube::generateRing() {

    // Generate the spine node
	SpineNode node;
	node.position = transform_ * Vector3::ZERO;
	node.visible = tubeVisible_;
	
    if (nodes_.size() != 0) {
        v_ += lastSpinePosition_.distance(node.position);
    }
	node.distance = v_;
    lastSpinePosition_ = node.position;
    nodes_.push_back(node);
    
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

			if (nodes_[i].visible) {
				// Add index to the visible mesh
				manual_->index(j % ringDivisions_ + i * ringDivisions_);
				manual_->index(j % ringDivisions_ + (i+1) * ringDivisions_);
				manual_->index((j+1) % ringDivisions_ + (i+1) * ringDivisions_);

				manual_->index(j % ringDivisions_ + i * ringDivisions_);
				manual_->index((j+1) % ringDivisions_ + (i+1) * ringDivisions_);
				manual_->index((j+1) % ringDivisions_ + i * ringDivisions_);
			}


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

/** Returns the spine projection given the current node number & the distance value */
SpineProjection DynamicTube::getSpineProjection(float distance, int node_i) const {
	int prev_i = node_i;
	int next_i = mod(prev_i + 1, nodes_.size());

	float total = nodes_[nodes_.size()-1].distance; // Total distance along tube
	distance = fmod(distance, total);
	if (distance < 0) distance += total;

	while (true) {
		const SpineNode& next = nodes_[next_i];
		const SpineNode& prev = nodes_[prev_i];

		if (distance < nodes_[prev_i].distance) {
			// Move forward along the tube to the next node
			next_i = mod(next_i - 1, nodes_.size());
			prev_i = mod(prev_i - 1, nodes_.size());
				
		} else if (distance > nodes_[next_i].distance) {
			// Move backward along the tube to the previous node
			next_i = mod(next_i + 1, nodes_.size());
			prev_i = mod(prev_i + 1, nodes_.size());

		} else {
			break;
		}
	}
	
	// At this point, "distance" should be between prev.distance and next.distance
    // compute location and forward vectors
	const SpineNode& prev = nodes_[prev_i];
    const SpineNode& next = nodes_[next_i];
    Vector3 prevForward = (next.position - prev.position).normalisedCopy();
    Vector3 nextForward = (nodes_[mod(next_i + 1, nodes_.size())].position - next.position).normalisedCopy();

    // projection of relative position in the direction of motion
    // divided by distance to the next node
    float alpha = (distance - prev.distance)/(next.distance - prev.distance);

    // interpolate
    SpineProjection result;
    result.position = (1 - alpha)*prev.position + (alpha)*next.position;
    result.forward = (1 - alpha)*prevForward + (alpha)*nextForward;
    result.forward.normalise();
	result.index = prev_i;
	result.distance = (1 - alpha)*prev.distance + (alpha)*next.distance;

	return result;
}

/** Returns the spine projection given the current node number */
SpineProjection DynamicTube::getSpineProjection(const Vector3& v, int node_i) const {

    const int n_nodes = nodes_.size();

    int closest_i = node_i;
    { /* find the closest node to v near node_i */
        float minDistance = nodes_[node_i].position.squaredDistance(v);
        // this loop will discover the local min in distance around node_i
        while(true) {
            // try the node in front and behind:
            float nextDistance = nodes_[mod(closest_i+1, n_nodes)].position.squaredDistance(v);
            if (nextDistance < minDistance) {
                minDistance = nextDistance;
                closest_i = mod(closest_i + 1, n_nodes);
                continue;
            }
            float prevDistance = nodes_[mod(closest_i-1, n_nodes)].position.squaredDistance(v);
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
    Vector3 forward = (nodes_[mod(closest_i + 1, n_nodes)].position - nodes_[closest_i].position).normalisedCopy();
    float projected_v = forward.dotProduct(v - nodes_[closest_i].position);
    if (projected_v > 0) {
        prev_i = closest_i;
        next_i = mod(closest_i + 1, n_nodes);
    } else {
        prev_i = mod(closest_i - 1, n_nodes);
        next_i = closest_i;
    }

    // compute location and forward vectors
	const SpineNode& prev = nodes_[prev_i];
    const SpineNode& next = nodes_[next_i];
    Vector3 prevForward = (next.position - prev.position).normalisedCopy();
    Vector3 nextForward = (nodes_[mod(next_i + 1, nodes_.size())].position - next.position).normalisedCopy();

    // projection of relative position in the direction of motion
    // divided by distance to the next node
    float alpha = prevForward.dotProduct(v - prev.position)/next.position.distance(prev.position);

    // interpolate
    SpineProjection result;
    result.position = (1 - alpha)*prev.position + (alpha)*next.position;
    result.forward = (1 - alpha)*prevForward + (alpha)*nextForward;
    result.forward.normalise();
	result.index = prev_i;
	result.distance = (1 - alpha)*prev.distance + (alpha)*next.distance;

	return result;
}
