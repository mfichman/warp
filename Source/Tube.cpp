/******************************************************************************
 * Warp: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Tube.hpp>
#include <Objects.hpp>

#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

using namespace Warp;
using namespace Ogre;
using namespace std;

int last =0;

struct Tube::Impl : public Game::Listener, public btMotionState {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init(const std::string& name) {
		// Create the road mesh
        
		SceneNode* node = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode();
        Entity* entity = game_->getSceneManager()->createEntity(name, name + ".mesh");
        entity->setCastShadows(false);
        entity->setMaterialName("Road");
        node->attachObject(entity);

        initPhysics(entity->getMesh());
        initSpine(name);
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
            in >> tag >> node.forward.x >> node.forward.y >> node.forward.z;
            in >> tag >> node.up.x >> node.up.y >> node.up.z;
            

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

    ~Impl() {
        game_->getWorld()->removeCollisionObject(object_.get());
    }

    /** Called by bullet to get the transform state */
    void getWorldTransform(btTransform& transform) const {
        transform = position_;
    }

    /** Called by Bullet to update the scene node */
    void setWorldTransform(const btTransform& transform) {
        //const btQuaternion& rotation = transform.getRotation();
        //node_->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
        //const btVector3& position = transform.getOrigin();
        //node_->setPosition(position.x(), position.y(), position.z());
    }

	/** Called when a new frame is detected */
	void onTimeStep() {
        const SpineNode& node = t->getClosestSpineNode(game_->getCamera()->getPosition(), last);

        last = node.index;

        cout << "Closest spine node: " << node.index << endl;
	}

	Game* game_;
    Tube* t;
	std::vector<Ogre::Vector3> vertices_;
	std::vector<int> indices_;
    auto_ptr<btTriangleIndexVertexArray> data_;
    auto_ptr<btGImpactMeshShape> shape_;
    auto_ptr<btCollisionObject> object_;
    btTransform position_;
    std::vector<Tube::SpineNode> nodes_;

};

Tube::Tube(Game* game, const std::string& name) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->init(name);
	game->addListener(impl_.get());
    impl_->t = this;
}

Tube::~Tube() {

}

const Tube::SpineNode&
Tube::getClosestSpineNode(const Vector3& pos, int guess) {
    int i = guess;
    assert(i >= 0 && i < (int)impl_->nodes_.size());

    // To start, find which direction we should be searching
    const SpineNode& prev = impl_->nodes_[max(0, i-1)];
    const SpineNode& current = impl_->nodes_[max(0, i)];
    const SpineNode& next = impl_->nodes_[min((int)impl_->nodes_.size()-1, i+1)];

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
        if ((i + direction) < 0 || (i + direction) >= (int)impl_->nodes_.size()) return impl_->nodes_[i];

        float distance = impl_->nodes_[i + direction].position.distance(pos);
        if (distance > best) return impl_->nodes_[i];

        best = distance;
        i += direction;
    }
}