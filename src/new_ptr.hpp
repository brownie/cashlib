#ifndef __NEW_PTR_HPP__
#define __NEW_PTR_HPP__

// wrapper for shared_ptr
//
// could also use tr1::shared_ptr
// http://gcc.gnu.org/onlinedocs/libstdc++/manual/shared_ptr.html

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

// XXX use brownie namespace
#define Ptr boost::shared_ptr
#define new_ptr boost::make_shared

#endif
