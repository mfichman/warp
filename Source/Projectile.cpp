#include "Projectile.hpp"
#include "Game.hpp"
#include "Object.hpp"
#include "Player.hpp"
#include "ScriptTask.hpp"
extern "C" {
#include <lua/lua.h> 
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}


using namespace Warp;
using namespace Ogre;
using namespace std;
using namespace boost;

Projectile::Projectile(Game* game, Level* level, const string& name, int id) :
	Object(game, level, name, id),
	hit_(false),
	time_(0.0f) {

	billboards_ = game_->getSceneManager()->createBillboardSet(name_ + ".Billboard0", 1);
	billboards_->setBillboardRotationType(BBR_VERTEX);
	billboards_->setMaterialName("PhotonBlue");
	billboards_->setDefaultWidth(1.0);
	billboards_->setDefaultHeight(1.0);
	billboards_->createBillboard(0.0f, 0.0f, 0.0f);
	node_->attachObject(billboards_);

	shape_.reset(new btSphereShape(0.5));
	body_->setCollisionShape(shape_.get());
	body_->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	loadScriptCallbacks();
}

/** Destructor */
Projectile::~Projectile() {
	lua_State* env = game_->getScriptState();
	StackCheck check(env);
	lua_getref(env, table_);
	lua_pushcclosure(env, &Object::luaWarningDestroyed, 0);
	lua_setfield(env, -2, "setMaterial");
}

void Projectile::loadScriptCallbacks() {
	lua_State* env = game_->getScriptState();
	StackCheck check(env);
	lua_getref(env, table_);
	lua_pushlightuserdata(env, this);
	lua_pushcclosure(env, &Projectile::luaSetMaterial, 1);
	lua_setfield(env, -2, "setMaterial");
}

int Projectile::luaSetMaterial(lua_State* env) {
	Projectile* self = (Projectile*)lua_touserdata(env, lua_upvalueindex(1));

	try {
		string material;
		env >> material;
		self->billboards_->setMaterialName(material);
	} catch (std::exception& ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}

void Projectile::onTimeStep() {
	Object::onTimeStep();

	Billboard* billboard = billboards_->getBillboard(0);
	billboard->setRotation(billboard->getRotation() + Radian(0.2f));

	time_ += 0.1f;

	float width = billboard->getOwnWidth();
	float height = billboard->getOwnWidth();

	if (hit_) {
		if (target_) node_->setPosition(target_->getPosition());
		width = min(2.0f, width + 0.2f); // Grow the projectile
		height = min(2.0f, height + 0.2f);
	} else {
		width = 0.2f * sinf(time_) + 1.0f; // Make the projectile oscillate in size
		height = 0.2f * sinf(time_) + 1.0f;
	}

	billboard->setDimensions(width, height);
}

void Projectile::collide(ObjectPtr other) { 
	if (other == target_) {
		other->onCollision(this);
	}
}

void Projectile::onCollision(EnemyPtr enemy) {
	if (static_pointer_cast<Object>(enemy) == target_) {
		callMethod("onTargetHit");
		hit_ = true;
		game_->getWorld()->removeCollisionObject(body_.get());
	}
}

void Projectile::onCollision(PlayerPtr player) {
	if (static_pointer_cast<Object>(player) == target_) {
		callMethod("onTargetHit");
		hit_ = true;
		game_->getWorld()->removeCollisionObject(body_.get());
	}
}