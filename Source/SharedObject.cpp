/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/

#include "SharedObject.hpp"
#include "Enemy.hpp"
#include "Player.hpp"
#include "Projectile.hpp"

using namespace Warp;
using namespace std;

void Warp::intrusive_ptr_add_ref(Enemy* p) {
	intrusive_ptr_add_ref(static_cast<SharedObject*>(p));
}

void Warp::intrusive_ptr_release(Enemy* p) {
	intrusive_ptr_release(static_cast<SharedObject*>(p));
}

void Warp::intrusive_ptr_add_ref(Player* p) {
	intrusive_ptr_add_ref(static_cast<SharedObject*>(p));
}

void Warp::intrusive_ptr_release(Player* p) {
	intrusive_ptr_release(static_cast<SharedObject*>(p));
}

void Warp::intrusive_ptr_add_ref(Projectile* p) {
	intrusive_ptr_add_ref(static_cast<SharedObject*>(p));
}

void Warp::intrusive_ptr_release(Projectile* p) {
	intrusive_ptr_release(static_cast<SharedObject*>(p));
}