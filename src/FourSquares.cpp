/*  This package defines an applet and an application to decompose a non-negative
    integer into a sum of at most four squares.

    Copyright (c) 2004 - 2005, Peter Schorn

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    PETER SCHORN BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    Change history
    October 2008 Brownie Points Project (http://cs.brown.edu/research/brownie)
    -   Converted to C++ for use in Cashlib library.

    29-Nov-2005 Peter Schorn
    -   Added isProbableSquare

    26-Nov-2005 Peter Schorn
    -   Simplified decomposePrime

    19-Nov-2005 Peter Schorn
    -   Replaced decomposePrime with version without isqrt

    12-Nov-2005 Peter Schorn
    -   Replaced isqrt by a more efficient version

    12-Dec-2004 Peter Schorn
    -   Introduced check for squares in iunit to handle (unlikely) case that a
        probable prime is not only composite but even a square resulting in a
        potentially extreme running time for iunit
    -   Fixed a bug in jacobi where instead of division by two multiplication
        by two was performed in a reduction step. No impact on final result as
        a subsequent step performed the correct division.
    -   Simplified the search loop in decompose and fixed a bug where in the case
        of an unsuccessful decomposition of a probable prime the algorithm would
        loop infinitely. Bug was unlikely to occur as isProbablePrime is very
        unlikely to identify a composite number as prime.
*/

#include "FourSquares.h"
#include "Timer.h"
#include <ext/hash_map>
#include <ext/hash_set>
#include <sstream>
#include <iostream>
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_set;

namespace __gnu_cxx {
    template <>
    struct hash<ZZ> {
        size_t operator() (const ZZ& x) const {
			std::ostringstream o; o << x;
            return hash<const char*>()(o.str().c_str());
        }
    };
}

static const ZZ ZERO       = to_ZZ(0L);
static const ZZ ONE        = to_ZZ(1L);
static const ZZ TWO        = to_ZZ(2L);
static const ZZ MAXINT     = to_ZZ(INT_MAX);
static const ZZ ITERBETTER = LeftShift(ONE, 1024);
    
#define ARRAYLEN(x) (sizeof(x)/sizeof(x[0]))

/*  Special cases for the decomposition algorithm
    0 <= i < specialCasesArray.length = specialCasesDecomposition.length: s
	pecialCasesArray[i] =
    specialCasesDecomposition[i][0]^2 + specialCasesDecomposition[i][1]^2 +
    specialCasesDecomposition[i][2]^2
*/
typedef hash_map<ZZ, const int*> specialcase_map_t;

static const ZZ specialCasesArray[] = {
    to_ZZ(9634L),   to_ZZ(2986L),   to_ZZ(1906L),
    to_ZZ(1414L),   to_ZZ(730L),    to_ZZ(706L),
    to_ZZ(526L),    to_ZZ(370L),    to_ZZ(226L),
    to_ZZ(214L),    to_ZZ(130L),    to_ZZ(85L),
    to_ZZ(58L),     to_ZZ(34L),     to_ZZ(10L),
    to_ZZ(3L),      TWO
};

static const int specialCasesDecomposition[][3] = {
    {56,    57, 57},    {21,    32, 39},    {13,    21, 36},
    { 6,    17, 33},    { 0,    1,  27},    {15,    15, 16},
    { 6,    7,  21},    { 8,    9,  15},    { 8,    9,  9},
    { 3,    6,  13},    { 0,    3,  11},    { 0,    6,  7},
    { 0,    3,  7},     { 3,    3,  4},     { 0,    1,  3},
    { 1,    1,  1},     { 0,    1,  1}
};

specialcase_map_t initSC() {
    specialcase_map_t ret;
    for (size_t i=0; i < ARRAYLEN(specialCasesArray); i++)
        ret[specialCasesArray[i]] = specialCasesDecomposition[i];
    return ret;
}
static const specialcase_map_t specialCases = initSC();

static const long magicN = 10080;
static const ZZ bigMagicN = to_ZZ(magicN);
// There are exactly 336 squares mod 10080. This is the smallest rate 
// of squares for all n < 15840.

// All primes less than 500 (499 = ip(95))
static const long primes[] = {
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59,
    61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
    191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251,
    257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317,
    331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397,
    401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
    467, 479, 487, 491, 499
	/*ADDED LATER ON*/
	, 503,   509,   521,   523,   541,
	547,    557,    563,    569,    571,   577,   587,   593,   599,   601,
	607,    613,    617,    619,    631,   641,   643,   647,   653,   659,
	661,    673,    677,    683,    691,   701,   709,   719,   727,   733,
	739,    743,    751,    757,    761,   769,   773,   787,   797,   809,
	811,    821,    823,    827,    829,   839,   853,   857,   859,   863,
	877,    881,    883,    887,    907,   911,   919,   929,   937,   941,
	947,    953,    967,    971,    977,   983,   991,   997,  1009,  1013,
	1019,   1021,   1031,   1033,   1039,  1049,  1051,  1061,  1063,  1069,
	1087,   1091,   1093,   1097,   1103,  1109,  1117,  1123,  1129,  1151,
	1153,   1163,   1171,   1181,   1187,  1193,  1201,  1213,  1217,  1223,
	1229,   1231,   1237,   1249,   1259,  1277,  1279,  1283,  1289,  1291,
	1297,   1301,   1303,   1307,   1319,  1321,  1327,  1361,  1367,  1373,
	1381,   1399,   1409,   1423,   1427,  1429,  1433,  1439,  1447,  1451,
	1453,   1459,   1471,   1481,   1483,  1487,  1489,  1493,  1499,  1511,
	1523,   1531,   1543,   1549,   1553,  1559,  1567,  1571,  1579,  1583,
	1597,   1601,   1607,   1609,   1613,  1619,  1621,  1627,  1637,  1657,
	1663,   1667,   1669,   1693,   1697,  1699,  1709,  1721,  1723,  1733,
	1741,   1747,   1753,   1759,   1777,  1783,  1787,  1789,  1801,  1811,
	1823,   1831,   1847,   1861,   1867,  1871,  1873,  1877,  1879,  1889,
	1901,   1907,   1913,   1931,   1933,  1949,  1951,  1973,  1979,  1987,
	1993,   1997,   1999,   2003,   2011,  2017,  2027,  2029,  2039,  2053,
	2063,   2069,   2081,   2083,   2087,  2089,  2099,  2111,  2113,  2129,
	2131,   2137,   2141,   2143,   2153,  2161,  2179,  2203,  2207,  2213,
	2221,   2237,   2239,   2243,   2251,  2267,  2269,  2273,  2281,  2287,
	2293,   2297,   2309,   2311,   2333,  2339,  2341,  2347,  2351,  2357,
	2371,   2377,   2381,   2383,   2389,  2393,  2399,  2411,  2417,  2423,
	2437,   2441,   2447,   2459,   2467,  2473,  2477,  2503,  2521,  2531,
	2539,   2543,   2549,   2551,   2557,  2579,  2591,  2593,  2609,  2617,
	2621,   2633,   2647,   2657,   2659,  2663,  2671,  2677,  2683,  2687,
	2689,   2693,   2699,   2707,   2711,  2713,  2719,  2729,  2731,  2741,
	2749,   2753,   2767,   2777,   2789,  2791,  2797,  2801,  2803,  2819,
	2833,   2837,   2843,   2851,   2857,  2861,  2879,  2887,  2897,  2903,
	2909,   2917,   2927,   2939,   2953,  2957,  2963,  2969,  2971,  2999,
	3001,   3011,   3019,   3023,   3037,  3041,  3049,  3061,  3067,  3079,
	3083,   3089,   3109,   3119,   3121,  3137,  3163,  3167,  3169,  3181,
	3187,   3191,   3203,   3209,   3217,  3221,  3229,  3251,  3253,  3257,
	3259,   3271,   3299,   3301,   3307,  3313,  3319,  3323,  3329,  3331,
	3343,   3347,   3359,   3361,   3371,  3373,  3389,  3391,  3407,  3413,
	3433,   3449,   3457,   3461,   3463,  3467,  3469,  3491,  3499,  3511,
	3517,   3527,   3529,   3533,   3539,  3541,  3547,  3557,  3559,  3571,
	3581,   3583,   3593,   3607,   3613,  3617,  3623,  3631,  3637,  3643,
	3659,   3671,   3673,   3677,   3691,  3697,  3701,  3709,  3719,  3727,
	3733,   3739,   3761,   3767,   3769,  3779,  3793,  3797,  3803,  3821,
	3823,   3833,   3847,   3851,   3853,  3863,  3877,  3881,  3889,  3907,
	3911,   3917,   3919,   3923,   3929,  3931,  3943,  3947,  3967,  3989,
	4001,   4003,   4007,   4013,   4019,  4021,  4027,  4049,  4051,  4057,
	4073,   4079,   4091,   4093,   4099,  4111,  4127,  4129,  4133,  4139,
	4153,   4157,   4159,   4177,   4201,  4211,  4217,  4219,  4229,  4231,
	4241,   4243,   4253,   4259,   4261,  4271,  4273,  4283,  4289,  4297,
	4327,   4337,   4339,   4349,   4357,  4363,  4373,  4391,  4397,  4409,
	4421,   4423,   4441,   4447,   4451,  4457,  4463,  4481,  4483,  4493,
	4507,   4513,   4517,   4519,   4523,  4547,  4549,  4561,  4567,  4583,
	4591,   4597,   4603,   4621,   4637,  4639,  4643,  4649,  4651,  4657,
	4663,   4673,   4679,   4691,   4703,  4721,  4723,  4729,  4733,  4751,
	4759,   4783,   4787,   4789,   4793,  4799,  4801,  4813,  4817,  4831,
	4861,   4871,   4877,   4889,   4903,  4909,  4919,  4931,  4933,  4937,
	4943,   4951,   4957,   4967,   4969,  4973,  4987,  4993,  4999,  5003,
	5009,   5011,   5021,   5023,   5039,  5051,  5059,  5077,  5081,  5087,
	5099,   5101,   5107,   5113,   5119,  5147,  5153,  5167,  5171,  5179,
	5189,   5197,   5209,   5227,   5231,  5233,  5237,  5261,  5273,  5279,
	5281,   5297,   5303,   5309,   5323,  5333,  5347,  5351,  5381,  5387,
	5393,   5399,   5407,   5413,   5417,  5419,  5431,  5437,  5441,  5443,
	5449,   5471,   5477,   5479,   5483,  5501,  5503,  5507,  5519,  5521,
	5527,   5531,   5557,   5563,   5569,  5573,  5581,  5591,  5623,  5639,
	5641,   5647,   5651,   5653,   5657,  5659,  5669,  5683,  5689,  5693,
	5701,   5711,   5717,   5737,   5741,  5743,  5749,  5779,  5783,  5791,
	5801,   5807,   5813,   5821,   5827,  5839,  5843,  5849,  5851,  5857,
	5861,   5867,   5869,   5879,   5881,  5897,  5903,  5923,  5927,  5939,
	5953,   5981,   5987,   6007,   6011,  6029,  6037,  6043,  6047,  6053,
	6067,   6073,   6079,   6089,   6091,  6101,  6113,  6121,  6131,  6133,
	6143,   6151,   6163,   6173,   6197,  6199,  6203,  6211,  6217,  6221,
	6229,   6247,   6257,   6263,   6269,  6271,  6277,  6287,  6299,  6301,
	6311,   6317,   6323,   6329,   6337,  6343,  6353,  6359,  6361,  6367,
	6373,   6379,   6389,   6397,   6421,  6427,  6449,  6451,  6469,  6473,
	6481,   6491,   6521,   6529,   6547,  6551,  6553,  6563,  6569,  6571,
	6577,   6581,   6599,   6607,   6619,  6637,  6653,  6659,  6661,  6673,
	6679,   6689,   6691,   6701,   6703,  6709,  6719,  6733,  6737,  6761,
	6763,   6779,   6781,   6791,   6793,  6803,  6823,  6827,  6829,  6833,
	6841,   6857,   6863,   6869,   6871,  6883,  6899,  6907,  6911,  6917,
	6947,   6949,   6959,   6961,   6967,  6971,  6977,  6983,  6991,  6997,
	7001,   7013,   7019,   7027,   7039,  7043,  7057,  7069,  7079,  7103,
	7109,   7121,   7127,   7129,   7151,  7159,  7177,  7187,  7193,  7207,
	7211,   7213,   7219,   7229,   7237,  7243,  7247,  7253,  7283,  7297,
	7307,   7309,   7321,   7331,   7333,  7349,  7351,  7369,  7393,  7411,
	7417,   7433,   7451,   7457,   7459,  7477,  7481,  7487,  7489,  7499,
	7507,   7517,   7523,   7529,   7537,  7541,  7547,  7549,  7559,  7561,
	7573,   7577,   7583,   7589,   7591,  7603,  7607,  7621,  7639,  7643,
	7649,   7669,   7673,   7681,   7687,  7691,  7699,  7703,  7717,  7723,
	7727,   7741,   7753,   7757,   7759,  7789,  7793,  7817,  7823,  7829,
	7841,   7853,   7867,   7873,   7877,  7879,  7883,  7901,  7907,  7919,
	7927 };

// Product of all odd primes less than or equal to 97
static const ZZ primeProduct97 = to_ZZ("1152783981972759212376551073665878035");
static const ZZ b341 = to_ZZ(341L);

hash_set<ZZ> initSMMN() {
    hash_set<ZZ> ret((magicN >> 1) + 1);
    for (long i = 0; i <= (magicN >> 1); i++)
        ret.insert(to_ZZ((i * i) % magicN));
    return ret;
}

hash_set<ZZ> squaresModMagicN = initSMMN();

vector<ZZ> vectorize(const ZZ& a, const ZZ& b) {
    vector<ZZ> v; 
	v.push_back(a); 
	v.push_back(b); 
    return v;
}
vector<ZZ> vectorize(const ZZ& a, const ZZ& b, const ZZ& c) {
    vector<ZZ> v; 
	v.push_back(a); 
	v.push_back(b); 
	v.push_back(c);
    return v;
}

vector<ZZ> vectorize(const ZZ& a, const ZZ& b, const ZZ& c, const ZZ& d) {
    vector<ZZ> v; 
	v.push_back(a); 
	v.push_back(b); 
	v.push_back(c); 
	v.push_back(d);
    return v;
}
vector<ZZ> vectorize(const ZZ& a, const ZZ& b, const ZZ& c, const ZZ& d, 
					 const ZZ& e) {
    vector<ZZ> v; 
	v.push_back(a); 
	v.push_back(b); 
	v.push_back(c); 
	v.push_back(d); 
	v.push_back(e);
    return v;
}

/*  If result is true then n may or may not be a square. If the result is 
	false then n cannot be a square. The 'false positive' rate is 
	3.3% = 336 / 10080 = 1 / 30. The smallest non-square for which 
	isProbableSquare(n) = True is 385. */
bool FourSquares::isProbableSquare(const ZZ& n) {
    return (squaresModMagicN.find(n % bigMagicN) != squaresModMagicN.end());
}

/*  Compute the jacobi symbol (b / p)
    Precondition: p = 1 mod 4
    Postcondition: Result is jacobi symbol (b / p) 
	or -1 if gcd(b, p) > 1 */
long FourSquares::jacobi(long b, const ZZ& p) {
#ifdef MININTL_GMPXX
	assert(IsOdd(b)); // mpz_jacobi is undefined if b is even
	ZZ bb = b;
	return mpz_jacobi(MPZ(bb), MPZ(p));
#else
    long s = 1;
    long a = p % b;
    while (a > 1) {
        if ((a & 3) == 0) a >>= 2;
        else if ((a & 1) == 0) {
            if (((b & 7) == 3) || ((b & 7) == 5)) 
				s = -s;
            a >>= 1;
        }
        else {
            if (((a & 2) == 2) && ((b & 3) == 3)) 
				s = -s;
            long t = b % a; 
			b = a; 
			a = t;
        }
    }
    // Return -1 also in case gcd(a, b) > 1 to ensure termination of 
	// /***/ below
    return a == 0 ? -1 : s;
#endif
}

/*  Compute an imaginary unit modulo p
    Precondition: (p is prime) and (p = 1 mod 4)
    Postcondition: Result is [x, ONE] where x^2 = -1 mod p or
    result is [x, ZERO] where x^2 = p
    Note that this algorithm might succeed even though p is not prime.
    Example (1)
        p = 3277 = 29 * 113 = 5 mod 8, 2^((p-1)/4) = 2^819 = 128 mod p
        128^2 = -1 mod p
    Example (2)
        p = 3281 = 17 * 193, jacobi(3, p) = -1, 3^((p-1)/4) = 2^820 = 81 mod p
        81^2 = -1 mod p
*/
vector<ZZ> FourSquares::iunit(const ZZ& p) {
    ZZ r = ZERO;
    if (bit(p, 0) == 1 && bit(p, 1) == 0 && bit(p, 2) == 1) 
		r = TWO; // p = 5 mod 8
    else {
		int k = 2;
		long q = 3;
        /*  Loop through the primes to find a quadratic non-residue.
            In case p is a square (could happen as p is not necessarily 
			a prime), we check for this fact after some unsuccessful 
			attempts at finding a prime q with jacobi(q, p) = -1
        */
/***/	while (jacobi(q, p) == 1) {
			if (k < (int)ARRAYLEN(primes)) {
				q = primes[k++];
				if ((q == 229) && isProbableSquare(p)) {
				// reached when decomposing d(1, 4*pp(k), 1) for k > 47
				    vector<ZZ> sr = isqrt(p);
                    if (sign(sr[1]) == 0)
                        return vectorize(sr[0], ZERO);
                }
            }
            else {
				/*  Unlikely case that we ran out of precomputed primes.
    				We can provoke this case by decomposing a prime p of the 
					form 4*k*pp(n)+1 since p !=5 mod 8 and jacobi(q, p) = 1 
					for q <= ip(n).
    				Example p = 4*11*pp(95)+1 */
                if (r == ZERO) 
					r = q;
                r = nextProbablePrime((r + TWO), 2);
                q = to_long(r);
            }
        }
        if (r == ZERO) 
			r = q;
    }
    return vectorize(PowerMod(r, RightShift(p, 2), p), ONE);
}
    
    
/*  Compute the integer square root of n or a number which is too large by one
    Precondition: n >= 0 and 2^log2n <= n < 2^(log2n + 1), i.e. 
	log2n = floor(log2(n))
    Postcondition: Result sr has the property (sr[0]^2 - 1) <= n < 
	(sr[0] + 1)^2 and (sr[0]^2 + sr[1] = n)
*/
vector<ZZ> FourSquares::isqrtInternal(const ZZ& n, int log2n) {
	//cout << n << endl;
    if (compare(n, MAXINT) < 1) {
        ZZ s = SqrRoot(n);
        return vectorize(s, n - s * s);
    }
    if (compare(n, ITERBETTER) < 1) {
        int d = 7 * (log2n / 14 - 1) , q = 7;
        ZZ s = SqrRoot(RightShift(n, d << 1));
        while (d > 0) {
            if (q > d) q = d;
            s = LeftShift(s, q);
            d -= q;
            q <<= 1;
            s = RightShift(s + (RightShift(n, d << 1) / s) , 1);
        }
        return vectorize(s, (n - (s * s)));
    }
    int log2b = log2n >> 2;
    ZZ mask = LeftShift(ONE, log2b) - ONE;
    vector<ZZ> sr = isqrtInternal(RightShift(n, log2b << 1), log2n - (log2b << 1));
    ZZ s = sr[0];
    ZZ todiv = (LeftShift(sr[1], log2b) + (RightShift(n, log2b) & mask));
    ZZ diver = LeftShift(s, 1);
    ZZ q = todiv / diver;
    return vectorize((LeftShift(s, log2b) + q), 
					 LeftShift(todiv % diver, log2b) + (n & mask) - (q * q));
}
    
/*  Compute the integer square root of n
    Precondition: n >= 0
    Postcondition: Result sr has the property sr[0]^2 <= n < (sr[0] + 1)^2 
	and (sr[0]^2 + sr[1] = n)
*/
vector<ZZ> FourSquares::isqrt(const ZZ& n) {
    if (compare(n, MAXINT) < 1) {
        ZZ s = SqrRoot(n);
        return vectorize(s, n - s * s);
    }
    vector<ZZ> sr = isqrtInternal(n, NumBits(n) - 1);
    if (sign(sr[1]) < 0) {
        return vectorize(sr[0] - ONE, sr[1] + LeftShift(sr[0], 1) - ONE);
    }
    return sr;
}

/*  Determine whether n is probably a prime with a given certainty
    Precondition: n odd
    we use the fact that for 2 < i < 341: 2^(i-1) mod i = 1 iff i is prime
    Postcondition: If result is false then n is definitely not a prime
*/
bool FourSquares::isProbablePrime(const ZZ& n, int certainty) {
	//cout << n << endl;
	return ((compare(n, b341) < 0) || GCD(primeProduct97, n) == ONE) &&
        PowerMod(TWO, (n - ONE), n) == ONE &&
        ProbPrime(n, certainty);
}
    
/*  Compute next probable prime
    Precondition: n odd
    Postcondition: Result is next probable prime p with given certainty and p >= n
*/
ZZ FourSquares::nextProbablePrime(const ZZ& n, int certainty) {
    return NextPrime(n, certainty);
}
    
/*  Decompose a prime into a sum of two squares
    Precondition: p is (probably) prime and (p = 1 mod 4)
    Postcondition: p = result[0]^2 + result[1]^2 and result[2] = 1
    or result = [0, 0, 0] if p was not a prime
*/
vector<ZZ> FourSquares::decomposePrime(const ZZ& p) {
    ZZ a = p, b, t, x0 = ZERO, x1 = ONE;
    vector<ZZ> sr = iunit(p);
    b = sr[0];
    if (ZERO == sr[1])
        return vectorize(ZERO, b, ONE);
    if (sign(((b * b) + ONE) % p) != 0)
        // Failure to compute imaginary unit, p was not a prime
        return vectorize(ZERO, ZERO, ZERO);
    while (compare(b * b, p) > 0) {
        t = a % b;
        a = b;
        b = t;
    }
    return vectorize(a % b, b, ONE);
}

// cache decompose (since s, t, sk_u often the same)
static hash_map<ZZ, vector<ZZ> > decomp_cache;
vector<ZZ> FourSquares::decompose(const ZZ& n) {
	//startTimer();
	if (!decomp_cache.count(n))
		decomp_cache[n] = _decompose(n);
	//printTimer("[FourSquares] Decomposed value to four squares");
	return decomp_cache[n];
}

/*  Decompose a positive integer into a sum of at most four squares
    Precondition: n >= 0
    Postcondition: n = result[0]^2 + result[1]^2 + result[2]^2 + result[3]^2
        result[4] = ONE if successful, result[4] = ZERO if failure (no known case)
*/
vector<ZZ> FourSquares::_decompose(const ZZ& narg) {
	ZZ n = narg; // prevent this function from modifying its input argument

    // Check for 0 and 1
    if (compare(n, ONE) < 1)
		return vectorize(ZERO, ZERO, ZERO, n, ONE);
    ZZ sq, x, p, delta, v;
    vector<ZZ> z, sqp;
    int k = getLowestSetBit(n) >> 1; // n = 4^k*m and (m != 0 mod 4)
    if (k > 0) {
        v = LeftShift(ONE, k);
        n = RightShift(n, k << 1);
    }
    else v = ONE;
    // Case 1: Check for perfect square, in this case one square is sufficient
    sqp = isqrt(n);
    sq = sqp[0];
    if (sign(sqp[1]) == 0) // n is a perfect square
        return vectorize(ZERO, ZERO, ZERO, (v * sq), ONE);
    // Case 2: Check for prime = 1 mod 4, in this case two squares are sufficient
    if (bit(n,0) == 1 && bit(n,1) == 0 && isProbablePrime(n, primeCertainty)) {
        z = decomposePrime(n);
        if (ONE == z[2])
            return vectorize(ZERO, ZERO, (v * z[0]), (v * z[1]), ONE);
        delta = ZERO;
    }
    else if (bit(n,0) == 1 && bit(n,1) == 1 && bit(n,2) == 1) {
        /*  n = 7 mod 8, need four squares
            Subtract largest square sq1^2 such that n > sq1^2 and sq1^2 != 0 mod 8
        */
        if (bit(sq,0) == 1 || bit(sq,1) == 1) {
            delta = (v * sq);
            n = sqp[1];
        }
        else {
            delta = (v * (sq - ONE));
            n = sqp[1] + LeftShift(sq, 1) - ONE;
        }
		// Recompute sq, n cannot be a perfect square (n(old) = 7 mod 8)
        sqp = isqrt(n); 
        sq = sqp[0];
    }
    else delta = ZERO;
    /*  Postcondition: (sq = isqrt(n)) && (n != 7 mod 8) && (n != 0 mod 4)
        This implies that n is a sum of three squares - now check whether n
        is one of the special cases the rest of the algorithm could not handle.
    */
    // look up in hash table
    specialcase_map_t::const_iterator scit = specialCases.find(n);
    if (scit != specialCases.end())
        return vectorize(delta,
            (v * to_ZZ(scit->second[0])),
            (v * to_ZZ(scit->second[1])),
            (v * to_ZZ(scit->second[2])), ONE);
    /*  Case n = 3 mod 4 (actually n = 3 mod 8)
        Attempt to represent n = x^2 + 2*p with p = 1 mod 4 and p is prime
        Then we can write p = y^2 + z^2 and get n = x^2 + (y+z)^2 + (y-z)^2
    */
    if (bit(n,0) == 1 && bit(n,1) == 1) {
        if  (bit(sq,0) == 1) {
            x = sq;
            p = RightShift(sqp[1], 1);
        }
        else {
            x = (sq - ONE);
            p = RightShift(sqp[1] + LeftShift(sq, 1) - ONE, 1);
        }
        while (true) {
            if (ProbPrime(p)) {
                z = decomposePrime(p);
                if (ONE == z[2]) {
                    return vectorize(delta, (v * x), (v * (z[0] + z[1])),
                        abs(v * (z[0] - z[1])), ONE);
                }
            }
            x = (x - TWO);
            // No case for the following to return is known
            if (sign(x) < 0)
				return vectorize(ZERO, ZERO, ZERO, ZERO, ZERO);
            p = (p + LeftShift((x + ONE),1)); // Proceed to next prime candidate
        }
    }
    /*  Case n = 1 mod 4 or n = 2 mod 4
        Attempt to represent n = x^2 + p with p = 1 mod 4 and p is prime
        Then we can write p = y^2 + z^2 and get n = x^2 + y^2 + z^2
    */
    if (bit((n - sq),0) == 1) {
        x = sq;
        p = sqp[1];
    }
    else {
        x = (sq - ONE);
        p = sqp[1] + LeftShift(sq, 1) - ONE;
    }
    while (true) {
        if (ProbPrime(p)) {
            z = decomposePrime(p);
            if (ONE == z[2]) {
                return vectorize(delta, (v * x), (v * z[0]), (v * z[1]), ONE);
            }
        }
        x = (x - TWO);
        // No case for the following to return is known
        if (sign(x) < 0)
			return vectorize(ZERO, ZERO, ZERO, ZERO, ZERO);
        p = (p + LeftShift(x + ONE, 2)); // Proceed to next prime candidate
    }
}
    
int FourSquares::getLowestSetBit(const ZZ& n)
{
	int i = 0;
    for(i = 0 ; i < NumBits(n) ; i++) {
        if(bit(n,i) == 1)
            return i;
    }
    return i;
}
