#ifndef __GMPXX_VEC_ZZ_H__
#define __GMPXX_VEC_ZZ_H__
#include <vector>
#include <iostream>
#include <NTL/ZZ.h>

namespace NTL {
	// from NTL/tools.h
#ifndef NTL_tools__H
	struct INIT_SIZE_STRUCT { };
	const INIT_SIZE_STRUCT INIT_SIZE = INIT_SIZE_STRUCT();
	typedef const INIT_SIZE_STRUCT& INIT_SIZE_TYPE;
#endif

	// a vec_ZZ just subclasses vector<ZZ> to define length() and SetLength()
	typedef std::vector<ZZ> vec_ZZ_t;
	class vec_ZZ : public vec_ZZ_t {
	public:
		vec_ZZ() : vec_ZZ_t() {}
		vec_ZZ(INIT_SIZE_TYPE i, long n) : vec_ZZ_t(n) {}
		vec_ZZ(const vec_ZZ_t& v) : vec_ZZ_t(v) {}
		long length() const { return size(); }
		void SetLength(long n) { resize(n); }
		// convert vector<ZZ> to vec_ZZ subclass
		vec_ZZ& operator=(const vec_ZZ_t& other) {
			vec_ZZ_t::operator=(other);
			return *this;
		}
		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar & boost::serialization::base_object<vec_ZZ_t>(*this);
		}
	};

	// NTL's vec_ZZ.clear() leaves length unchanged
	inline void clear(vec_ZZ &v) { size_t l = v.size(); v.clear(); v.resize(l); }

	inline void append(vec_ZZ& v, const ZZ& z) { v.push_back(z); }
	inline void append(vec_ZZ& v, const vec_ZZ& z) { 
		std::copy(z.begin(), z.end(), std::back_inserter(v));
	}

	inline std::ostream& operator<<(std::ostream& s, const vec_ZZ &v) {
		bool once = true;
		s << "[";
		for (vec_ZZ::const_iterator i = v.begin(); i != v.end(); ++i) {
			if (once) { once = false; }
			else { s << " "; }
			s << *i << " ";
		}
		s << "]";
        return s;
    }

	// result must have length n
	inline vec_ZZ VectorCopy(const vec_ZZ& a, long n) {
		vec_ZZ ret(INIT_SIZE, n); // all set to 0
		for (unsigned i = 0; i < a.size(); i++) 
			ret[i] = a[i];
		return ret;
	}
};
#endif
