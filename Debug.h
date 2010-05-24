/*
 * File:   Debug.h
 * Author: kupcu
 *
 * Created on October 5, 2008, 2:40 PM
 */

#ifndef _DEBUG_H
#define	_DEBUG_H

#include "CommonFunctions.h"
#include <boost/lexical_cast.hpp>

//#define DEBUG
#define TIMER

namespace __gnu_cxx {
	template<> struct hash<std::string> {
		size_t operator()(const std::string &s) const {
			return hash<const char*>()(s.c_str());
		};
	};

	template<> struct hash< std::pair<std::string, int> > {
		size_t operator()(const std::pair<std::string, int> &p) const {
			return hash<const char*>()((p.first + boost::lexical_cast<string>(p.second)).c_str());
		};
	};
};

#endif	/* _DEBUG_H */

