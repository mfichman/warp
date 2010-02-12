/******************************************************************************
 * Warp: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Terrain.hpp>
#include <fstream>
#include <OgreTerrainSceneManager.h>

using namespace Warp;
using namespace Ogre;
using namespace std;

struct Terrain::Impl : public Game::Listener {

	/** Initializes the OGRE scene nodes, and the attached rigid bodies */
	void init(const std::string& name) {

		// Initialize the heightmap
		game_->getSceneManager()->setWorldGeometry(name + ".cfg");

		// Create the road mesh
        Entity* entity;
		SceneNode* node = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode();
        entity = game_->getSceneManager()->createEntity(name + "Right", name + "Right.mesh");
        entity->setCastShadows(false);
        node->attachObject(entity);
		entity = game_->getSceneManager()->createEntity(name + "Left", name + "Left.mesh");
        entity->setCastShadows(false);
        node->attachObject(entity);
		entity = game_->getSceneManager()->createEntity(name + "Top", name + "Top.mesh");
		entity->setCastShadows(false);
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
	}

	void onTimeStep() {
	}

	Game* game_;
	std::vector<Ogre::Vector3> vertices_;
	std::vector<int> indices_;
};

Terrain::Terrain(Game* game, const std::string& name) : impl_(new Impl()) {
	impl_->game_ = game;
	impl_->init(name);
	game->addListener(impl_.get());
}
