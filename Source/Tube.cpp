/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Tube.hpp>
#include <Objects.hpp>

using namespace Criterium;
using namespace Ogre;
using namespace std;

struct Tube::Impl : public Game::Listener {

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
        assert(indexSize == sizeof(short));
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
        ibuf->unlock();
		
		// Lock buffers, read, and build the index
		data_ = dGeomTriMeshDataCreate();
		dGeomTriMeshDataBuildSingle(data_, &vertices_.front(), sizeof(Vector3), vertices_.size(), &indices_.front(), indices_.size(), sizeof(int));		
		tube_ = dCreateTriMesh(game_->getSpace(), data_, 0, 0, 0);
        dGeomSetCategoryBits(tube_, Objects::TYPE_TERRAIN);
		dGeomSetCollideBits(tube_, Objects::TYPE_BALL);

		// Setup the height map
		/*Ogre::TubeSceneManager* mgr = static_cast<Ogre::TubeSceneManager*>(game_->getSceneManager());
		heightfield_ = dGeomHeightfieldDataCreate();

		dGeomHeightfieldDataBuildCallback (heightfield_, mgr, &Impl::onHeightfieldQuery, 3000.0, 3000.0, 2, 2, 1.0, 0.0, 0, 0);
		dGeomHeightfieldDataSetBounds(heightfield_, 0.0f,  100.0f);

		tube_ = dCreateHeightfield(game_->getSpace(), heightfield_, 0);
		dGeomSetCategoryBits(road_, TYPEROAD);
		dGeomSetCollideBits(road_, TYPEBALL | TYPEWHEEL);*/
	}

	void onTimeStep() {
		//dMatrix4 matrix;
		//memset(matrix, 0, sizeof(matrix));
		//matrix[0] = 1.0f;
		//matrix[5] = 1.0f;
		//matrix[10] = 1.0f;
		//matrix[15] = 1.0f;
		//dGeomTriMeshSetLastTransform(geom_, matrix);

		//cout << dGeomTriMeshGetTriangleCount(geom_) << endl;
	}

	Game* game_;
	dTriMeshDataID data_;
	dGeomID tube_;
	std::vector<Ogre::Vector3> vertices_;
	std::vector<int> indices_;
};

Tube::Tube(Game* game, const std::string& name) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->init(name);
	game->addListener(impl_.get());
}
