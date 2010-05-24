#ifndef MULTIEXP_H_
#define MULTIEXP_H_

#include <NTL/ZZ.h>
#include <vector>

NTL_CLIENT
/*
 * --Multi Exponentiation: Whenever there is more than one base that will be
 *   raised to more than one exponent, this must be used for efficiency.
 */

// Multi Exponentiation
// set cacheBases to false if these bases will not be used again

#ifndef PROFILE_MULTIEXP

ZZ MultiExp(const vector<ZZ> &bases, const vector<ZZ> &exponents, 
			const ZZ &modulus, bool cacheBases=true);
#define MultiExpOnce(b,e,m) MultiExp((b),(e),(m),false)
#else
// profiling: record FILE/LINE number of each caller
ZZ MultiExp_(const vector<ZZ> &bases, const vector<ZZ> &exponents, 
			 const ZZ &modulus, bool cacheBases, const char* fn, unsigned line);
#define MultiExp(b,e,m) MultiExp_((b),(e),(m),true,__FILE__,__LINE__)
#define MultiExpOnce(b,e,m) MultiExp_((b),(e),(m),false,__FILE__,__LINE__)
#endif

#endif
