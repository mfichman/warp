#include <Game.hpp>
#include <Overlays.hpp>
#include <Script.hpp>
#include <PickingRay.hpp>
#include <sstream>

using namespace Ogre;
using namespace std;

int main(int argc, char** argv) {
    try {
        dInitODE();
		auto_ptr<Criterium::Game> game(new Criterium::Game()); 
		
		game->getCamera()->setNearClipDistance(0.5);
		game->getCamera()->setFarClipDistance(400);
        game->getCamera()->setPosition(1, 200, 1);
        game->getCamera()->lookAt(0, 0, 0);
		game->getWindow()->getViewport(0)->setBackgroundColour(ColourValue(0.6, 0.6, 1.0));
		//game->getSceneManager()->setFog(FOG_LINEAR, ColourValue(0.6, 0.6, 1.0), 0.0, 200, 400);

		Light* light = game->getSceneManager()->createLight("Light");
		light->setType(Light::LT_DIRECTIONAL);
		light->setDiffuseColour(ColourValue(0.8, 0.8, 0.8));
		light->setSpecularColour(ColourValue(0.0, 0.0, 0.0));
		light->setDirection(Vector3(0, -1, 1)); 

        //Criterium::Script script(game.get(), "Scripts/Test.lua");
        //Criterium::PickingRay ray(game.get());

		SceneNode* node = game->getSceneManager()->getRootSceneNode()->createChildSceneNode("Test");
		Entity* entity = game->getSceneManager()->createEntity("Test", "Test.mesh");
		entity->setMaterialName("Test");
		node->attachObject(entity);

		game->getRoot()->startRendering();
        
    } catch (Exception& ex) {
        cerr << "Exception: " << ex.getFullDescription() << endl;
    } catch (std::exception& ex) {
        cerr << "Exception: " << ex.what() << endl;
    }   

    return 0;
}