#ifndef __GMPXX_ZZ_H__
#define __GMPXX_ZZ_H__
#include <gmpxx.h>
#include <assert.h>
#include "CashException.h"
#include "Serialize.h"

/* 
 * miniNTL: somewhat NTL-compatible ZZ and vec_ZZ classes,
 * backed by mpz_class from GNU GMP C++ API (gmpxx.h).
 *
 * based on 
 *   NTL       http://www.shoup.net/ntl/ 
 *   ZZ        http://www.shoup.net/ntl/doc/ZZ.txt
 *   vec_ZZ    http://www.shoup.net/ntl/doc/vec_ZZ.txt
 */

#define NTL_CLIENT using std::vector; using std::string; using namespace NTL;

namespace NTL {

// MININTL_GMPXX indicates ZZ's are actually mpz_class from GMP++
#define MININTL_GMPXX 1
typedef mpz_class ZZ;
#define MPZ(x) (x).get_mpz_t() // helpful for using mpz_ functions
#define to_ZZ(x) ZZ(x) // mpz_class constructor handles NTL::to_ZZ cases

void SetSeed(const ZZ& z);
void SetSeed(unsigned long int l);

inline void PowerMod(ZZ& x, const ZZ& a, const ZZ& e, const ZZ& n) { 
	mpz_powm(MPZ(x), MPZ(a), MPZ(e), MPZ(n)); 
}
inline ZZ PowerMod(const ZZ& a, const ZZ& e, const ZZ& n) { 
	ZZ r; PowerMod(r, a, e, n); return r; 
}

inline ZZ power(const ZZ& a, long e) { // x = a^e (e >= 0)
	if (e < 0) 
		throw CashException(CashException::CE_NTL_ERROR, "NTL::power: e < 0");
	ZZ r; mpz_pow_ui(MPZ(r), MPZ(a), e); return r;
}

inline void AddMod(ZZ& r, const ZZ& a, const ZZ& b, const ZZ& n) {
	mpz_add(MPZ(r), MPZ(a), MPZ(b));
	mpz_mod(MPZ(r), MPZ(r), MPZ(n));
}
inline ZZ AddMod(const ZZ& a, const ZZ& b, const ZZ& n) { 
	ZZ r; AddMod(r, a, b, n); return r; 
}

inline void SubMod(ZZ& r, const ZZ& a, const ZZ& b, const ZZ& n) {
	mpz_sub(MPZ(r), MPZ(a), MPZ(b));
	mpz_mod(MPZ(r), MPZ(r), MPZ(n));
}
inline ZZ SubMod(const ZZ& a, const ZZ& b, const ZZ& n) { 
	ZZ r; SubMod(r, a, b, n); return r; 
}

// x = (a*b)%n
inline void MulMod(ZZ& r, const ZZ& a, const ZZ& b, const ZZ& n) {
	mpz_mul(MPZ(r), MPZ(a), MPZ(b));
	mpz_mod(MPZ(r), MPZ(r), MPZ(n));
}

inline ZZ MulMod(const ZZ& a, const ZZ& b, const ZZ& n) { 
	ZZ r; MulMod(r, a, b, n); return r; 
}

inline void InvMod(ZZ& r, const ZZ& a, const ZZ& n) {
	mpz_invert(MPZ(r), MPZ(a), MPZ(n));
}
inline ZZ InvMod(const ZZ& a, const ZZ& n) { 
	ZZ r; InvMod(r, a, n); return r; 
}

inline void SqrMod(ZZ& x, const ZZ& a, const ZZ& n) { // x = a^2 % n
	mpz_powm_ui(MPZ(x), MPZ(a), 2, MPZ(n));
}

inline void add(ZZ& r, const ZZ& a, const ZZ& b) { 
	mpz_add(MPZ(r), MPZ(a), MPZ(b)); 
}

inline void mul(ZZ& r, const ZZ& a, const ZZ& b) { 
	mpz_mul(MPZ(r), MPZ(a), MPZ(b)); 
}

inline long rem(const ZZ& a, long b) { 
	return mpz_fdiv_ui(MPZ(a), b); 
}

inline ZZ SqrRoot(const ZZ& a) { ZZ r; mpz_sqrt(MPZ(r), MPZ(a)); return r; }

inline ZZ GCD(const ZZ& a, const ZZ& b) { 
	ZZ r; mpz_gcd(MPZ(r), MPZ(a), MPZ(b)); return r; 
}		

inline long bit(const ZZ& a, long k) { return mpz_tstbit(MPZ(a), k); }

// returns original value of p-th bit of |a|, and replaces p-th bit of
// a by 1 if it was zero; low order bit is bit 0; error if p < 0;
// the sign of x is maintained
inline long SetBit(ZZ& x, long p) {
	long old = bit(x, p);
	mpz_setbit(MPZ(x), p); // XXX maintains sign of x?
	return old;
}

inline long IsOdd(const ZZ& a) { return mpz_odd_p(MPZ(a)); }

inline long to_int(const ZZ& z) { 
	if (!z.fits_slong_p()) 
		throw CashException(CashException::CE_NTL_ERROR, 
							"NTL::to_int: ZZ doesn't fit in int!");
	else return z.get_si();
}
inline long to_long(const ZZ& z) { return to_int(z); }

// x = pseudo-random number in the range 0..n-1, or 0 if n <= 0
ZZ RandomBnd(const ZZ& n);

inline void RandomBnd(ZZ& x, const ZZ& n) { x = RandomBnd(n); }

inline long RandomBnd(long nl) { return to_int(RandomBnd(ZZ(nl))); }

// x = pseudo-random number in the range 0..2^l-1.
ZZ RandomBits_ZZ(long l);

inline void RandomBits(ZZ& r, long l) { r = RandomBits_ZZ(l); }

// x = psuedo-random number with precisely l bits,
// or 0 of l <= 0.
inline void RandomLen(ZZ& r, long l) { // this is what ZZ.c:RandomLen() does:
	if (l <= 0) { r = 0; return; }
	if (l == 1) { r = 1; return; }
	RandomBits(r, l-1); 
	SetBit(r, l-1);
} 
inline ZZ RandomLen_ZZ(long l) { ZZ r; RandomLen(r, l); return r; }

inline ZZ NextPrime(const ZZ& m, long NumTrials=10) { // XXX ignores NumTrials
	ZZ r; mpz_nextprime(MPZ(r), MPZ(m)); return r;
}
inline long ProbPrime(const ZZ& n, long NumTrials=10) {
	return mpz_probab_prime_p(MPZ(n), NumTrials);
}
// n = random l-bit prime.  Uses ProbPrime with NumTrials.
inline void RandomPrime(ZZ& r, long l, long NumTrials=10) {
	if (l <= 1) throw CashException(CashException::CE_NTL_ERROR, 
									"NTL::RandomPrime: l (%ld) out of range", l);
	if (l == 2) {
		if (RandomBnd(2)) r = 3;
		else r = 2;
		return;
	}
	do {
		RandomLen(r, l);
		if (!IsOdd(r)) r += 1;
	} while (!ProbPrime(r, NumTrials));
}
inline ZZ RandomPrime_ZZ(long l, long NumTrials=10) { 
	ZZ r; RandomPrime(r, l, NumTrials); return r; 
}

inline long sign(const ZZ& a) { return mpz_sgn(MPZ(a)); } // returns sign of a (-1, 0, +1)

inline ZZ LeftShift(const ZZ& a, long n) { ZZ r; r = a << n; return r; }
inline ZZ RightShift(const ZZ& a, long n) { ZZ r; r = a >> n; return r; }

inline long NumBits(const ZZ& a) {	// NumBits(0) = 0
	if (a == 0) return 0;
	else return mpz_sizeinbase(MPZ(a), 2);
}

inline long compare(const ZZ& a, const ZZ& b) { // returns sign of a-b (-1, 0, or 1).
	return mpz_cmp(MPZ(a), MPZ(b));
}

inline void swap(ZZ& x, ZZ& y) { mpz_swap(MPZ(x), MPZ(y)); }

/* copy-pasted from NTL ZZ.c */
void GenPrime(ZZ& n, long l, long err);
inline ZZ GenPrime_ZZ(long l, long err = 80) { ZZ r; GenPrime(r, l, err); return r; }
void GenGermainPrime(ZZ& n, long l, long err = 80);
inline ZZ GenGermainPrime_ZZ(long l, long err = 80) { ZZ r; GenGermainPrime(r, l, err); return r; }

// x = sum(p[i]*256^i, i=0..n-1). 
inline void ZZFromBytes(ZZ& x, const unsigned char *p, long n) { 
	// NTL uses native byte order? (that means all our data files are little-endian?)
	mpz_import(MPZ(x), n, -1, 1, -1, 0, p);
}
inline ZZ ZZFromBytes(const unsigned char *p, long n) { ZZ r; ZZFromBytes(r, p, n); return r; }

inline long NumBytes(const ZZ& a) { 
	return (NumBits(a) + 7) / 8;
}

// Computes p[0..n-1] such that abs(a) == sum(p[i]*256^i, i=0..n-1) mod 256^n.
inline void BytesFromZZ(unsigned char *p, const ZZ& a, long n) {
	assert(p); // GMP can allocate this for us, though
	size_t cnt;
	mpz_export(p, &cnt, -1, 1, -1, 0, MPZ(a));	// like NTL, GMP does not export the sign of ZZs
}

// Convert binary string to NTL integer
inline ZZ ZZFromBytes(const string &str) {
	return ZZFromBytes((const unsigned char *)str.data(), str.size());
}
 
inline string ZZToBytes(const ZZ& n) {
	unsigned char buf[NumBytes(n)];
	BytesFromZZ((unsigned char *)&buf, n, sizeof(buf));
	return string((char *)&buf, sizeof(buf));
}

} // namespace NTL

BOOST_SERIALIZATION_SPLIT_FREE(NTL::ZZ)

#define STRING_ZZ 1

namespace boost { namespace serialization {
	template<class Archive> inline
		void save(Archive & ar, const NTL::ZZ& t, unsigned int version)
		{
			// like NTL, GMP does not export the sign of ZZs
			bool neg = (NTL::sign(t) == -1);
			ar & auto_nvp(neg); // save sign (true if negative)
			size_t len = (mpz_sizeinbase(MPZ(t), 2) + 7) / 8;
			assert(len >= 0);
			ar & auto_nvp(len);
			if (len) {
				unsigned char buf[len];
				size_t l;
				mpz_export(buf, &l, -1, 1, -1, 0, MPZ(t));
				assert(l == len);
				ar & make_nvp("mpz", make_binary_object(buf, l));
			}
		}

	template<> inline
		void save<bar::xml_oarchive>(bar::xml_oarchive & ar, 
									 const NTL::ZZ& t, unsigned int version)
		{
			string s = t.get_str();
			ar & make_nvp("mpz", s);
		}

	template<class Archive> inline
		void load(Archive & ar, NTL::ZZ& t, unsigned int version)
		{
			bool neg; ar & auto_nvp(neg);
			size_t len; ar & auto_nvp(len);
			if (len) {
				unsigned char buf[len]; // XXX stack
				ar & make_nvp("mpz", make_binary_object(buf, len));
				mpz_import(MPZ(t), len, -1, 1, -1, 0, buf);
				if (neg) mpz_neg(MPZ(t), MPZ(t));
			}
		}

	template<> inline
		void load<bar::xml_iarchive>(bar::xml_iarchive & ar, 
									 NTL::ZZ& t, unsigned int version)
		{
			string s; ar & make_nvp("mpz", s); NTL::ZZ z(s); t = z;
		}

} } // namespace boost::serialization


#endif

