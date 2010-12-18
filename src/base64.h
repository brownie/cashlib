/* 
 * File:   base64.h
 * Author: kupcu
 *
 * Created on October 31, 2008, 6:04 PM
 */

#ifndef _BASE64_H
#define	_BASE64_H

#include "NTL/ZZ.h"
#include <string>

std::string base64_encode(/*unsigned*/ char const* , unsigned int len);
std::string base64_decode(std::string const& s);

/// base64-decode a string and turn it into a ZZ
inline void b64_to_ZZ(NTL::ZZ& z, const string& str) {
    if (!str.size())
		throw CashException(CashException::CE_IO_ERROR,
			"NTL::b64_to_ZZ: can't unserialize empty string");
    if (str == "0") { z = 0; return; }
	unsigned int negsign = (str[0] == '-') ? 1 : 0;
	z = NTL::ZZFromBytes(base64_decode(negsign ? str.substr(1) : str));
    if (negsign) { z = -z; }
}
inline NTL::ZZ b64_to_ZZ(const string& str) { NTL::ZZ z; b64_to_ZZ(z, str); return z; }

/// take a ZZ and return a base64-encoded string
inline void ZZ_to_b64(string &str, const NTL::ZZ& z) {
    if (z == 0) { str = "0"; return; }
	unsigned int blen = NTL::NumBytes(z); // NumBytes(0) => 0
	char buf[blen]; 
	NTL::BytesFromZZ((unsigned char *)&buf, z, blen);
	str = base64_encode(buf, blen);
    if (z < 0) { str = "-" + str; } // add sign if negative
}
inline string ZZ_to_b64(const NTL::ZZ& z) { string s; ZZ_to_b64(s, z); return s; }

#endif	/* _BASE64_H */

