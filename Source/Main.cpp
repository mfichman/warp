#include <Game.hpp>

using namespace Ogre;
using namespace std;

int main(int argc, char** argv) {
    try {
        dInitODE();
		Criterium::Game::Ptr game = new Criterium::Game(); 

		//Hack hack hack
		
		//Ogre::Plane plane(Vector3::UNIT_Y, -0.742);
		//Ogre::MeshManager::getSingleton().createPlane(
		//	"Ground", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		//	1500, 1500, 200, 200, true, 1, 500, 500, Vector3::UNIT_Z);
		

		// Hack hack hack - flat ground		
		//Ogre::Entity* entity = game->getSceneManager()->createEntity("Ground", "Ground");
		//entity->setMaterialName("Examples/Rockwall");
		//entity->setCastShadows(false);
		//game->getSceneManager()->getRootSceneNode()->createChildSceneNode()->attachObject(entity);
		

		Ogre::Entity* entity = game->getSceneManager()->createEntity("Road", "Road.mesh");
		game->getSceneManager()->getRootSceneNode()->createChildSceneNode()->attachObject(entity);
		// Hack ahack hack
		
		//game->getCamera()->setPosition(0, 100, 500);
		game->getCamera()->setPosition(10, 500, 10);
		game->getCamera()->lookAt(0, 0, 0);
		game->getCamera()->setNearClipDistance(0.5);
		game->getCamera()->setFarClipDistance(100);
		game->getSceneManager()->setWorldGeometry("terrain.cfg");
		game->getWindow()->getViewport(0)->setBackgroundColour(ColourValue(0.6, 0.6, 1.0));
		game->getSceneManager()->setFog(FOG_LINEAR, ColourValue(0.6, 0.6, 1.0), 0.0, 300, 900);

		Ogre::Light* light = game->getSceneManager()->createLight("Light");
		light->setType(Light::LT_DIRECTIONAL);
		light->setDiffuseColour(ColourValue(1.0, 1.0, 1.0));
		light->setSpecularColour(ColourValue(1.0, 1.0, 1.0));
		light->setDirection(Vector3(0, -1, 1)); 
		//game->getSceneManager()->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));

		game->getSceneManager()->setAmbientLight(ColourValue(0.0f, 0.0f, 0.0f));
		game->getRoot()->startRendering();
        
    } catch (Exception& ex) {
        cerr << "Exception: " << ex.getFullDescription() << endl;
    }

    return 0;
}