/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/

#include <PickingRay.hpp>
#include <Game.hpp>
#include <Objects.hpp>

using namespace Criterium;
using namespace Ogre;
using namespace std;

struct PickingRay::Impl : public Game::Listener, public Game::Collidable {

    void init() {
        ray_ = dCreateRay(game_->getSpace(), 10000);
        dGeomSetData(ray_, static_cast<Game::Collidable*>(this));
        dGeomSetCategoryBits(ray_, Objects::TYPE_RAY);
        dGeomSetCollideBits(ray_, Objects::TYPE_BALL);
    }

    void onTimeStep() {

        // Create the picking ray
        //const OIS::MouseState& mouse = game_->getMouse()->getMouseState();
        Ray ray = game_->getCamera()->getCameraToViewportRay(0.5, 0.5);
        dGeomRaySet(ray_, ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z, ray.getDirection().x, ray.getDirection().y, ray.getDirection().z);

        Vector3 position = ray.getPoint(depth_);
        if (!game_->getMouse()->getMouseState().buttonDown(OIS::MB_Right) && object_) {
            Vector3 velocity = 10 * (position - last_);
            dBodySetLinearVel(dGeomGetBody(object_), velocity.x, velocity.y, velocity.z);
            object_ = 0;
            depth_ = 0;
        } else if (object_) {
            dBodySetLinearVel(dGeomGetBody(object_), 0.0f, 0.0f, 0.0f);
            dGeomSetPosition(object_, position.x, position.y, position.z);

            last_ = position;
        }
    }

    void onCollision(dGeomID other, dContactGeom& contact) {
       
        if (!object_ && game_->getMouse()->getMouseState().buttonDown(OIS::MB_Right) && dGeomGetCategoryBits(other) == Objects::TYPE_BALL) {
            dVector3 origin;
            dVector3 dir;
            dGeomRayGet(ray_, origin, dir);

            Vector3 target(contact.pos);
            object_ = other;
            depth_ = target.distance(Vector3(origin));

            //Vector3 point = depth_ * Vector3(dir);
            //Vector3 position(dGeomGetPosition(other));
            ///offset_ = position - point;
        }
    }

    Game* game_;
    dGeomID ray_;
    dGeomID object_;
    float depth_;
    //Vector3 offset_;
    Vector3 last_;
};

PickingRay::PickingRay(Game* game) : impl_(new Impl()) {
    impl_->game_ = game;
    impl_->object_ = 0;
    impl_->game_->addListener(impl_.get());
    impl_->init();
}

PickingRay::~PickingRay() {

}