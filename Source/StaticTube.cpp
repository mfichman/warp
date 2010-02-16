/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include <StaticTube.hpp>
#include <Objects.hpp>

#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

using namespace Warp;
using namespace Ogre;
using namespace std;

struct StaticTube::Impl : public Game::Listener {

	/** Initializes the the tube */
    Impl(Game* game, const std::string& name) :
        game_(game),
        name_(name),
        lastSpineNodeIndex_(0) {
        
		SceneNode* node = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name);
        Entity* entity = game_->getSceneManager()->createEntity(name, name + ".mesh");
        entity->setCastShadows(false);
        entity->setMaterialName("Road");
        node->attachObject(entity);

        initPhysics(entity->getMesh());
        initSpine(name);

	    game_->addListener(this);
    }

    /** Destroys the tube */
    ~Impl() {
        game_->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name_);
        game_->getSceneManager()->destroyEntity(name_);
        game_->removeListener(this);
        game_->getWorld()->removeCollisionObject(object_.get());
    }

    /** Loads the spines from the spine file */
    void initSpine(const std::string& name) {
        std::ifstream in((name + ".spine").c_str());

        cout << name + ".spine" << endl;

        if (!in.is_open()) throw runtime_error("Unable to read file");
        SpineNode node;
        node.index = 0;
        while (!in.eof() && !in.fail()) {
            std::string tag;
            in >> tag >> node.position.x >> node.position.y >> node.position.z;
            
            // Push a copy of the node
            nodes_.push_back(node);
            node.index++; 
        }

    }

    /** Loads the vertices into Bullet's triangle mesh object */
    void initPhysics(MeshPtr mesh) {
		SubMesh* submesh = mesh->getSubMesh(0);

		// Prepare to read vertex data from the hardware mesh buffer
		VertexData* vertexData = submesh->vertexData;
		VertexBufferBinding* binding = vertexData->vertexBufferBinding;
		VertexDeclaration* decl = vertexData->vertexDeclaration;
		HardwareVertexBufferSharedPtr vbuf = binding->getBuffer(0);
		int vertexCount = vertexData->vertexCount;
		int vertexSize = decl->getVertexSize(0);

		// Read vertices into an array
		char* vptr = (char*)vbuf->lock(HardwareBuffer::HBL_READ_ONLY) + vertexData->vertexStart*vertexSize;
		for (int i = 0; i < vertexCount; i++) {
			vertices_.push_back(*(Ogre::Vector3*)vptr);
			vptr += vertexSize;
		}
		vbuf->unlock();

		// Read indices into a temporary array...this assumes the vertices are stored
		// as a triangle strip!
        IndexData* indexData = submesh->indexData;
        HardwareIndexBufferSharedPtr ibuf = indexData->indexBuffer;
        int indexCount = indexData->indexCount;
        int indexSize = ibuf->getIndexSize();

        if (indexSize == sizeof(short)) {
            // Use this branch if indices are 16 bits (small objects)
            short* iptr = ((short*)ibuf->lock(HardwareBuffer::HBL_READ_ONLY)) + indexData->indexStart;
		    for (int i = 2; i < indexCount; i++) {
                if (i % 2 == 0) {
                    indices_.push_back(iptr[i-2]);
                    indices_.push_back(iptr[i-1]);
                    indices_.push_back(iptr[i]);
                } else {
                    indices_.push_back(iptr[i]);
                    indices_.push_back(iptr[i-1]);
                    indices_.push_back(iptr[i-2]);
                }
		    }
        } else {
            // Use this branch if indices are 32 bits (large objects)
            assert(indexSize == sizeof(int));
            int* iptr = ((int*)ibuf->lock(HardwareBuffer::HBL_READ_ONLY)) + indexData->indexStart;
		    for (int i = 2; i < indexCount; i++) {
                if (i % 2 == 0) {
                    indices_.push_back(iptr[i-2]);
                    indices_.push_back(iptr[i-1]);
                    indices_.push_back(iptr[i]);
                } else {
                    indices_.push_back(iptr[i]);
                    indices_.push_back(iptr[i-1]);
                    indices_.push_back(iptr[i-2]);
                }
		    }
        }
        ibuf->unlock();

        data_.reset(new btTriangleIndexVertexArray(indices_.size()/3, &indices_.front(), 3*sizeof(int), vertices_.size(), (btScalar*)&vertices_.front(), sizeof(Vector3)));

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

	/** Called when a new frame is detected */
	void onTimeStep() {
        const SpineNode& node = getClosestSpineNode(game_->getCamera()->getPosition(), lastSpineNodeIndex_);
        lastSpineNodeIndex_ = node.index;
        game_->setSpineNode(node);
	}

    /** 
     * Returns the spine that is closest to the given location.
     * position: this function will find the closest spine to this position
     * guess: where the function will start looking for the nearest spine
     */
    const SpineNode& getClosestSpineNode(const Vector3& pos, int guess) {
        int i = guess;
        assert(i >= 0 && i < (int)nodes_.size());

        // To start, find which direction we should be searching
        const SpineNode& prev = nodes_[max(0, i-1)];
        const SpineNode& current = nodes_[max(0, i)];
        const SpineNode& next = nodes_[min((int)nodes_.size()-1, i+1)];

        float distancePrev = prev.position.distance(pos);
        float distanceCurrent = current.position.distance(pos);
        float distanceNext = next.position.distance(pos);

        float best;
        int direction;
        if (distancePrev < distanceCurrent) {
            best = distancePrev;
            direction = -1;
        } else if (distanceNext < distanceCurrent) {
            best = distanceNext;
            direction = 1;
        } else {
            return current;
        }

        while (true) {
            if ((i + direction) < 0 || (i + direction) >= (int)nodes_.size()) return nodes_[i];

            float distance = nodes_[i + direction].position.distance(pos);
            if (distance > best) return nodes_[i];

            best = distance;
            i += direction;
        }
    }

	Game* game_;
    std::string name_;
	std::vector<Ogre::Vector3> vertices_;
	std::vector<int> indices_;
    auto_ptr<btTriangleIndexVertexArray> data_;
    auto_ptr<btGImpactMeshShape> shape_;
    auto_ptr<btCollisionObject> object_;
    btTransform position_;
    std::vector<SpineNode> nodes_;
    int lastSpineNodeIndex_;

};

StaticTube::StaticTube(Game* game, const std::string& name) : impl_(new Impl(game, name)) {
}

StaticTube::~StaticTube() {

}