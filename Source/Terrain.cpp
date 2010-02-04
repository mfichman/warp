/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Terrain.hpp>
#include <fstream>

using namespace Criterium;
using namespace Ogre;
using namespace std;

struct Terrain::Impl : public Game::Listener {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init(const std::string& name) {

		// Initialize the heightmap
		game_->getSceneManager()->setWorldGeometry(name + ".cfg");

		// Create the road mesh
		SceneNode* node = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		Entity* entity = game_->getSceneManager()->createEntity(name + "Top", name + "Top.mesh");
		node->attachObject(entity);
		node->attachObject(game_->getSceneManager()->createEntity(name + "Right", name + "Right.mesh"));
		node->attachObject(game_->getSceneManager()->createEntity(name + "Left", name + "Left.mesh"));

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
		char* ptr = (char*)vbuf->lock(HardwareBuffer::HBL_READ_ONLY) + vertexData->vertexStart*vertexSize;
		for (int i = 0; i < vertexCount; i++) {
			vertices_.push_back(*(Ogre::Vector3*)ptr);
			ptr += vertexSize;
		}
		vbuf->unlock();

		
		// Read indices into a temporary array...this assumes the vertices are stored
		// as a triangle strip!
		for (int i = 0; i < vertexCount-2; i++) {

			if (i % 2 == 0) {
				indices_.push_back(i);
				indices_.push_back(i+1);
				indices_.push_back(i+2);
			} else {
				indices_.push_back(i+2);
				indices_.push_back(i+1);
				indices_.push_back(i);
			}
		}
		
		// Lock buffers, read, and build the index
		data_ = dGeomTriMeshDataCreate();
		dGeomTriMeshDataBuildSingle(data_, &vertices_.front(), sizeof(Vector3), vertices_.size(), &indices_.front(), indices_.size(), sizeof(int));
		
		
		geom_ = dCreateTriMesh(game_->getSpace(), data_, 0, 0, 0);
		dGeomSetCategoryBits(geom_, TYPEROAD);
		dGeomSetCollideBits(geom_, TYPEBALL | TYPEWHEEL);
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
	dGeomID geom_;
	std::vector<Ogre::Vector3> vertices_;
	std::vector<int> indices_;
};

Terrain::Terrain(Game* game, const std::string& name) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->init(name);
	game->addListener(impl_.get());
}
