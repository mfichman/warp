/******************************************************************************
 * Criterium: CS248 Final Project                                             *
 * Copyright (c) 2010 Matt Fichman                                            *
 ******************************************************************************/
#pragma once 

#include <boost/intrusive_ptr.hpp>
 
namespace Criterium {
using namespace boost;

class Interface {
public: 
	typedef intrusive_ptr<Interface> Ptr;
    
	inline int          refCount() const { return refCount_; }
	inline void         refCountDec() const { refCount_--; if (refCount_ <= 0) delete this; }
    inline void         refCountInc() const { refCount_++; }
    
    friend void intrusive_ptr_add_ref(Interface* t);
    friend void intrusive_ptr_release(Interface* t);

protected:
	Interface() : refCount_(0) {}
	virtual ~Interface() {}
    
private:
    mutable int         refCount_;
};

//------------------------------------------------------------------------------
inline void 
intrusive_ptr_add_ref(Interface* t) { 
    t->refCountInc(); 
}

//------------------------------------------------------------------------------
inline void 
intrusive_ptr_release(Interface* t) { 
    t->refCountDec();
}

}
