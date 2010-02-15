#include <Game.hpp>
#include <Ball.hpp>
#include <StaticTube.hpp>
#include <DynamicTube.hpp>
#include <FreeCamera.hpp>
#include <Overlays.hpp>
#include <Script.hpp>
#include <sstream>

using namespace Ogre;
using namespace std;

int main(int argc, char** argv) {
    try {
		auto_ptr<Warp::Game> game(new Warp::Game()); 
		
		game->getCamera()->setNearClipDistance(0.5);
		//game->getCamera()->setFarClipDistance(400);
        game->getCamera()->setPosition(0, 0, -20);
        game->getCamera()->lookAt(0, -10, 100);
		game->getWindow()->getViewport(0)->setBackgroundColour(ColourValue(0.6, 0.6, 1.0));
		//game->getSceneManager()->setFog(FOG_LINEAR, ColourValue(0.6, 0.6, 1.0), 0.0, 200, 400);

		Light* light = game->getSceneManager()->createLight("Light");
		light->setType(Light::LT_DIRECTIONAL);
		light->setDiffuseColour(ColourValue(0.8, 0.8, 0.8));
		light->setSpecularColour(ColourValue(2.0, 2.0, 2.0));
		light->setDirection(Vector3(0, -1, 1)); 

        
        //Warp::Overlays overlays(game.get());
        Warp::Script script(game.get(), "Scripts/Test.lua");
        //Warp::PickingRay ray(game.get());
        Warp::Ball ball(game.get());
        Warp::StaticTube tube(game.get(), "Test");
        //Warp::DynamicTube tube(game.get(), "Tube2");
        Warp::FreeCamera camera(game.get());

		//SceneNode* node = game->getSceneManager()->getRootSceneNode()->createChildSceneNode("Test");
		//Entity* entity = game->getSceneManager()->createEntity("Test", "Test.mesh");
		//entity->setMaterialName("Test");
		//node->attachObject(entity);

       /* Ogre::CompositorManager::ResourceMapIterator resourceIterator = Ogre::CompositorManager::getSingleton().getResourceIterator();

        // add all compositor resources to the view container
        while (resourceIterator.hasMoreElements())
        {
            Ogre::ResourcePtr resource = resourceIterator.getNext();
            const Ogre::String& compositorName = resource->getName();
            // Don't add base Ogre/Scene compositor to view
            if (compositorName == "Ogre/Scene")
                continue;

            cout << "@@@@@ " << compositorName << endl;
			int addPosition = -1;
			if (compositorName == "HDR")
			{
				// HDR must be first in the chain
				addPosition = 0;
			}
            Ogre::CompositorInstance *instance = Ogre::CompositorManager::getSingleton().addCompositor(game->getWindow()->getViewport(0), compositorName, addPosition);
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(game->getWindow()->getViewport(0), compositorName, false);

        }*/
        //Ogre::CompositorManager::getSingleton().addCompositor(game->getWindow()->getViewport(0), "Glass");
       //O/gre::CompositorManager::getSingleton().setCompositorEnabled(game->getWindow()->getViewport(0), "Glass", true);

		game->getRoot()->startRendering();
        
    } catch (Exception& ex) {
        cerr << "Exception: " << ex.getFullDescription() << endl;
    } catch (std::exception& ex) {
        cerr << "Exception: " << ex.what() << endl;
    }   

    return 0;
}