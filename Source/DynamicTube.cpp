/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include <DynamicTube.hpp>
#include <Objects.hpp>

#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

using namespace Warp;
using namespace Ogre;
using namespace std;

struct DynamicTube::Impl : public Game::Listener {

    Impl(Game* game, const std::string& name) :
        curveStep_(5.0f),
        ringDivisions_(32),
        ringRadius_(10.0f),
        transform_(Matrix4::IDENTITY),
        ringCount_(0),
        v_(0),
        vertexCount_(0),
        triangleCount_(0),
        indexCount_(0),
        name_(name),
        manual_(game->getSceneManager()->createManualObject(name)),
        data_(new btTriangleMesh(true, false)),
        game_(game) {

        // Generate the mesh data
        manual_->begin("Road", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
        readInputFile();
        generateIndices();
        manual_->end();

        // Initialize the graphical rep
		SceneNode* node = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode();
        manual_->setCastShadows(false);
        node->attachObject(manual_);

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

	    game_->addListener(this);
    }
    
    ~Impl() {
        // destroy scene node
        // destroy manual
        game_->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name_);
        game_->getSceneManager()->destroyManualObject(name_);
        game_->removeListener(this);
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
    void readInputFile() {
        // Create the first ring
        generateStraight(0);

        // Read the input file
        std::ifstream in((name_ + ".tube").c_str());

        if (!in.is_open()) throw runtime_error("Unable to read tube file");
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

            if (in.fail()) throw runtime_error("Unable to read tube file");
        }
    }

    /** Generates one ring around the current spine point using the transform_ */
    void generateRing() {

        // Generate the spine node
	    Vector3 spinePosition = transform_ * Vector3::ZERO;
	    Vector4 spineForward = transform_ * Vector4(0, 0, 1, 0);
	    Vector4 spineUp = transform_ * Vector4(0, 1, 0, 0);

        if (ringCount_ != 0) {
            v_ += lastSpinePosition_.distance(spinePosition);
        }
        lastSpinePosition_ = spinePosition;

        SpineNode node;
        node.position = spinePosition;
        node.forward = Vector3(spineForward.x, spineForward.y, spineForward.z);
        node.up = Vector3(spineUp.x, spineUp.y, spineUp.z);
        node.index = ringCount_;
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
            data_->findOrAddVertex(btVector3(position.x, position.y, position.z), false);

            vertexCount_++;
	    }
	    ringCount_++;
    }

    /** Generates a straight tube segment */
    void generateStraight(float length) {
	    transform_ = transform_.concatenate(Matrix4::getTrans(0, 0, length));
        generateRing();
    }

    /** Generates a curved tube segment */
    void generateCurve(const Vector3& normal, float angle, float radius) {
	    for (float t = 0; t < angle; t += curveStep_) {
		    Vector3 right = normal.crossProduct(Vector3::UNIT_Z);
		    right.normalise();

		    transform_ = transform_.concatenate(Matrix4::getTrans(radius * right));
		    transform_ = transform_.concatenate(Matrix4(Quaternion(Degree(curveStep_), normal)));
	        transform_ = transform_.concatenate(Matrix4::getTrans(-radius * right));	
	        generateRing();
        }
    }

    /** Generates the indexCount_ for the mesh */
    void generateIndices() {
	    for (int i = 0; i < ringCount_-1; i++) {
		    for (int j = 0; j <= ringDivisions_; j++) {

                    // Add index to the visible mesh
			        manual_->index(j % ringDivisions_ + i * ringDivisions_);
			        manual_->index(j % ringDivisions_ + (i+1) * ringDivisions_);

                    // Add indices to the physical mesh
                    data_->addIndex(j % ringDivisions_ + i * ringDivisions_);
                    data_->addIndex(j % ringDivisions_ + (i+1) * ringDivisions_);
                    data_->addIndex(j % (ringDivisions_+1) + ringDivisions_);

                    data_->addIndex(j % (ringDivisions_+1) + ringDivisions_);
                    data_->addIndex(j % ringDivisions_ + (i+1) * ringDivisions_);
                    data_->addIndex(j % (ringDivisions_+1) + (i+1) * ringDivisions_);

                    indexCount_ += 2;
                    triangleCount_ += 2;
		    }
	    }

        // This accounts for the first two indexCount_, which don't 
        // create unique triangleCount_
        triangleCount_ -= 2; 
    }

    /** These parameters are exclusively for vertex generation */
    Matrix4 transform_;
    std::string name_;
    int vertexCount_;
    int triangleCount_;
    int indexCount_;
    int ringCount_;
    float v_;
    Vector3 lastSpinePosition_;
    float curveStep_;
    float ringRadius_;
    int ringDivisions_;

    /** These variables are for the loaded scene objects */
    Game* game_;
    ManualObject* manual_;
    auto_ptr<btTriangleMesh> data_;
    auto_ptr<btGImpactMeshShape> shape_;
    auto_ptr<btCollisionObject> object_;
    btTransform position_;
    std::vector<SpineNode> nodes_;
    int lastSpineNodeIndex_;
};

DynamicTube::DynamicTube(Game* game, const std::string& name) : impl_(new Impl(game, name)) {
}

DynamicTube::~DynamicTube() {

}
