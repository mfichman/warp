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


		game->getWindow()->getViewport(0)->setBackgroundColour(ColourValue(0.2f, 0.2f, 0.2f));
		// TOOD: REENABLE FOG
#ifndef LEVEL_EDITOR_MODE
		game->getSceneManager()->setFog(FOG_LINEAR, ColourValue(0.2f, 0.2f, 0.2f), 0.0f, 0.0f, 450.0f);
		//game->getSceneManager()->setFog(FOG_EXP, ColourValue(0.4, 0.4, 0.8), 0.007, 0.005);
#endif
		game->getSceneManager()->setAmbientLight(ColourValue(0.4f, 0.4f, 0.4f));

		Light* light = game->getSceneManager()->createLight("Light");
		light->setType(Light::LT_DIRECTIONAL);
		light->setDiffuseColour(ColourValue(0.6f, 0.6f, 0.6f));
		light->setSpecularColour(ColourValue(1.0f, 1.0f, 1.0f));
		light->setDirection(Vector3(0.5f, -1.0f, 0.8f)); 
        
        //Warp::ScriptTask script(game.get(), "Scripts/Test.lua");
#ifdef LEVEL_EDITOR_MODE
		game->getCamera()->setPosition(2500, 10000, 2500);
        game->getCamera()->lookAt(0, 0, 0);
        Warp::FreeCamera camera(game.get());
#endif

		// Glass
		// Radial Blur
        //Ogre::CompositorManager::getSingleton().addCompositor(game->getWindow()->getViewport(0), "Motion Blur");
        //Ogre::CompositorManager::getSingleton().setCompositorEnabled(game->getWindow()->getViewport(0), "Motion Blur", true);

		//game->setLevel("Tube1");
		game->setScreen("LoadScreen");
		game->getRoot()->startRendering();

    } catch (std::exception& ex) {
        cerr << "Exception: " << ex.what() << endl;
		system("pause");
    } 

#ifdef WIN32   
	//system("taskkill /IM chuck.exe");
#endif

    return 0;
}
