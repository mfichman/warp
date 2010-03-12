#include <Game.hpp>
#include <Player.hpp>
#include <Level.hpp>
#include <DynamicTube.hpp>
#include <FreeCamera.hpp>
#include <ScriptTask.hpp>
#include <City.hpp>
#include <sstream>
#include <boost/thread.hpp>
#include <cstdio>

using namespace Ogre;
using namespace std;

int main(int argc, char** argv) {
#ifdef WIN32
	system("start /Daudio \"Warp: Chuck Server\" chuck.exe play.ck");
#endif

	try {
		auto_ptr<Warp::Game> game(new Warp::Game()); 
		
		Ogre::LogManager::getSingleton().setLogDetail(LL_LOW);

        game->getCamera()->setPosition(400, 100, 800);
        game->getCamera()->lookAt(0, 0, 0);
		game->getWindow()->getViewport(0)->setBackgroundColour(ColourValue(0.2, 0.2, 0.2));
		game->getSceneManager()->setFog(FOG_LINEAR, ColourValue(0.2, 0.2, 0.2), 0.0, 0, 450);
		//game->getSceneManager()->setFog(FOG_EXP, ColourValue(0.4, 0.4, 0.8), 0.007, 0.005);
		game->getSceneManager()->setAmbientLight(ColourValue(0.4, 0.4, 0.4));

		Light* light = game->getSceneManager()->createLight("Light");
		light->setType(Light::LT_DIRECTIONAL);
		light->setDiffuseColour(ColourValue(0.6, 0.6, 0.6));
		light->setSpecularColour(ColourValue(1.0, 1.0, 1.0));
		light->setDirection(Vector3(0.5, -1, 0.8)); 
        
        //Warp::ScriptTask script(game.get(), "Scripts/Test.lua");
        //Warp::FreeCamera camera(game.get());

		// Glass
		// Radial Blur
        //Ogre::CompositorManager::getSingleton().addCompositor(game->getWindow()->getViewport(0), "Motion Blur");
        //Ogre::CompositorManager::getSingleton().setCompositorEnabled(game->getWindow()->getViewport(0), "Motion Blur", true);

		//game->setLevel("Tube1");
		game->setScreen("LoadScreen");
		game->getRoot()->startRendering();

    } catch (std::exception& ex) {
        cerr << "Exception: " << ex.what() << endl;
		cerr << "Press any key to continue..." << endl;
		getchar();
    } 

#ifdef WIN32   
	//system("taskkill /IM chuck.exe");
#endif

    return 0;
}
