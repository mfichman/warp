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

struct Tube::Impl : public Game::Listener, public btMotionState {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init(const std::string& name) {
		// Create the road mesh
        
		SceneNode* node = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode();
        Entity* entity = game_->getSceneManager()->createEntity(name, name + ".mesh");
        entity->setCastShadows(false);
        entity->setMaterialName("Road");
        node->attachObject(entity);


		MeshPtr mesh = entity->getMesh();
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
	}

	Game* game_;
	std::vector<Ogre::Vector3> vertices_;
	std::vector<int> indices_;
    auto_ptr<btTriangleIndexVertexArray> data_;
    auto_ptr<btGImpactMeshShape> shape_;
    auto_ptr<btCollisionObject> object_;
    btTransform position_;

};

Tube::Tube(Game* game, const std::string& name) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->init(name);
	game->addListener(impl_.get());
}

Tube::~Tube() {

}