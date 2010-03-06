#include "Projectile.hpp"
#include "Game.hpp"
#include "Object.hpp"

using namespace Warp;
using namespace Ogre;
using namespace std;

Projectile::Projectile(Game* game, Level* level, const string& name, int id) :
	Object(game, level, name, id),
	hit_(false),
	time_(0.0f) {


	billboards_ = game_->getSceneManager()->createBillboardSet(name_ + ".Billboard", 1);
	billboards_->setBillboardRotationType(BBR_VERTEX);
	billboards_->setMaterialName("PhotonBlue");
	billboards_->setDefaultWidth(1.0);
	billboards_->setDefaultHeight(1.0);
	billboards_->createBillboard(0.0f, 0.0f, 0.0f);
	node_->attachObject(billboards_);

	shape_.reset(new btSphereShape(0.1));
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

	float width = billboard->getOwnWidth();
	float height = billboard->getOwnWidth();

	if (hit_) {
		if (target_) node_->setPosition(target_->getPosition());
		width = min(5.0, width + 0.2); // Grow the projectile
		height = min(5.0, height + 0.2);
	} else {
		width = 0.2 * sinf(time_) + 1; // Make the projectile oscillate in size
		height = 0.2 * sinf(time_) + 1;
	}

	billboard->setDimensions(width, height);
}

void Projectile::onCollision(Enemy* enemy) {
	if (static_cast<Object*>(enemy) == target_) {
		hit_ = true;
		cout << "collision hit" << endl;
		game_->getWorld()->removeCollisionObject(body_.get());
	}
}