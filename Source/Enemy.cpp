/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "Enemy.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "Game.hpp"
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
Enemy::Enemy(Game* game, Level* level, const string& type, int id) :
	Object(game, level, type, id),
	selected_(false),
	spineNodeIndex_(0),
	billboards_(0),
	hitCount_(0), 
	hitPoints_(1),
	finalHitCount_(0) {

	// Find the spawn position
	const SpineProjection& spawn = level_->getPlayer()->getSpawnProjection();


	// TODO: Align the object so it's facing down the tube
	setPosition(spawn.position + Vector3(Math::RangeRandom(-3.0, 3.0), Math::RangeRandom(-3.0, 3.0), Math::RangeRandom(-3.0, 3.0)));
	setSpeed(40);
	setTarget(level_->getPlayer());

	loadScriptCallbacks();

	
}

Enemy::~Enemy() {
	lua_State* env = game_->getScriptState();
	lua_getref(env, table_);
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "target");
	lua_pop(env, 1);
}

/** Called when the object is selected */
void Enemy::setSelected(bool selected) {
	if (selected == selected_) return;

	if (selected) {
		selected_ = true;
		callMethod("onSelect");
	} else {
		selected_ = false;
		callMethod("onDeselect");
	}
}

void Enemy::onCollision(Projectile* p) {
	callMethod("onProjectileHit");

	finalHitCount_++;
	if (finalHitCount_ == hitPoints_) {
		callMethod("onDestroy");
	}
	if (billboards_) {
		node_->detachObject(billboards_);
		game_->getSceneManager()->destroyBillboardSet(billboards_);
		billboards_ = 0;
	}
}

void Enemy::onCollision(Player* p) {
	callMethod("onPlayerHit");
}

void Enemy::setWorldTransform(const btTransform& transform) {

	// Get info from bullet
    const btQuaternion& rotation = transform.getRotation();
    const btVector3& origin = transform.getOrigin();
    // Apply to scene node
    //node_->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    node_->setPosition(origin.x(), origin.y(), origin.z());
    // Set local info
    transform_ = transform;

	btVector3 btvelocity = body_->getLinearVelocity();
	Vector3 velocity(btvelocity.x(), btvelocity.y(), btvelocity.z());
	velocity.normalise();
	Vector3 right = velocity.crossProduct(Vector3::UNIT_Y);
	Vector3 forward = right.crossProduct(Vector3::UNIT_Y);
	node_->setOrientation(Quaternion(right, Vector3::UNIT_Y, forward));
}

void Enemy::loadScriptCallbacks() {
	lua_State* env = game_->getScriptState();
	lua_getref(env, table_);
	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Enemy::luaTarget, 1);
	lua_setfield(env, -2, "target");
	
	lua_getfield(env, -1, "hitPoints");
	hitPoints_ = max(1, lua_tointeger(env, -1));
	lua_pop(env, 1);
	
	lua_pop(env, 1);
}

/** Destroys the object */
int Enemy::luaTarget(lua_State* env) {
	Enemy* self = (Enemy*)lua_touserdata(env, lua_upvalueindex(1));
	
	if (!self->billboards_) {
		self->billboards_ = self->game_->getSceneManager()->createBillboardSet(self->name_ + ".Target", 1);
		self->billboards_->setMaterialName("Circle");
		self->billboards_->setDefaultWidth(5.0f);
		self->billboards_->setDefaultHeight(5.0f);
		self->billboards_->createBillboard(0.0f, 0.0f, 0.0f);
		self->node_->attachObject(self->billboards_);
	}

	return 0;
} 