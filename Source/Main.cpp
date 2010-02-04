#include <Game.hpp>
#include <Terrain.hpp>
#include <Ball.hpp>
#include <Cylinder.hpp>
#include <Overlays.hpp>
#include <Bicycle.hpp>
#include <OgreTerrainSceneManager.h>
#include <sstream>

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
		
		
		
		game->getCamera()->setNearClipDistance(0.5);
		game->getCamera()->setFarClipDistance(100);
		game->getWindow()->getViewport(0)->setBackgroundColour(ColourValue(0.6, 0.6, 1.0));
		game->getSceneManager()->setFog(FOG_LINEAR, ColourValue(0.6, 0.6, 1.0), 0.0, 300, 900);

		Criterium::Terrain* terrain = new Criterium::Terrain(game.get(), "Road1");
		//Criterium::Cylinder* cylinder = new Criterium::Cylinder(game.get());
		Criterium::Ball* ball = new Criterium::Ball(game.get());
		Criterium::Overlays* overlays = new Criterium::Overlays(game.get());
		//Criterium::Bicycle* bicycle = new Criterium::Bicycle(game.get());

		// Light
		Ogre::Light* light = game->getSceneManager()->createLight("Light");
		light->setType(Light::LT_DIRECTIONAL);
		light->setDiffuseColour(ColourValue(0.8, 0.8, 0.8));
		light->setSpecularColour(ColourValue(0.0, 0.0, 0.0));
		light->setDirection(Vector3(0, -1, 1)); 
		game->getSceneManager()->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));

		game->getRoot()->startRendering();
        
    } catch (Exception& ex) {
        cerr << "Exception: " << ex.getFullDescription() << endl;
    }

    return 0;
}