#include "Projectile.hpp"
#include "Game.hpp"
#include "Object.hpp"

using namespace Warp;
using namespace Ogre;
using namespace std;
using namespace boost;

Projectile::Projectile(Game* game, Level* level, const string& name, int id) :
	Object(game, level, name, id),
	hit_(false),
	time_(0.0f),
	immunity_(0.6f) {

	billboards_ = game_->getSceneManager()->createBillboardSet(name_ + ".Billboard", 1);
	billboards_->setBillboardRotationType(BBR_VERTEX);
	billboards_->setMaterialName("PhotonBlue");
	billboards_->setDefaultWidth(1.0);
	billboards_->setDefaultHeight(1.0);
	billboards_->createBillboard(0.0f, 0.0f, 0.0f);
	node_->attachObject(billboards_);

	shape_.reset(new btSphereShape(1.5));
	body_->setCollisionShape(shape_.get());
	body_->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

/** Destructor */
Projectile::~Projectile() {

}


void Projectile::onTimeStep() {
	Object::onTimeStep();

	Billboard* billboard = billboards_->getBillboard(0);
	billboard->setRotation(billboard->getRotation() + Radian(0.2));

	time_ += 0.6f;

	if (immunity_ > 0.0f) {
		immunity_ -= 0.01f;
	}

	float width = billboard->getOwnWidth();
	float height = billboard->getOwnWidth();

	if (hit_) {
		if (target_) node_->setPosition(target_->getPosition());
		width = min(2.0, width + 0.2); // Grow the projectile
		height = min(2.0, height + 0.2);
	} else {
		width = 0.2 * sinf(time_) + 1; // Make the projectile oscillate in size
		height = 0.2 * sinf(time_) + 1;
	}

	billboard->setDimensions(width, height);

	if (target_) {
		/*float speed = getVelocity().length();
		if (getPosition().distance(target_->getPosition()) < speed * 1.5) {
			// Projectile is close enough, so fake a collision
			collide(target_);
		}*/
	}
}


void Projectile::onCollision(EnemyPtr enemy) {
	if (immunity_ > 0.0f) return;
	if (static_pointer_cast<Object>(enemy) == target_) {
		hit_ = true;
		game_->getWorld()->removeCollisionObject(body_.get());
	} else if (!target_) {
		hit_ = true;
		game_->getWorld()->removeCollisionObject(body_.get());
	}
}

void Projectile::onCollision(PlayerPtr player) {
	if (immunity_ > 0.0f) return;
	if (!target_) {
		hit_ = true;
		game_->getWorld()->removeCollisionObject(body_.get());
	}
}