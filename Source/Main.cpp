#include <Game.hpp>
#include <Player.hpp>
#include <Level.hpp>
#include <DynamicTube.hpp>
#include <FreeCamera.hpp>
#include <Overlays.hpp>
#include <ScriptTask.hpp>
#include <sstream>
#include <boost/thread.hpp>

using namespace Ogre;
using namespace std;

int main(int argc, char** argv) {
#ifdef WIN32
	//system("start /Daudio \"Warp: Chuck Server\" chuck.exe play.ck");
#endif

	try {
		auto_ptr<Warp::Game> game(new Warp::Game()); 
		
        game->getCamera()->setPosition(0, 0, -20);
        game->getCamera()->lookAt(0, -10, 100);
		game->getWindow()->getViewport(0)->setBackgroundColour(ColourValue(0.0, 0.0, 0.0));
		game->getSceneManager()->setFog(FOG_LINEAR, ColourValue(0.0, 0.0, 0.0), 0.0, 0, 150);

		Light* light = game->getSceneManager()->createLight("Light");
		light->setType(Light::LT_DIRECTIONAL);
		light->setDiffuseColour(ColourValue(0.8, 0.8, 0.8));
		light->setSpecularColour(ColourValue(2.0, 2.0, 2.0));
		light->setDirection(Vector3(0, -1, 1)); 

        
        //Warp::ScriptTask script(game.get(), "Scripts/Test.lua");
        //Warp::PickingRay ray(game.get());
        //Warp::StaticTube tube(game.get(), "Test");
        //Warp::DynamicTube tube(game.get(), "Tube1");
        Warp::FreeCamera camera(game.get());
        Warp::Overlays overlays(game.get());

        //Ogre::CompositorManager::getSingleton().addCompositor(game->getWindow()->getViewport(0), "Glass");
       //O/gre::CompositorManager::getSingleton().setCompositorEnabled(game->getWindow()->getViewport(0), "Glass", true);

		game->setLevel("Tube1");
		game->getRoot()->startRendering();


    } catch (Exception& ex) {
        cerr << "Exception: " << ex.getFullDescription() << endl;
    } catch (std::exception& ex) {
        cerr << "Exception: " << ex.what() << endl;
    }   
#ifdef WIN32   
	//system("taskkill /IM chuck.exe");
#endif

    return 0;
}
