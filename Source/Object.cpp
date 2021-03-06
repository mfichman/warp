/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Object.hpp"

#include "Game.hpp"
#include "SubObject.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "Projectile.hpp"
#include "ScriptTask.hpp"

#include <sstream>
#include <stdexcept>
 extern "C" {
#include <lua/lua.h> 
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

using namespace Warp;
using namespace Ogre;
using namespace std;
using namespace boost;

/** Initializes the OGRE scene nodes, and the attached rigid bodies */
Object::Object(Game* game, Level* level, const string& type, int id) :
    game_(game),
	type_(type),
	exploded_(false),
	alive_(true),
	level_(level),
	target_(0)
{
	ostringstream os;
	os << type << id;
	name_ = os.str();

	// Create scene node
	node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name_);

	// Initialize the physics shape
	shape_.reset(new btCompoundShape());

	// Initialize the mass and inertia
	mass_ = 1.0f;
	btScalar mass(mass_);
	btVector3 inertia(0.0f, 0.0f, 0.0f);
	shape_->calculateLocalInertia(mass, inertia);

	// Initialize the rigid body
	transform_ = btTransform(btQuaternion::getIdentity(), btVector3(0.0f, 0.0f, 0.0f));
	btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, this, shape_.get(), inertia);
	body_.reset(new btRigidBody(rbinfo));
	body_->setFriction(0.0f);
	body_->setRestitution(0.0f);
	body_->setUserPointer(this);
	game_->getWorld()->addRigidBody(body_.get());

	loadScriptCallbacks();
}

Object::~Object() {

	// Notify projectiles that the object is toast
	clearAllTrackers();
	//cout << "Collecting " << name_ << endl;

	// Un-track
	if (target_) target_->removeTracker(this);

	// Clean up physics
	game_->getWorld()->removeCollisionObject(body_.get());

	// Destroy all subobjects
	subObjects_.clear();

	// Destroy objects attached to the scene node
	while (node_->numAttachedObjects() > 0) {
		MovableObject* obj = node_->getAttachedObject(0U);
		node_->detachObject((unsigned short)0U);
		node_->getCreator()->destroyMovableObject(obj->getName(), obj->getMovableType());
	}

	// Destroy all attached entities and scene nodes
	SceneNode::ChildNodeIterator i = node_->getChildIterator();
	while (i.hasMoreElements()) {
		SceneNode* node = static_cast<SceneNode*>(i.getNext());
		MovableObject* obj = node->getAttachedObject(0U);
		node->detachObject((unsigned short)0U);
		node->getCreator()->destroyMovableObject(obj->getName(), obj->getMovableType());
	}
	node_->removeAndDestroyAllChildren();
	node_->getParentSceneNode()->removeAndDestroyChild(node_->getName());

	// Clean up scripting
	lua_State* env = game_->getScriptState();
	StackCheck check(env);
	lua_getref(env, table_);
	
	lua_pushnil(env);
	lua_setfield(env, -2, "__thiscpp");

	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "addEntity");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "setEntity");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "addParticleSystem");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "setParticleSystem");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "set");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "explode");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "destroy");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "target");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "setPosition");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "getPosition");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "setVelocity");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "getVelocity");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "setOrientation");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "getOrientation");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "createMissile");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "getTarget");
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "setTarget");
	lua_unref(env, table_);
}

/** Called when the game updates */
void Object::onTimeStep() {
	for (std::list<AnimationState*>::iterator i = activeAnimations_.begin(); i != activeAnimations_.end(); i++) {
		(*i)->addTime(game_->getTimeStep());
	}

	callMethod("onTimeStep"); // destruction also handled in lua

	const SpineProjection proj = level_->getPlayer()->getPlayerProjection();
	Vector3 to = proj.position - node_->getPosition();

	// If the players forward vector is facing away from us, then the object is
	// out of the field of view so this object should be deleted.  N.B.:
	// there is probably a better way to do this using the distance along the
	// spine node path.
	//HACKHACKAHCK
	if (to.dotProduct(proj.forward) > 0 && to.length() > 100 && name_ != "Player0") {
		alive_ = false;
		//cout << "Killing " << name_ << endl;
	}

	for (std::list<SubObjectPtr>::iterator i = subObjects_.begin(); i != subObjects_.end(); i++) {
		(*i)->onTimeStep();
	}
}

void Object::getWorldTransform(btTransform& transform) const {
	transform = transform_;
}

void Object::setWorldTransform(const btTransform& transform) {

	// Get info from bullet
    const btQuaternion& rotation = transform.getRotation();
    const btVector3& origin = transform.getOrigin();
    // Apply to scene node
    node_->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    node_->setPosition(origin.x(), origin.y(), origin.z());
    // Set local info
    transform_ = transform;
}

void Object::setTarget(ObjectPtr target) {
	if (target_ == target) return;

	if (target_) {
		target_->removeTracker(this);
	}
	target_ = target;
	if (target_) {
		target_->addTracker(this);
	}
}

void Object::addTracker(TrackerPtr p) {
	if (!alive_ || exploded_) {
		p->onTargetDelete(this);
	} else if (p) {
		trackers_.insert(p);
	}
}

void Object::removeTracker(TrackerPtr p) {
	trackers_.erase(p);
}

void Object::clearAllTrackers() {
	for (std::set<TrackerPtr>::iterator i = trackers_.begin(); i != trackers_.end(); i++) {
		(*i)->onTargetDelete(this);
	}
	trackers_.clear();
}

void Object::setPosition(const Ogre::Vector3& p) {
	btTransform transform = body_->getCenterOfMassTransform();
	transform.setOrigin(btVector3(p.x, p.y, p.z));
	body_->setCenterOfMassTransform(transform);
    setWorldTransform(transform);
}

void Object::setOrientation(const Ogre::Quaternion& q) {
	btTransform transform = body_->getCenterOfMassTransform();
    transform.setRotation(btQuaternion(q.x, q.y, q.z, q.w));
	body_->setCenterOfMassTransform(transform);
    //setWorldTransform(transform);
    node_->setOrientation(q);
}

void Object::onTargetDelete(ObjectPtr target) {
	target_.reset();
	alive_ = false;
	callMethod("onTargetDelete");
}

Vector3 Object::getVelocity() {
	const btVector3& vel = body_->getLinearVelocity();
	return Vector3(vel.x(), vel.y(), vel.z());
}

void Object::setVelocity(const Vector3& velocity) {
	body_->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}


lua_State* Warp::operator<<(lua_State* env, Warp::ObjectPtr e) {
	lua_getref(env, e->table_);
	return env;
}

void Object::loadScriptCallbacks() {
	lua_State* env = game_->getScriptState();
	StackCheck check(env);

	// Create the peer Lua object
	lua_getglobal(env, type_.c_str());
	lua_getfield(env, -1, "new");
	lua_pushvalue(env, -2);
	lua_remove(env, -3);

	lua_newtable(env);

	lua_pushlightuserdata(env, this);
	lua_setfield(env, -2, "__thiscpp");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaAddEntity, 1);
	lua_setfield(env, -2, "addEntity");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaSetEntity, 1);
	lua_setfield(env, -2, "setEntity");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaAddParticleSystem, 1);
	lua_setfield(env, -2, "addParticleSystem");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaSetParticleSystem, 1);
	lua_setfield(env, -2, "setParticleSystem");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaExplode, 1);
	lua_setfield(env, -2, "explode");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaDestroy, 1);
	lua_setfield(env, -2, "destroy");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaGetPosition, 1);
	lua_setfield(env, -2, "getPosition");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaSetPosition, 1);
	lua_setfield(env, -2, "setPosition");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaGetVelocity, 1);
	lua_setfield(env, -2, "getVelocity");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaSetVelocity, 1);
	lua_setfield(env, -2, "setVelocity");

    lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaGetOrientation, 1);
	lua_setfield(env, -2, "getOrientation");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaSetOrientation, 1);
	lua_setfield(env, -2, "setOrientation");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaCreateMissile, 1);
	lua_setfield(env, -2, "createMissile");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaGetTarget, 1);
	lua_setfield(env, -2, "getTarget");

	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Object::luaSetTarget, 1);
	lua_setfield(env, -2, "setTarget");

	// Call <name>:new()
	if (lua_pcall(env, 2, 1, 0)) {
		string message(lua_tostring(env, -1));
		throw runtime_error("Error creating Object: " + message);
	}

	table_ = lua_ref(env, -1);
}

/** Adds an entity to the Object in its own scene node */
int Object::luaAddEntity(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		string name, mesh;
		lua_getfield(env, -1, "name");
		env >> name;
		lua_getfield(env, -1, "mesh");
		env >> mesh;
		lua_getfield(env, -1, "mass");
		float mass = (float)lua_tonumber(env, -1);
		lua_pop(env, 1);

		// Create the subobject and add it to this object
		name = self->name_ + "." + name;
		SubObjectPtr subobj(new SubObject(self->game_, self, name, mesh));
		self->subObjects_.push_back(subobj);


		
		const Vector3 v = subobj->getOffsets();
		btTransform transform(btQuaternion::getIdentity(), btVector3(v.x, v.y, v.z));

		self->shape_->addChildShape(transform, subobj->getShape());
		self->body_->updateInertiaTensor();

		lua_getfield(env, -1, "animation");
		if (!lua_isnil(env, -1)) {
			string animation;
			env >> animation;
			AnimationState* state = subobj->getEntity()->getAnimationState(animation);
			state->setEnabled(true);
			state->setLoop(true);
			self->activeAnimations_.push_back(state);
		} else {
			lua_pop(env, 1);
		}

	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

/** Adds an entity to the Object in its own scene node */
int Object::luaAddParticleSystem(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		string name, templ;
		lua_getfield(env, -1, "name");
		env >> name;
		lua_getfield(env, -1, "template");
		env >> templ;
		lua_getfield(env, -1, "position");
		Vector3 position;
		if (!lua_isnil(env, -1)) {
			env >> position;
		} else {
			lua_pop(env, 1);
		}
		

		name = self->name_ + "." + name;

		SceneNode* node = self->node_->createChildSceneNode(name);
		node->setPosition(position);
		ParticleSystem* system = self->game_->getSceneManager()->createParticleSystem(name, templ);
		node->attachObject(system);

		lua_getfield(env, -1, "on");
		if (!lua_isnil(env, -1)) {
#pragma warning(disable:4800)
			for (int i = 0; i < system->getNumEmitters(); i++ ) {
				system->getEmitter(i)->setEnabled(lua_toboolean(env, -1));
			}
#pragma warning(default:4800)
		}
		lua_pop(env, 1);

	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

int Object::luaCreateMissile(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		string type;
		lua_getfield(env, -1, "type");
		env >> type;


		ProjectilePtr p = self->level_->createProjectile(type);
		p->setPosition(self->getPosition());

		lua_getfield(env, -1, "velocity");
		if (lua_istable(env, -1)) {
			Vector3 velocity;
			env >> velocity;
			p->setVelocity(velocity);
		} else {
			lua_pop(env, 1);
		}

        env << p;
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 1;
}

/** Sets an entity */
int Object::luaSetEntity(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		string name;
		lua_getfield(env, -1, "name");
		env >> name;
		name = self->name_ + "." + name;

		SceneNode* node = self->game_->getSceneManager()->getSceneNode(name);
		Entity* entity = static_cast<Entity*>(node->getAttachedObject(0U));
		SubObject* subobj = any_cast<SubObject*>(node->getUserAny());

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

		env >> *entity;
		env >> *node;

		// Recalculate the physics for collision detection
		const Quaternion& q = node->getOrientation();
		const Vector3 v = node->getPosition() + subobj->getOffsets();
		btTransform transform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(v.x, v.y, v.z));

		self->shape_->removeChildShape(subobj->getShape());
		self->shape_->addChildShape(transform, subobj->getShape());
		self->body_->updateInertiaTensor();

	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}


/** Sets an entity */
int Object::luaSetParticleSystem(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		string name;
		lua_getfield(env, -1, "name");
		env >> name;
		name = self->name_ + "." + name;

		SceneNode* node = self->game_->getSceneManager()->getSceneNode(name);
		//env >> *node;

		ParticleSystem* psys = dynamic_cast<ParticleSystem*>(node->getAttachedObject(0U));
		if (psys) {
			lua_getfield(env, -1, "width");
			lua_getfield(env, -2, "height");
			if (!lua_isnil(env, -1) && !lua_isnil(env, -2)) {
				float width = (float)lua_tonumber(env, -1);
				float height = (float)lua_tonumber(env, -1);
				psys->setDefaultDimensions(width, height);
			}
			lua_pop(env, 2);

			lua_getfield(env, -1, "velocity");
			if (!lua_isnil(env, -1)) {
				float velocity = (float)lua_tonumber(env, -1);
				psys->getEmitter(0U)->setParticleVelocity(velocity);
			}
			lua_pop(env, 1);

			lua_getfield(env, -1, "on");
			if (!lua_isnil(env, -1)) {
#pragma warning(disable:4800)
				for (int i = 0; i < psys->getNumEmitters(); i++ ) {
					psys->getEmitter(i)->setEnabled(lua_toboolean(env, -1));
				}
#pragma warning(default:4800)
			}
			lua_pop(env, 1);
		} else {
			cout << "Warning, particle system does not exist" << endl;
		}

	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

/** Explodes the object */
int Object::luaExplode(lua_State* env) {
	try {
		Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));
		if (self->exploded_) {
			return 0;
		}
		self->exploded_ = true;

		// Separate the subobjects
		for (std::list<SubObjectPtr>::iterator i = self->subObjects_.begin(); i != self->subObjects_.end(); i++) {
			(*i)->separateFromParent();
		}

		// Disable the original rigid body for the object
		self->game_->getWorld()->removeCollisionObject(self->body_.get());
		self->clearAllTrackers();
		self->node_->setVisible(false);
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

int Object::luaGetTarget(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));
	if (self->target_) {
		env << self->target_;
	} else {
		lua_pushnil(env);
	}

	return 1;
}

int Object::luaSetTarget(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));
	
	if (!lua_istable(env, -1)) {
		lua_pushstring(env, "Expected a table");
		lua_error(env);
	}
	lua_getfield(env, -1, "__thiscpp");
	if (lua_isnil(env, -1)) {
		lua_pushstring(env, "Expected a C++ object");
		lua_error(env);
	}
	Object* target = (Object*)lua_touserdata(env, -1);
	self->setTarget(target);

	return 0;
}

/** Destroys the object */
int Object::luaDestroy(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));
	self->alive_ = false;
	return 0;
}

/** Warning */
int Object::luaWarningDestroyed(lua_State* env) {
	cout << "This object is no longer valid" << endl;
	return 0;
}

/** Return position */
int Object::luaGetPosition(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));
	env << self->getPosition();
	return 1;
}

int Object::luaSetPosition(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));
	try {
		Vector3 position;
		env >> position;
		self->setPosition(position);
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
	return 0;
}

int Object::luaGetVelocity(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));
	env << self->getVelocity();
	return 1;
}

int Object::luaSetVelocity(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));
	try {
		Vector3 velocity;
		env >> velocity;
		self->setVelocity(velocity);
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
	return 0;
}

/** Return quaternion corresponding to the rotation */
int Object::luaGetOrientation(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));
	env << self->getOrientation();
	return 1;
}

int Object::luaSetOrientation(lua_State* env) {
	Object* self = (Object*)lua_touserdata(env, lua_upvalueindex(1));
	try {
		Quaternion orientation;
		env >> orientation;
		self->setOrientation(orientation);
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}
	return 0;
}

/** Calls a method on the peer Lua object */
void Object::callMethod(const std::string& method) {
	lua_State* env = game_->getScriptState();
	StackCheck check(env);

	lua_getglobal(env, "debug"); // Push the debuggger
    lua_getfield(env, -1, "traceback");
    lua_remove(env, -2);

	lua_getref(env, table_);
	lua_getfield(env, -1, method.c_str());
	if (!lua_isfunction(env, -1)) {
		return;
	}

	lua_pushvalue(env, -2); // Push the table to front
	lua_remove(env, -3); // Remove the table

	if (lua_pcall(env, 1, 0, -3)) {
		string message(lua_tostring(env, -1));
		throw runtime_error(message);
	}
}


Ogre::Vector3 Object::getPosition() const {
	btVector3 pos = body_->getCenterOfMassPosition();
	return Vector3(pos.x(), pos.y(), pos.z());
}
