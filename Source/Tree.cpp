/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <Bicycle.hpp>
#include <Application.hpp>
#include <sstream>

using namespace Criterium;
using namespace Ogre;
using namespace std;


//------------------------------------------------------------------------------
Tree::Ptr
Tree::make(Application* app) {
    return new Tree(app);
}


//------------------------------------------------------------------------------
Tree::Tree(Application* app) {

	SceneNode* root = app->sceneManager()->getRootSceneNode();
	

	for (int i = 0; i < 50; i++) {
		ostringstream os;
		os << "Set" << i << endl;

		BillboardSet* set = app->sceneManager()->createBillboardSet(os.str());
		set->setMaterialName("Scene/PineMaterial");
		set->setCastShadows(true);

		Billboard* billboard = set->createBillboard(Vector3(0, 0, 0));
		billboard->setDimensions(3.0, 3.0);
		
		SceneNode* node = root->createChildSceneNode(os.str());
		node->setPosition((rand()%50)-25, 0.2, (rand()%50)-25);
		node->attachObject(set);
	}

	for (int i = 0; i < 50; i++) {
		ostringstream os;
		os << "Set" << i+50 << endl;

		BillboardSet* set = app->sceneManager()->createBillboardSet(os.str());
		set->setMaterialName("Scene/EucalyptusMaterial");
		set->setCastShadows(true);

		Billboard* billboard = set->createBillboard(Vector3(0, 0, 0));
		billboard->setDimensions(3.0, 3.0);
		
		SceneNode* node = root->createChildSceneNode(os.str());
		node->setPosition((rand()%50)-25, 0.2, (rand()%50)-25);
		node->attachObject(set);
	}
}



//------------------------------------------------------------------------------

Tree::~Tree() {
}
