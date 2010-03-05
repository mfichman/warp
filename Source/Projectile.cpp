#include "Projectile.hpp"
#include "Game.hpp"
#include "Object.hpp"

#define PROJECTILE_RADIUS 1.0f
#define PROJECTILE_SPEED 100.0f;

using namespace Warp;
using namespace Ogre;
using namespace std;

Projectile::Projectile(Game* game, const std::string& material, int id, const Vector3& pos, Object* target) :
	game_(game),
	alive_(true),
	target_(target),
	time_(0),
	hit_(false) {

	ostringstream os;
	os << id;
	name_ = "Projectile" + os.str();

	// Init particle billboard
	node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name_);
	billboards_ = game_->getSceneManager()->createBillboardSet(name_ + "Billboard", 1);
	billboards_->setBillboardRotationType(BBR_VERTEX);
	billboards_->setMaterialName(material);
	billboards_->setDefaultWidth(1.0);
	billboards_->setDefaultHeight(1.0);
	billboards_->createBillboard(0.0f, 0.0f, 0.0f);
	node_->attachObject(billboards_);

	// Initialize physics shape
	shape_.reset(new btSphereShape(PROJECTILE_RADIUS));
		
	btScalar mass(0.001f);
	btVector3 inertia(0.0f, 0.0f, 0.0f);
	shape_->calculateLocalInertia(mass, inertia);

	transform_ = btTransform(btQuaternion::getIdentity(), btVector3(pos.x, pos.y, pos.z));
	btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, this, shape_.get(), inertia);
	body_.reset(new btRigidBody(rbinfo));
	body_->setFriction(0.0f);
	body_->setRestitution(0.0f);
	body_->setUserPointer(static_cast<Collidable*>(this));
	body_->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

	game_->getWorld()->addRigidBody(body_.get());

}

/** Destructor */
Projectile::~Projectile() {
	game_->getWorld()->removeCollisionObject(body_.get());
	node_->detachObject(billboards_);
	node_->getParentSceneNode()->removeAndDestroyChild(node_->getName());
	game_->getSceneManager()->destroyBillboardSet(billboards_);
	if (target_) target_->removeProjectile(this);
}

/* Called when the tracked object changes position */
void Projectile::onTargetMovement(const Ogre::Vector3& newPosition) {
	if (!hit_) {
		Vector3 velocity = newPosition - node_->getPosition();
		velocity.normalise();
		velocity *= PROJECTILE_SPEED;
		body_->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
	} else {
		node_->setPosition(newPosition);
		Billboard* billboard = billboards_->getBillboard(0);
		float width = billboard->getOwnWidth();
		float height = billboard->getOwnWidth();
		width = min(5.0, width + 0.1);
		height = min(5.0, height + 0.1);
		billboard->setDimensions(width, height);
	}
}

/* Called when the tracked object is removed from memory */
void Projectile::onTargetDelete() {
	alive_ = false;
	target_ = 0;
}


// Bullet callbacks
void Projectile::getWorldTransform(btTransform& transform) const {
	transform = transform_;
}

void Projectile::setWorldTransform(const btTransform& transform) {
	// Get info from bullet
    const btQuaternion& rotation = transform.getRotation();
    const btVector3& origin = transform.getOrigin();
    // Apply to scene node
    node_->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    node_->setPosition(origin.x(), origin.y(), origin.z());
    // Set local info
    transform_ = transform;
}

void Projectile::onCollision(Object* object) {
	if (object == target_) {
		game_->getWorld()->removeCollisionObject(body_.get());
		hit_ = true;
	}
}

void Projectile::onTimeStep() {
	Billboard* billboard = billboards_->getBillboard(0);
	billboard->setRotation(billboard->getRotation() + Radian(0.2));

	time_ += 0.6f;

	float width = billboard->getOwnWidth();
	float height = billboard->getOwnWidth();
	width = 0.2 * sinf(time_) + 1;
	height = 0.2 * sinf(time_) + 1;

	billboard->setDimensions(width, height);
	if (target_) onTargetMovement(target_->getPosition());
}