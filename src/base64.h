/* 
 * File:   base64.h
 * Author: kupcu
 *
 * Created on October 31, 2008, 6:04 PM
 */

#ifndef _BASE64_H
#define	_BASE64_H

#include <string>

std::string base64_encode(/*unsigned*/ char const* , unsigned int len);
std::string base64_decode(std::string const& s);


#endif	/* _BASE64_H */

