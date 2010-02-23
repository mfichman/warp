/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Enemy.hpp"

#include "Game.hpp"

#include <sstream>
 extern "C" {
#include <lua/lua.h> 
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

using namespace Warp;
using namespace Ogre;
using namespace std;

#define ENEMY_MASS 1.0f
#define ENEMY_RADIUS 0.5f

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Enemy::Enemy(Game* game, const string& type, int id) :
    game_(game),
	type_(type)
{
	ostringstream os;
	os << type << id;
	name_ = os.str();

	node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name_);

	
	loadScriptCallbacks();
}

Enemy::~Enemy() {

	lua_State* env = game_->getScriptState();
	lua_unref(env, table_);

	// Destroy all attached entities and scene nodes
	SceneNode::ChildNodeIterator i = node_->getChildIterator();
	while (i.hasMoreElements()) {
		SceneNode* node = static_cast<SceneNode*>(i.getNext());
		MovableObject* obj = node->getAttachedObject(0U);
		node->detachAllObjects();
		node->getCreator()->destroyMovableObject(obj->getName(), obj->getMovableType());
	}
	node_->removeAndDestroyAllChildren();
	node_->getParentSceneNode()->removeAndDestroyChild(node_->getName());
}

void Enemy::loadScriptCallbacks() {
	lua_State* env = game_->getScriptState();

	// Create the peer Lua object
	lua_getglobal(env, type_.c_str());
	lua_getfield(env, -1, "new");
	lua_pushvalue(env, -2);
	lua_remove(env, -3);

	lua_newtable(env);

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Enemy::luaAddEntity, 1);
	lua_setfield(env, -2, "addEntity");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Enemy::luaSetEntity, 1);
	lua_setfield(env, -2, "setEntity");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Enemy::luaAddParticleSystem, 1);
	lua_setfield(env, -2, "addParticleSystem");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Enemy::luaSetParticleSystem, 1);
	lua_setfield(env, -2, "setParticleSystem");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Enemy::luaSet, 1);
	lua_setfield(env, -2, "set");

	// Call <name>:new()
	if (lua_pcall(env, 2, 1, 0)) {
		string message(lua_tostring(env, -1));
		lua_pop(env, 1);
		throw runtime_error("Error creating enemy: " + message);
	}

	table_ = lua_ref(env, -1);
}

/** Sets the scene node position and orientation */
int Enemy::luaSet(lua_State* env) {
	Enemy* self = (Enemy*)lua_touserdata(env, lua_upvalueindex(1));
	env >> *self->node_;

	return 0;
}

/** Adds an entity to the enemy in its own scene node */
int Enemy::luaAddEntity(lua_State* env) {
	Enemy* self = (Enemy*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		string name, mesh;
		lua_getfield(env, -1, "name");
		env >> name;
		lua_getfield(env, -1, "mesh");
		env >> mesh;

		name = self->name_ + "." + name;

		SceneNode* node = self->node_->createChildSceneNode(name);
		Entity* entity = self->game_->getSceneManager()->createEntity(name, mesh);
		node->attachObject(entity);

	} catch (Exception& ex) {
		lua_pushstring(env, ex.getFullDescription().c_str());
		lua_error(env);
	} catch (exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

/** Adds an entity to the enemy in its own scene node */
int Enemy::luaAddParticleSystem(lua_State* env) {
	Enemy* self = (Enemy*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		string name, templ;
		lua_getfield(env, -1, "name");
		env >> name;
		lua_getfield(env, -1, "template");
		env >> templ;

		name = self->name_ + "." + name;

		SceneNode* node = self->node_->createChildSceneNode(name);
		ParticleSystem* system = self->game_->getSceneManager()->createParticleSystem(name, templ);
		node->attachObject(system);

	} catch (Exception& ex) {
		lua_pushstring(env, ex.getFullDescription().c_str());
		lua_error(env);
	} catch (exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

/** Sets an entity */
int Enemy::luaSetEntity(lua_State* env) {
	Enemy* self = (Enemy*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		string name;
		lua_getfield(env, -1, "name");
		env >> name;

		lua_pushvalue(env, -1);
		name = self->name_ + "." + name;

		SceneNode* node = self->game_->getSceneManager()->getSceneNode(name);
		Entity* entity = static_cast<Entity*>(node->getAttachedObject(0U));

		lua_getfield(env, -1, "animation");
		if (!lua_isnil(env, -1)) {
			string animation;
			env >> animation;
			AnimationState* state = entity->getAnimationState(animation);
			state->setEnabled(true);
			state->setLoop(true);
			self->activeAnimations_.push_back(state);
		} else {
			lua_pop(env, 1);
		}

		env >> *node;
		env >> *entity;

	} catch (Exception& ex) {
		lua_pushstring(env, ex.getFullDescription().c_str());
		lua_error(env);
	} catch (exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}


/** Sets an entity */
int Enemy::luaSetParticleSystem(lua_State* env) {
	Enemy* self = (Enemy*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		string name;
		lua_getfield(env, -1, "name");
		env >> name;

		lua_pushvalue(env, -1);
		name = self->name_ + "." + name;

		SceneNode* node = self->game_->getSceneManager()->getSceneNode(name);

		env >> *node;

	} catch (Exception& ex) {
		lua_pushstring(env, ex.getFullDescription().c_str());
		lua_error(env);
	} catch (exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

/** Called when the game updates */
void Enemy::onTimeStep() {
	for (list<AnimationState*>::iterator i = activeAnimations_.begin(); i != activeAnimations_.end(); i++) {
		(*i)->addTime(0.01);
	}

	lua_State* env = game_->getScriptState();

	lua_getref(env, table_);
	lua_getfield(env, -1, "onTimeStep");
	lua_pushvalue(env, -2);
	lua_remove(env, -3);

	if (lua_pcall(env, 1, 0, 0)) {
		string message(lua_tostring(env, -1));
		lua_pop(env, 1);
		throw runtime_error("Error updating enemy: " + message);
	}
}

lua_State* Warp::operator>>(lua_State* env, Warp::Enemy& e) {
	lua_getref(env, e.table_);
	return env;
}