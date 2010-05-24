
#ifndef _POWERCACHE_H_
#define _POWERCACHE_H_

#include <NTL/ZZ.h>
#include <vector>
#include <boost/unordered_map.hpp>

class PowerCache {
	public:
		struct Table {
			Table() {}
			int k, bits, numLookups;		
			ZZ mod;
			vector<vector<ZZ> > table;
		};
		typedef boost::unordered_map<string, Table> cache_t;

		PowerCache() {}
		PowerCache(const PowerCache &o) : cache(o.cache) {}
		~PowerCache() {}

		void store(const string &baseName, const ZZ &base, const ZZ &mod, 
				   int bits, int k) {
			Table table;
			int lookupSize = 1 << k;
			table.numLookups = bits / k + 1;
			table.k = k;
			table.bits = bits;
			table.mod = mod;
			ZZ multiplier = base;
			vector<vector<ZZ> > tab;
			for (int i = 0; i < table.numLookups; i++) {
				vector<ZZ> lookup(lookupSize);
				lookup[0] = 1;
				for (int j = 1; j < lookupSize; j++) {
					// do square-and-multiply (sort of)
					lookup[j] = MulMod(multiplier, lookup[j-1], mod);
				}
				multiplier = MulMod(multiplier, lookup[lookupSize - 1], mod);
				tab.push_back(lookup);
			}
			table.table = tab;
			cache[baseName] = table;
		}

		ZZ modPow(const Table &table, const ZZ& n) const {
			int word;                     /* the word to look up. 0<word<base */
			int row, s;                   /* row and col in base table */
			int num_lookups;
			ZZ result = 1;
			
			// early abort if raising to power 0
			if (n == 0)	return 1;

			num_lookups = mpz_sizeinbase(MPZ(n), 2) / table.k + 1;
			for (row = 0; row < num_lookups; row++) {
				word = 0;
				for (s = 0; s < table.k; s++) {
					word |= mpz_tstbit(MPZ(n), table.k * row + s) << s;
				}
				if (word > 0) {
					MulMod(result, result, table.table[row][word], table.mod);
				}
			}
			return result;
		}

		void modPow(ZZ& res, const string& baseName, const ZZ& n, 
					const ZZ& mod) const {
			// look up table
			const Table &table = cache.at(baseName);
			assert(mod == table.mod);
			res = modPow(table, n);
		}

		ZZ modPow(const string& baseName, const ZZ& n, const ZZ& mod) const {
			ZZ r = 0; modPow(r, baseName, n, mod);	return r;
		}

		bool contains(const string &baseName) const
					{return cache.count(baseName) != 0;}

		void clear() { cache.clear(); }

	private:
		cache_t cache;
};

#endif /*_POWERCACHE_H_*/
