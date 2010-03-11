/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include "Warp.hpp"

namespace Warp {

class SharedObject {
public:
	virtual ~SharedObject() {}

	SharedObject() : refCount_(0) {}
	friend void intrusive_ptr_add_ref(SharedObject* p);
	friend void intrusive_ptr_release(SharedObject* p);

private:
	mutable int refCount_;

};

inline void intrusive_ptr_add_ref(SharedObject* p) {
	p->refCount_++;
}

inline void intrusive_ptr_release(SharedObject* p) {
	p->refCount_--;
	if (p->refCount_ <= 0) {
		delete p;
	}
}

void intrusive_ptr_add_ref(Enemy* p);
void intrusive_ptr_release(Enemy* p);
void intrusive_ptr_add_ref(Player* p);
void intrusive_ptr_release(Player* p);
void intrusive_ptr_add_ref(Projectile* p);
void intrusive_ptr_release(Projectile* p);

}