#ifndef _FOURSQUARES_H
#define _FOURSQUARES_H

#include <climits>
#include "NTL/ZZ.h"
#include <vector>

NTL_CLIENT

class FourSquares {
	public:
    	static vector<ZZ> decompose(const ZZ& n);
    	static vector<ZZ> _decompose(const ZZ& n);
    
	private:
    	// Certainty used for finding good primes
    	static const int primeCertainty = 10;
    
    	// Helper Functions 
    	static bool isProbableSquare(const ZZ& n);
    	static long jacobi(long b, const ZZ& p);
    	static vector<ZZ> iunit(const ZZ& p);
    	static vector<ZZ> isqrtInternal(const ZZ& n, int log2n);
    	static vector<ZZ> isqrt(const ZZ& n);
    	static bool isProbablePrime(const ZZ& n, int certainty);
    	static ZZ nextProbablePrime(const ZZ& n, int certainty);
    	static vector<ZZ> decomposePrime(const ZZ& p);
    	static int getLowestSetBit(const ZZ& n);
};

#endif  /*_FOURSQUARES_H */

