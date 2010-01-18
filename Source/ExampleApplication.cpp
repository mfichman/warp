/*#include "ExampleApplication.h"

class TutorialApplication : public ExampleApplication
{
protected:
public:
    TutorialApplication()
    {
    }

    ~TutorialApplication() 
    {
    }
protected:
    void chooseSceneManager(void)
    {
		mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE);
    }

    void createScene(void)
    {
		mSceneMgr->setWorldGeometry("terrain.cfg");
		//mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
		//mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

		//ColourValue fadeColour(0.9, 0.9, 0.9);
        //mWindow->getViewport(0)->setBackgroundColour(fadeColour);
		//mSceneMgr->setFog(FOG_LINEAR, fadeColour, 0, 50, 500);
		//mSceneMgr->setFog(FOG_EXP, fadeColour, 0.005);

		ColourValue fadeColour(0.1, 0.1, 0.1);
        mWindow->getViewport(0)->setBackgroundColour(fadeColour);
        mSceneMgr->setFog(FOG_LINEAR, fadeColour, 0.0, 10, 150);

        mSceneMgr->setWorldGeometry("terrain.cfg");

        Plane plane;
        plane.d = 10;
        plane.normal = Vector3::NEGATIVE_UNIT_Y;

        mSceneMgr->setSkyPlane(true, plane, "Examples/SpaceSkyPlane", 100, 45, true, 0.5, 150, 150);
    }
};

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

//INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
//#else

#endif
int main(int argc, char **argv)
{
    // Create application object
    TutorialApplication app;

    try {
        app.go();
    } catch(Exception& e) {
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occurred: %s\n",
                e.getFullDescription().c_str());
#endif
    }

    return 0;
}*/
