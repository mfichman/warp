/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "SubObject.hpp"

#include "Game.hpp"
#include "Object.hpp"

using namespace Warp;
using namespace Ogre;
using namespace std;


/** Creates a new subobject */
SubObject::SubObject(Game* game, Object* parent, const std::string& name, const std::string& mesh) :
	parent_(parent),
	game_(game),
	separated_(false),
	name_(name) {

	// Initialize the Ogre data
	entity_ = game->getSceneManager()->createEntity(name, mesh);
	node_ = parent->getSceneNode()->createChildSceneNode(name);
	node_->attachObject(entity_);
	node_->setUserAny(Any(this));

	// Initialize the physics shape
	const AxisAlignedBox& box = entity_->getMesh()->getBounds();
	btVector3 half(box.getHalfSize().x, box.getHalfSize().y, box.getHalfSize().z);
	shape_.reset(new btBoxShape(half));
}

/** Destructor */
SubObject::~SubObject() {

	// Destroy all attached entities and scene nodes
	unsigned short count = node_->numAttachedObjects();
	for (unsigned short i = 0; i < count; i++) {
		MovableObject* obj = node_->getAttachedObject(0U);
		node_->detachObject((unsigned short)0);
		node_->getCreator()->destroyMovableObject(obj->getName(), obj->getMovableType());
	}
	node_->getParentSceneNode()->removeAndDestroyChild(node_->getName());

	// Remove from world if the subobject was separated off
	if (body_.get()) { 
		body_->setUserPointer(0);
		game_->getWorld()->removeCollisionObject(body_.get()); 
	}
}

void SubObject::getWorldTransform(btTransform& transform) const {
	transform = transform_;
}

void SubObject::setWorldTransform(const btTransform& transform) {
	// Get info from bullet
    const btQuaternion& rotation = transform.getRotation();
    const btVector3& position = transform.getOrigin();
    // Apply to scene node
    node_->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    node_->setPosition(position.x(), position.y(), position.z());
    // Set local info
    transform_ = transform;
}

void SubObject::onTimeStep() {
	if (body_.get()) {
		body_->applyCentralForce(btVector3(0.0, -4.0, 0.0));
	}
}

void SubObject::separateFromParent() {
	if (separated_) {
		return;
	}
	separated_ = true;

	// Get current offset
	const Ogre::Vector3& v = node_->getPosition();
	const Ogre::Quaternion& q = node_->getOrientation();
	btTransform t1(btQuaternion(q.x, q.y, q.z, q.w), btVector3(v.x, v.y, v.z));
	const btTransform& t2 = parent_->getTransform();

	// Destroy old scene node
	MovableObject* obj = node_->getAttachedObject(0U);
	node_->detachAllObjects();
	node_->getParentSceneNode()->removeAndDestroyChild(node_->getName());

	// Create new scene node
	node_ = game_->getSceneManager()->getRootSceneNode()->createChildSceneNode(name_);
	node_->attachObject(obj);

	
	// Initialize the mass and inertia
	btScalar mass(1.0f);
	btVector3 inertia(0.0f, 0.0f, 0.0f);
	shape_->calculateLocalInertia(mass, inertia);

	// Set up the rigid body and add it to the world
	transform_.mult(t1, t2);
	btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, this, shape_.get(), inertia);
	body_.reset(new btRigidBody(rbinfo));
	body_->setFriction(1.0f);
	body_->setRestitution(1.0f);
	body_->setUserPointer(0);
	game_->getWorld()->addRigidBody(body_.get());

	// Shoot the object in a random direction
	float theta = Math::RangeRandom(-Math::PI, Math::PI);
	float phi = Math::RangeRandom(-Math::PI, Math::PI);
	btVector3 linvel(cosf(theta)*sinf(phi), sinf(theta)*cosf(phi), cosf(phi));
	linvel *= 10;
	body_->setLinearVelocity(linvel + parent_->getRigidBody()->getLinearVelocity());

	btVector3 axis(Math::RangeRandom(-1.0, 1.0), Math::RangeRandom(0, 5.0), Math::RangeRandom(-1.0, 1.0));
	body_->setAngularVelocity(axis);

	// Add flames
	SceneNode* node = node_->createChildSceneNode(name_ + ".Flames");
	ParticleSystem* system = game_->getSceneManager()->createParticleSystem(name_ + ".Flames", "Flames");
	node_->attachObject(system);
}