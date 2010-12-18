#include "MultiExp.h"
#include "Hash.h"
#include "CommonFunctions.h"
#include <ext/hash_map>
#include <sys/time.h>
#include <boost/thread/tss.hpp>

using namespace std;

/* Determine the difference, in milliseconds, between two struct timevals. */
#define TV_DIFF_MS(a, b) \
    (((b).tv_sec - (a).tv_sec) * 1000 + ((b).tv_usec - (a).tv_usec) / 1000)

/* Determine the difference, in microseconds, between two struct timevals. */
#define TV_DIFF_US(a, b) \
    (((b).tv_sec - (a).tv_sec) * 1000000 + ((b).tv_usec - (a).tv_usec))

namespace __gnu_cxx {
    template <>
    struct hash<vector<ZZ> > {
        size_t operator() (const vector<ZZ> &x) const {
			// XXX SHA1 too slow?
			hash_t h = Hash::hash(saveString(x), Hash::SHA1, string(), 
								  Hash::TYPE_PLAIN);
			// XXX need to base64_encode if hash<char*> wants a C string?
            // return hash<const char*>()(base64_encode(h.data(), h.size()).c_str());
            return hash<const char*>()(h.c_str());
        }
    };
}

struct MultiExpCacher {

	// single cache entry: a precomputed table G_i
	typedef vector<ZZ> cache_entry_t;

	// the multi-exponentiation cache
	typedef __gnu_cxx::hash_map<hash_t, cache_entry_t> cache_t;
	cache_t cache;

	ZZ exp(const vector<ZZ>& bases, const vector<ZZ>& exponents, const ZZ& mod) {
		hash_t h = Hash::hash(saveString(bases) +
							  ZZToBytes(mod), 
							  Hash::SHA1, string(), Hash::TYPE_PLAIN);
		//cout << "[MultiExpCacher::exp] multi-exp bases length " << bases.length() 
		//	 << " hash " << base64_encode(h.c_str(), h.size()) << endl;

		vector<ZZ> G;
		cache_t::iterator ci = cache.find(h);
		int blen = bases.size();
		if (ci == cache.end()) {
#ifdef PROFILE_MODEXP
			misses++;
#endif
			// not in cache: precompute G_i for these bases

			// can only handle between 2 and 4 bases right now
			assert(blen > 1 && blen <= 4);

			vector<ZZ> Gi(1 << blen);
			Gi[0] = to_ZZ(1);
			if (blen > 1) {
				Gi[1] = bases[0];
				Gi[2] = bases[1];
				Gi[3] = MulMod(bases[0], bases[1], mod);
			}
			if (blen > 2) {
				Gi[4] = bases[2];
				Gi[5] = MulMod(bases[0], Gi[4], mod);
				Gi[6] = MulMod(bases[1], Gi[4], mod);
				Gi[7] = MulMod(bases[0], Gi[6], mod);
			}
			if (blen > 3) {
				Gi[8]  = bases[3];
				Gi[9]  = MulMod(bases[0], Gi[8], mod);
				Gi[10] = MulMod(bases[1], Gi[8], mod);
				Gi[11] = MulMod(bases[0], Gi[10], mod);
				Gi[12] = MulMod(bases[2], Gi[8], mod);
				Gi[13] = MulMod(bases[0], Gi[12], mod);
				Gi[14] = MulMod(bases[1], Gi[12], mod);
				Gi[15] = MulMod(bases[0], Gi[14], mod);
			}
			cache[h] = Gi;
			G = Gi;
		} else {
#ifdef PROFILE_MODEXP
			hits++;
#endif
			G = ci->second;
		}

		// simultaneous multiple exponentiation
		ZZ A = to_ZZ(1);
		long t = 0; // max bits considered
		for (int i=0; i < blen; i++)
			t = max(t, NumBits(exponents[i]));

		for (long i=1; i <= t; i++) {
			// A <- A * A
			SqrMod(A, A, mod);
			
			// A <- A * G_Ii
			unsigned short expcol = 0;
			for (int j=0; j < blen; j++)
				expcol |= (bit(exponents[j], t-i) << j);

			if (expcol)
				MulMod(A, A, G[expcol], mod);
		}
		return A;
	}

#ifdef PROFILE_MULTIEXP
	unsigned long total_usecs;
	unsigned long hits;
	unsigned long misses;
	MultiExpCacher() : total_usecs(0), hits(0), misses(0) {}
	~MultiExpCacher() { 
		cout << "total time multiexping: " << total_usecs << endl; 
		cout << "total multiexp cache hits: " << hits << " " << " misses " << misses << endl;
	}
#endif
};

boost::thread_specific_ptr<MultiExpCacher> multiexp_cache;

#define USE_MULTIEXP

#ifdef PROFILE_MULTIEXP
ZZ MultiExp_(const vector<ZZ> &bases, const vector<ZZ> &exponents, 
			 const ZZ &modulus, bool cacheBases, const char* fn, unsigned line)
#else
ZZ MultiExp(const vector<ZZ> &bases, const vector<ZZ> &exponents, 
			const ZZ &modulus, bool cacheBases) 
#endif
{ 
	if(bases.size() != exponents.size())
		throw CashException(CashException::CE_SIZE_ERROR,
                            "[MultiExp] The number of bases (%u) does not match "
                            "the number of exponents (%u)", bases.size(), 
							exponents.size());
	if (bases.size() == 1)
		return PowerMod(bases[0], exponents[0], modulus);

	bool multiexp_ok = false;
#ifdef USE_MULTIEXP
	// init MultiExpCacher for this thread (Boost will delete)
	if (multiexp_cache.get() == 0) 
		multiexp_cache.reset(new MultiExpCacher());

	// can we use the multi-exponentiation optimization?
	multiexp_ok = (bases.size() <= 4 && bases.size() >= 2) && cacheBases;
	if (multiexp_ok) 
		for (unsigned i=0; i < exponents.size(); i++) {
		// XXX multi-exponentiation doesn't work with negative exponents right now
		if (exponents[i] < 0) {
			multiexp_ok = false;
			break;
		}
	}
#endif

	ZZ result = to_ZZ(1);

#ifdef PROFILE_MULTIEXP
	unsigned long hit = multiexp_cache->hits, miss = multiexp_cache->misses;
	timeval start, end;
    gettimeofday(&start, NULL);
#endif

#ifdef USE_MULTIEXP
	if (multiexp_ok) {
		result = multiexp_cache->exp(bases, exponents, modulus);
#ifdef MULTIEXP_DOUBLE_CHECK
		ZZ check = to_ZZ(1);
		for(unsigned i = 0; i < bases.size(); i++)
			check = MulMod(check, PowerMod(bases[i], exponents[i], modulus), 
						   modulus);
		assert(check == result);
#endif
	} else {
#endif
		// regular, slow MulMod + PowerMod exponentiation: probably okay for 2 bases?
		for(unsigned i = 0; i < bases.size(); i++)
			result = MulMod(result, PowerMod(bases[i], exponents[i], modulus), 
							modulus);
#ifdef USE_MULTIEXP
	}
#endif

#ifdef PROFILE_MULTIEXP
    gettimeofday(&end, NULL);
	cout << "[MultiExp] " << (multiexp_ok ? "multi" : "     ") 
		 << "exp " << bases.size() << " " << TV_DIFF_US(start, end) << " us" 
		 << ( (multiexp_cache->hits-hit) ? " HIT " :
			  (multiexp_cache->misses-miss) ? " MISS " : " ") << fn << ":" << line << endl;
	multiexp_cache->total_usecs += TV_DIFF_US(start, end);
#endif

	return result;
}

