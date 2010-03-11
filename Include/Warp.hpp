/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************
 *
 * This contains forward declarations for all of the
 * classes as well as some global structs and functions
 */

#pragma once

#include <Ogre.h>
#include <string>

#define TIME_STEP 1.0f/60.0f
#define WARP_BT_FLAG 0x01000000 

// Forward declarations to increase compilation speed
namespace Ogre {
	class SceneManager;
	class RenderWindow;
	class Camera;
	class Root;
	class Vector3;
	class Quaternion;
	class Light;
	class SceneNode;
	class ColourValue;
	class Matrix4;
}

namespace OIS {
	class Keyboard;
	class Mouse;
	class InputManager;
}

namespace CEGUI {
	class OgreCEGUIRenderer;
	class System;
}

class btBroadphaseInterface;
class btCollisionDispatcher;
class btConstraintSolver;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;
class btTriangleIndexVertexArray;
class btGImpactMeshShape;
class btCollisionObject;
class btDynamicsWorld;
class btRigidBody;


class OscSender;
class OscBeatListener;

struct lua_State;

namespace Warp {

class Overlays;
class Objects;
class ScriptTask;
class Player;
class DynamicTube;
class Level;
class Game;
class Object;
class Enemy;
class GameListener;
class SubObject;
class Projectile;
class Targetable;
class City;
class Screen;


inline int mod(int num, int divisor) {
    int remainder = num % divisor;
    if (remainder < 0) remainder += divisor;
    return remainder;
}

struct SpineNode {
	SpineNode() :
		distance(0) {}

	Ogre::Vector3 position; // Position of the node
	float distance; // Distance along the path from first node
	bool visible;
	int script;
};

struct SpineProjection {
	SpineProjection() :
		index(0),
		distance(0),
		script(0) {}

    Ogre::Vector3 position;
    Ogre::Vector3 forward;
	int index;
	float distance;
	int script;
};

struct BeatLoop {

    BeatLoop()
        : id(1)
        , path_name("no_path_name")
        , bpm(120)
        , n_beats(8)
    {}
    
    int id;
    std::string path_name;
    int bpm;
    int n_beats;
};

class GameListener {
public:

	/** Called for every physics timestep (fixed at every 0.01 s) */
	virtual void onTimeStep() {}
};

/** Methods for sending Ogre and ODE values to a script */
lua_State* operator<<(lua_State* env, const Ogre::Vector3& v);
lua_State* operator<<(lua_State* env, const Ogre::Quaternion& q);
lua_State* operator<<(lua_State* env, const Ogre::SceneNode& n);
lua_State* operator<<(lua_State* env, const Ogre::Light& l);
lua_State* operator<<(lua_State* env, const Ogre::ColourValue& c);


/** Methods for reading Ogre and ODE values from a script */
lua_State* operator>>(lua_State* env, Ogre::Vector3& v);
lua_State* operator>>(lua_State* env, Ogre::Quaternion& q);
lua_State* operator>>(lua_State* env, Ogre::SceneNode& n);
lua_State* operator>>(lua_State* env, Ogre::Light& l);
lua_State* operator>>(lua_State* env, Ogre::ColourValue& c);
lua_State* operator>>(lua_State* env, std::string& s);
lua_State* operator>>(lua_State* env, Warp::BeatLoop& bl);
lua_State* operator>>(lua_State* env, Warp::Object& e);
lua_State* operator>>(lua_State* env, Ogre::Entity& e);
lua_State* operator>>(lua_State* env, btRigidBody& n);

void loadScript(lua_State* env, const std::string& name);
void loadScriptFolder(lua_State* env, const std::string& dir);

}
