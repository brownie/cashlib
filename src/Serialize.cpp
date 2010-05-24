#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/hash_map.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/iostreams/filtering_stream.hpp> 
#include <boost/iostreams/filter/gzip.hpp> 
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/file.hpp> 
#include <boost/iostreams/stream.hpp>
#include <string>
#include <iostream>
#include <strstream>
#include <sstream>
#include <fstream>
#include "Serialize_map.h"

/* NVP stands for "name-value pair" and is used for naming variables
 *  in archives (i.e. for XML or more legible text archives)
 *
 * Usage: 
 *  void serialize(Archive& ar, const unsigned int ver) {
 *     ar & make_nvp("stat", stat); // load/save stat using name "stat"
 *     ar & auto_nvp(stat);         // does the same thing as above
 *     ar & stat;                   // just serialize this data with no name
 *  }
 */
#define auto_nvp BOOST_SERIALIZATION_NVP
#define base_object_nvp BOOST_SERIALIZATION_BASE_OBJECT_NVP
using boost::serialization::make_nvp;

namespace bsz = boost::serialization;
namespace bar = boost::archive;
namespace bio = boost::iostreams;

/* wrappers to save/load any boost-serializable object to/from a string or file */

template <class T> inline std::string saveString(const T & o) {
	std::ostringstream oss;
	bar::binary_oarchive oa(oss);
	oa << o;
	return oss.str();
}

template <class T> inline void loadString(T & o, const std::string& s) {
	std::istringstream iss(s);
	bar::binary_iarchive ia(iss);
	ia >> o;
}

template <class T> inline std::string saveGZString(const T & o) {
	std::ostringstream oss;
	{ 
		bio::filtering_stream<bio::output> f;
		f.push(bio::gzip_compressor());
		f.push(oss);
		bar::binary_oarchive oa(f);
		oa << o;
	} // gzip_compressor flushes when f goes out of scope
	return oss.str();
}
template <class T> inline void loadGZString(T & o, const std::string& s) {
	std::istringstream iss(s);
	bio::filtering_stream<bio::input> f;
	f.push(bio::gzip_decompressor());
	f.push(iss);
	bar::binary_iarchive ia(f);
	ia >> o;
}

// save and load to file
//   Ar = boost::archive::xml_archive is more portable
//   Ar = boost::archive::binary_archive is more compact
// both benefit from compression (i.e. for string unordered_map keys)

template <class T, class Ar> 
inline void saveFile_(const T & o, const char* fn) {
	std::ofstream ofs(fn, std::ios::out|std::ios::trunc);
	assert(ofs.good());
	Ar oa(ofs);
	oa << o;
}

template <class T, class Ar> 
inline void loadFile_(T & o, const char* fn) {
	std::ifstream ifs(fn, std::ios::in);
    assert(ifs.good());
	Ar ia(ifs);
	ia >> o;
}

template <class T, class Ar> 
inline void saveGZFile_(const T & o, const char* fn) {
	//std::ofstream ofs(fn, std::ios::out|std::ios::binary|std::ios::trunc);
	bio::filtering_stream<bio::output> out;
	out.push(bio::gzip_compressor() | 
			 bio::file_sink(fn, std::ios::binary|std::ios::trunc));
	//out.push(ofs);
	Ar oa(out);
	oa << o;
}

template <class T, class Ar> 
inline void loadGZFile_(T & o, const char* fn) {
#if 1
	//std::ifstream ifs(fn, std::ios::in|std::ios::binary);
    //assert(ifs.good()); // catch if file not found
	bio::filtering_istream in;
	in.push(bio::gzip_decompressor());
	in.push(bio::file_source(fn, std::ios_base::binary));
	assert(in.is_complete());
	//in.sync();
	//bio::copy(in, std::cout);
	//in.push(ifs);
	//	try {
	//	std::cout << in << endl;
	Ar ia(in);
	ia >> o;
	//	} catch (std::exception& e) { throw e; }

#else

	//	std::ifstream ifs(fn, std::ios::in|std::ios::binary);
	//	assert(ifs.good()); // catch if file not found
	bio::filtering_istream f; 
	f.push(bio::gzip_decompressor()); 
	f.push(bio::file_source(fn, std::ios_base::binary)); 
	while (f.get() != EOF) {}
	Ar ia(f);
	ia >> o;
#endif
}

template <class T> inline void saveXML(const T& o, const char* f) { 
	saveFile_<T,bar::xml_oarchive>(o, f); 
}
template <class T> inline void saveXML(const T& o, const char* n, const char* f) { 
	saveFile_<T,bar::xml_oarchive>(make_nvp(n, o), f); 
}
template <class T> inline void saveFile(const T& o, const char* f) {
	saveFile_<T,bar::binary_oarchive>(o, f); 
}
template <class T> inline void loadXML(T& o, const char* f) {
	loadFile_<T,bar::xml_iarchive>(o, f); 
}
template <class T> inline void loadFile(T& o, const char* f) {
	loadFile_<T,bar::binary_iarchive>(o, f); 
}
template <class T> inline void saveXMLGZ(const T& o, const char* f) {
	saveGZFile_<T,bar::xml_oarchive>(o, f); 
}
template <class T> inline void saveXMLGZ(const T& o, const char* n, const char* f) {
	saveGZFile_<T,bar::xml_oarchive>(make_nvp(n, o), f); 
}

template <class T> inline void saveGZ(const T& o, const char* f) {
	saveGZFile_<T,bar::binary_oarchive>(o, f); 
}
template <class T> inline void loadXMLGZ(T& o, const char* f) {
	loadGZFile_<T,bar::xml_iarchive>(o, f); 
}
template <class T> inline void loadGZ(T& o, const char* f) {
	loadGZFile_<T,bar::binary_iarchive>(o, f); 
}

template <class T> inline void saveFile(const T & o, const std::string& f)
{ saveFile(o, f.c_str()); }
template <class T> inline void loadFile(T & o, const std::string& f)
{ loadFile(o, f.c_str()); }
template <class T> inline void saveXMLGZ(const T & o, const std::string& f)
{ saveXMLGZ(o, f.c_str()); }
template <class T> inline void loadXMLGZ(T & o, const std::string& f)
{ loadXMLGZ(o, f.c_str()); }
template <class T> inline void saveXMLGZ(const T & o, const char* n, const std::string& f)
{ saveXMLGZ(o, n, f.c_str()); }
template <class T> inline void loadXMLGZ(T & o, const char* n, const std::string& f)
{ loadXMLGZ(o, n, f.c_str()); }

#ifdef __TESTSZ__

#include "NTL/ZZ.h"

using namespace NTL;

struct test_t {
	ZZ a, b;
	template <class Ar>	
	void serialize(Ar& ar, const unsigned int ver) { 
		ar & auto_nvp(a) & auto_nvp(b); 
	}
};
std::ostream& operator<<(std::ostream& s, const test_t& o) {
	s << "[" << o.a << ", " << o.b << "]"; return s;
}

int main(int argc, const char* argv) {
	test_t o/* = {3,5}*/, oo, ooo, oooo, ooooo;
	o.a = RandomLen_ZZ(1024);
	o.b = -RandomLen_ZZ(1024);
	cout << "o: " << o << endl;
	string s = saveGZString(make_nvp("o",o));
	saveFile(make_nvp("o",o), "testsz.datt");
	saveGZ(make_nvp("o",o), "testsz.dat.gz");
	saveXML(make_nvp("o",o), "testsz.xml");
	saveXMLGZ(make_nvp("o",o), "testsz.xml.gz");
	cout << "size " << s.size() << endl;

	loadGZString(make_nvp("o",oo), s);
	cout << "oo: " << oo << endl;
	loadGZ(make_nvp("o",ooo), "testsz.dat.gz");
	cout << "ooo: " << ooo << endl;
	loadXML(make_nvp("o", oooo), "testsz.xml");
	cout << "oooo: " << oooo << endl;
	loadXMLGZ(make_nvp("o", ooooo), "testsz.xml.gz");
	cout << "ooooo: " << ooooo << endl;
}

#endif
#endif

