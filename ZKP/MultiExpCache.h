
#ifndef _MULTIEXPCACHE_H_
#define _MULTIEXPCACHE_H_

//#define MEXP_DEBUG 1

#include <NTL/ZZ.h>
#include <vector>
#include <boost/unordered_map.hpp>
#include "../MultiExp.h"
#ifdef MEXP_DEBUG
#include <boost/algorithm/string/join.hpp>
#endif

typedef vector<vector<ZZ> > table_t;

struct multiexp_table {
	multiexp_table() : blen(0), width(0), mod(0) {}
	unsigned blen;  // number of bases
	unsigned width;     // each iteration scan this many exponent bits
	ZZ mod; // for safety let's keep this around

	// table is a 2-D array of rows, tables
	// 1st dimension (rows):   size maxexplen/width+1
	//   1 row for each W-bit-wide window of exponent bits (0..3, 4..7, etc)
	// 2nd dimension (tables): size 2^(bases*width)
	//   each table entry stores f^x * g^y * h^z for x,y,z from scanned bits
	table_t table;

	void precomp(const vector<ZZ>& bases, const ZZ& mod, 
				 unsigned width, unsigned maxlen) {
		this->mod = mod;
		this->width = width;
		this->blen = bases.size();
		// precompute G_i for scanning N cols at a time
		// (e.g., 3 bases, width=2: tabsz is 64; width=4, tabsz is 4096)
		unsigned tabsz = 1 << (width * blen);
		unsigned rows = (maxlen+width-1)/width; // = ceil(maxlen/width)
#ifdef MEXP_DEBUG
		cout << "Precomputing: " << blen << "-base table scans " << width 
			 << "-bit-wide exp columns, for table size " << tabsz 
			 << " * " << rows << " rows" 
			 << " * #bits(mod) " << NumBits(mod) << " = " 
			 << tabsz*rows*((NumBits(mod)+7)/8) << " bytes" << endl;
#endif
		table = table_t(rows);

		// build bitmask (i.e. 000011 for width=2)
		unsigned bitmask = 0;
		assert(width * blen <= sizeof(bitmask)*8);
		for (unsigned i=0; i < width; i++)
			bitmask |= 1 << i;

		// build first table
		table[0] = vector<ZZ>(tabsz);
		// compute f^x g^y h^z for each xyz where |x|=|y|=|z|=width
		for (unsigned i=0; i < tabsz; i++) {
			table[0][i] = 1;
			for (unsigned j=0; j < blen; j++) {
				// bpow is the current considered W-bit digit for this base
				unsigned bpow = (i & (bitmask << (width*j))) >> (width*j);
				MulMod(table[0][i], table[0][i], 
					   PowerMod(bases[j], bpow, mod), mod);
			}
		}

		// build remaining tables for higher bits
		// (same as first table, each cell raised to 2^width)
		unsigned sqrstep = 1 << width;
		for (unsigned row = 1; row < rows; row++) {
			table[row] = vector<ZZ>(tabsz);
#ifdef MEXP_DEBUG
			cout << "." << flush;
#endif
			for (unsigned i=0; i < tabsz; i++)
				PowerMod(table[row][i], table[row-1][i], sqrstep, mod);
		}
#ifdef MEXP_DEBUG
		cout << endl;
#endif
	}

	void multiexp_2(ZZ &A, const ZZ& e0, const ZZ& e1, const ZZ& m) const {
		assert(blen == 2);
		assert(m == mod); // XXX don't need mod
		unsigned t = max(NumBits(e0), NumBits(e1));
		assert( ((t+width-1)/width) <= table.size()); // enough rows
		
		A = 1;
		for (unsigned i=0, row=0; i < t; i+=width, row++) {
			unsigned short expcol = 0;
			// e.g. for width=2, OR bits (f[0]->0, g[0]->2) 
			//                           (f[1]->1, g[1]->3)
			for (unsigned j=0; j < width; j++)
				expcol |= 
					(bit(e1, i+j) << (width+j)) | 
					(bit(e0, i+j) << (j));
			
			if (expcol)	// A <- A * G_Ii
				MulMod(A, A, table[row][expcol], m);
		}
	}

	void multiexp_3(ZZ &A, const ZZ& e0, const ZZ& e1, const ZZ& e2, 
					const ZZ& m) const {
		assert(blen == 3);
		assert(m == mod); // XXX don't need mod
		unsigned t = max( max(NumBits(e0), NumBits(e1)), NumBits(e2));
		assert( ((t+width-1)/width) <= table.size()); // enough rows

		A = 1;
		for (unsigned i=0, row=0; i < t; i+=width, row++) {
			unsigned short expcol = 0;
			// e.g. for width=2, OR bits (f[0]->0, g[0]->2, h[0]->4)
			//                           (f[1]->1, g[1]->3, h[1]->5)
			for (unsigned j=0; j < width; j++)
				expcol |= 
					(bit(e2, i+j) << (width*2+j)) | 
					(bit(e1, i+j) << (width*1+j)) | 
					(bit(e0, i+j) << (width*0+j));
	  
			if (expcol) // A <- A * G_Ii
				MulMod(A, A, table[row][expcol], m);
		}
	}

	void multiexp_4(ZZ &A, const ZZ& e0, const ZZ& e1, const ZZ& e2, const ZZ& e3, 
					const ZZ& m) const {
		assert(blen == 4);
		assert(m == mod); // XXX don't need mod
		unsigned t = max( max(NumBits(e0), NumBits(e1)), 
						  max(NumBits(e2), NumBits(e3)) );
		assert( ((t+width-1)/width) <= table.size()); // enough rows

		A = 1;
		for (unsigned i=0, row=0; i < t; i+=width, row++) {
			unsigned short expcol = 0;
			for (unsigned j=0; j < width; j++)
				expcol |= 
					(bit(e3, i+j) << (width*3+j)) | 
					(bit(e2, i+j) << (width*2+j)) | 
					(bit(e1, i+j) << (width*1+j)) | 
					(bit(e0, i+j) << (width*0+j));
	  
			if (expcol) // A <- A * G_Ii
				MulMod(A, A, table[row][expcol], m);
		}
	}
};

class MultiExpCache {
	public:
		typedef boost::unordered_map<vector<string>, multiexp_table> cache_t;

		MultiExpCache() {}
		MultiExpCache(const MultiExpCache &o) : cache(o.cache) {}
		~MultiExpCache() {}

		static const unsigned WIDTH=4;
		// XXX should infer from Group ordlen?
		static const unsigned MAX_EXPLEN=1200; 

		void store(const vector<string>& baseNames, const vector<ZZ>& bases, 
				   const ZZ& mod, unsigned width=WIDTH, 
				   unsigned max_explen=MAX_EXPLEN) {
			if (cache.count(baseNames))
				return; // don't bother if already precomputed
			multiexp_table& t = cache[baseNames]; // avoids a copy
#ifdef MEXP_DEBUG
			cout << "Precomputing MultiExpCache for bases.size()=" << bases.size()
				 << " " << boost::algorithm::join(baseNames, std::string(",")) 
				 << endl;
#endif
			t.precomp(bases, mod, width, max_explen);
		}

		bool contains(const vector<string> &bNames) const
						{return cache.count(bNames) != 0;}

		ZZ modPow(const vector<string>& baseNames, const vector<ZZ> &bases, 
				  const vector<ZZ>& exps, const ZZ &mod) const {
			assert (baseNames.size() == exps.size());
			// XXX: right now only do 2-, 3-, 4- base exponentiation
			ZZ r;
			const multiexp_table& tab = cache.at(baseNames);
#ifdef MEXP_DEBUG
			cout << "Called MultiExpCache::modPow on exps.size()=" << exps.size()
				 << " " << boost::algorithm::join(baseNames, std::string(",")) 
				 << endl;
#endif
			if (baseNames.size() == 2) {
				tab.multiexp_2(r, exps[0], exps[1], mod);
				return r;
			} else if (baseNames.size() == 3) {
				tab.multiexp_3(r, exps[0], exps[1], exps[2], mod);
				return r;
			} else if (baseNames.size() == 4) {
				tab.multiexp_4(r, exps[0], exps[1], exps[2], exps[3], mod);
				return r;
			} else {
				return MultiExp(bases, exps, mod);
			}
		}

		void clear() { cache.clear(); }

	private:
		cache_t cache;
};

#endif /*_MULTIEXPCACHE_H_*/
