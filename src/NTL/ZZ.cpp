#include "ZZ.h"
#include <stdlib.h>
#include <math.h>

namespace NTL {

static gmp_randclass _randstate(gmp_randinit_default);
 
void SetSeed(const ZZ& z) { _randstate.seed(z); }
void SetSeed(unsigned long int l) { _randstate.seed(l); }
ZZ RandomBnd(const ZZ& n) { return _randstate.get_z_range(n); }
ZZ RandomBits_ZZ(long l) { return _randstate.get_z_bits(l); }

#define Error(x) throw CashException(CashException::CE_NTL_ERROR, x)

#define NTL_FDOUBLE_PRECISION (((double)(1L<<30))*((double)(1L<<22)))

#define NTL_ZZ_NBITS (32)
#define NTL_NBITS_MAX (30)
#define NTL_SP_NBITS NTL_NBITS_MAX
#define NTL_SP_BOUND (1L << NTL_SP_NBITS)
#if (NTL_SP_NBITS > 30)
#define NTL_PRIME_BND ((1L << 14) - 1)
#else
#define NTL_PRIME_BND ((1L << (NTL_SP_NBITS/2-1)) - 1)
#endif

// XXX okay?
#define NTL_MALLOC(n, a, b) malloc(n*a)

#define max(a, b) ((a) > (b) ? (a) : (b))

inline long MakeOdd(ZZ& z) {
	return mpz_remove(MPZ(z), MPZ(z), MPZ(ZZ(2))); // XXX probably not very fast
}

/* from NTL's g_lip_impl.h */

/*
 * DIRT: this implementation of _ntl_gsqrts relies crucially
 * on the assumption that the number of bits per limb_t is at least
 * equal to the number of bits per long.
 */

long SqrRoot(long n)
{
   mp_limb_t ndata, rdata;

   if (n == 0) {
      return 0;
   }

   if (n < 0) Error("negative argument to _ntl_sqrts");

   ndata = n;

   mpn_sqrtrem(&rdata, 0, &ndata, 1);

   return rdata;
}

/* from NTL's ZZ.c */

// More prime generation stuff...

static
double Log2(double x)
{
   static double log2 = log(2.0);
   return log(x)/log2;
}

long MillerWitness(const ZZ& n, const ZZ& x)
{
   ZZ m, y, z;
   long j, k;

   if (x == 0) return 0;

   add(m, n, -1);
   k = MakeOdd(m);
   // n - 1 == 2^k * m, m odd

   PowerMod(z, x, m, n);
   if (z == 1) return 0;

   j = 0;
   do {
      y = z;
      SqrMod(z, y, n);
      j++;
   } while (j != k && z != 1);

   if (z != 1) return 1;
   add(y, y, 1);
   if (y != n) return 1;
   return 0;
}

// ComputePrimeBound computes a reasonable bound for trial
// division in the Miller-Rabin test.
// It is computed a bit on the "low" side, since being a bit
// low doesn't hurt much, but being too high can hurt a lot.

static
long ComputePrimeBound(long bn)
{
   long wn = (bn+NTL_ZZ_NBITS-1)/NTL_ZZ_NBITS;

   long fn;

   if (wn <= 36)
      fn = wn/4 + 1;
   else
      fn = long(1.67*sqrt(double(wn)));

   long prime_bnd;

   if (NumBits(bn) + NumBits(fn) > NTL_SP_NBITS)
      prime_bnd = NTL_SP_BOUND;
   else
      prime_bnd = bn*fn;

   return prime_bnd;
}


// Define p(k,t) to be the conditional probability that a random, odd, k-bit 
// number is composite, given that it passes t iterations of the 
// Miller-Rabin test.
// This routine returns 0 or 1, and if it returns 1 then
// p(k,t) <= 2^{-n}.
// This basically encodes the estimates of Damgard, Landrock, and Pomerance;
// it uses floating point arithmetic, but is coded in such a way
// that its results should be correct, assuming that the log function
// is computed with reasonable precision.
// 
// It is assumed that k >= 3 and t >= 1; if this does not hold,
// then 0 is returned.

static
long ErrBoundTest(long kk, long tt, long nn)

{
   const double fudge = (1.0 + 1024.0/NTL_FDOUBLE_PRECISION);
   const double log2_3 = Log2(3.0);
   const double log2_7 = Log2(7.0);
   const double log2_20 = Log2(20.0);

   double k = kk;
   double t = tt;
   double n = nn;

   if (k < 3 || t < 1) return 0;
   if (n < 1) return 1;

   // the following test is largely academic
   if (9*t > NTL_FDOUBLE_PRECISION) Error("ErrBoundTest: t too big");

   double log2_k = Log2(k);

   if ((n + log2_k)*fudge <= 2*t)
      return 1;

   if ((2*log2_k + 4.0 + n)*fudge <= 2*sqrt(k))
      return 2;

   if ((t == 2 && k >= 88) || (3 <= t && 9*t <= k && k >= 21)) {
      if ((1.5*log2_k + t + 4.0 + n)*fudge <= 0.5*Log2(t) + 2*(sqrt(t*k)))
         return 3;
   }

   if (k <= 9*t && 4*t <= k && k >= 21) {
      if ( ((log2_3 + log2_7 + log2_k + n)*fudge <= log2_20 + 5*t)  &&
           ((log2_3 + (15.0/4.0)*log2_k + n)*fudge <= log2_7 + k/2 + 2*t) &&
           ((2*log2_3 + 2 + log2_k + n)*fudge <= k/4 + 3*t) )
         return 4; 
   }

   if (4*t >= k && k >= 21) {
      if (((15.0/4.0)*log2_k + n)*fudge <= log2_7 + k/2 + 2*t)
         return 5;
   }

   return 0;
}

void GenPrime(ZZ& n, long k, long err)
{
   if (k <= 1) Error("GenPrime: bad length");

   if (k > (1L << 20)) Error("GenPrime: length too large");

   if (err < 1) err = 1;
   if (err > 512) err = 512;

   if (k == 2) {
      if (RandomBnd(2))
         n = 3;
      else
         n = 2;

      return;
   }

   long t;

   t = 1;
   while (!ErrBoundTest(k, t, err))
      t++;

   RandomPrime(n, k, t);
}

class PrimeSeq {


char *movesieve;
char *movesieve_mem;
long pindex;
long pshift;
long exhausted;

public:

PrimeSeq();
~PrimeSeq();

long next();
// returns next prime in the sequence.
// returns 0 if list of small primes is exhausted.

void reset(long b);
// resets generator so that the next prime in the sequence
// is the smallest prime >= b.

private:

PrimeSeq(const PrimeSeq&);        // disabled
void operator=(const PrimeSeq&);  // disabled

// auxilliary routines

void start();
void shift(long);

};


PrimeSeq::PrimeSeq()
{
   movesieve = 0;
   movesieve_mem = 0;
   pshift = -1;
   pindex = -1;
   exhausted = 0;
}

PrimeSeq::~PrimeSeq()
{
   if (movesieve_mem)
      free(movesieve_mem);
}

long PrimeSeq::next()
{
   if (exhausted) {
      return 0;
   }

   if (pshift < 0) {
      shift(0);
      return 2;
   }

   for (;;) {
      char *p = movesieve;
      long i = pindex;

      while ((++i) < NTL_PRIME_BND) {
         if (p[i]) {
            pindex = i;
            return pshift + 2 * i + 3;
         }
      }

      long newshift = pshift + 2*NTL_PRIME_BND;

      if (newshift > 2 * NTL_PRIME_BND * (2 * NTL_PRIME_BND + 1)) {
         /* end of the road */
         exhausted = 1;
         return 0;
      }

      shift(newshift);
   }
}

static char *lowsieve = 0;

void PrimeSeq::shift(long newshift)
{
   long i;
   long j;
   long jstep;
   long jstart;
   long ibound;
   char *p;

   if (!lowsieve)
      start();

   pindex = -1;
   exhausted = 0;

   if (newshift < 0) {
      pshift = -1;
      return;
   }

   if (newshift == pshift) return;

   pshift = newshift;

   if (pshift == 0) {
      movesieve = lowsieve;
   } 
   else {
      if (!movesieve_mem) {
         movesieve_mem = (char *) NTL_MALLOC(NTL_PRIME_BND, 1, 0);
         if (!movesieve_mem) 
            Error("out of memory in PrimeSeq");
      }

      p = movesieve = movesieve_mem;
      for (i = 0; i < NTL_PRIME_BND; i++)
         p[i] = 1;

      jstep = 3;
      ibound = pshift + 2 * NTL_PRIME_BND + 1;
      for (i = 0; jstep * jstep <= ibound; i++) {
         if (lowsieve[i]) {
            if (!((jstart = (pshift + 2) / jstep + 1) & 1))
               jstart++;
            if (jstart <= jstep)
               jstart = jstep;
            jstart = (jstart * jstep - pshift - 3) / 2;
            for (j = jstart; j < NTL_PRIME_BND; j += jstep)
               p[j] = 0;
         }
         jstep += 2;
      }
   }
}


void PrimeSeq::start()
{
   long i;
   long j;
   long jstep;
   long jstart;
   long ibnd;
   char *p;

   p = lowsieve = (char *) NTL_MALLOC(NTL_PRIME_BND, 1, 0);
   if (!p)
      Error("out of memory in PrimeSeq");

   for (i = 0; i < NTL_PRIME_BND; i++)
      p[i] = 1;
      
   jstep = 1;
   jstart = -1;
   ibnd = (SqrRoot(2 * NTL_PRIME_BND + 1) - 3) / 2;
   for (i = 0; i <= ibnd; i++) {
      jstart += 2 * ((jstep += 2) - 1);
      if (p[i])
         for (j = jstart; j < NTL_PRIME_BND; j += jstep)
            p[j] = 0;
   }
}

void PrimeSeq::reset(long b)
{
   if (b > (2*NTL_PRIME_BND+1)*(2*NTL_PRIME_BND+1)) {
      exhausted = 1;
      return;
   }

   if (b <= 2) {
      shift(-1);
      return;
   }

   if ((b & 1) == 0) b++;

   shift(((b-3) / (2*NTL_PRIME_BND))* (2*NTL_PRIME_BND));
   pindex = (b - pshift - 3)/2 - 1;
}
 
void GenGermainPrime(ZZ& n, long k, long err)
{
   if (k <= 1) Error("GenGermainPrime: bad length");

   if (k > (1L << 20)) Error("GenGermainPrime: length too large");

   if (err < 1) err = 1;
   if (err > 512) err = 512;

   if (k == 2) {
      if (RandomBnd(2))
         n = 3;
      else
         n = 2;

      return;
   }


   long prime_bnd = ComputePrimeBound(k);

   if (NumBits(prime_bnd) >= k/2)
      prime_bnd = (1L << (k/2-1));


   ZZ two;
   two = 2;

   ZZ n1;

   
   PrimeSeq s;

   ZZ iter;
   iter = 0;


   for (;;) {
      iter++;

      RandomLen(n, k);
      if (!IsOdd(n)) add(n, n, 1);

      s.reset(3);
      long p;

      long sieve_passed = 1;

      p = s.next();
      while (p && p < prime_bnd) {
         long r = rem(n, p);

         if (r == 0) {
            sieve_passed = 0;
            break;
         }

         // test if 2*r + 1 = 0 (mod p)
         if (r == p-r-1) {
            sieve_passed = 0;
            break;
         }

         p = s.next();
      }

      if (!sieve_passed) continue;


      if (MillerWitness(n, two)) continue;

      // n1 = 2*n+1
      mul(n1, n, 2);
      add(n1, n1, 1);


      if (MillerWitness(n1, two)) continue;

      // now do t M-R iterations...just to make sure
 
      // First compute the appropriate number of M-R iterations, t
      // The following computes t such that 
      //       p(k,t)*8/k <= 2^{-err}/(5*iter^{1.25})
      // which suffices to get an overall error probability of 2^{-err}.
      // Note that this method has the advantage of not requiring 
      // any assumptions on the density of Germain primes.

      long err1 = max(1, err + 7 + (5*NumBits(iter) + 3)/4 - NumBits(k));
      long t;
      t = 1;
      while (!ErrBoundTest(k, t, err1))
         t++;

      ZZ W;
      long MR_passed = 1;

      long i;
      for (i = 1; i <= t; i++) {
         do {
            RandomBnd(W, n);
         } while (W == 0);
         // W == 0 is not a useful candidate witness!

         if (MillerWitness(n, W)) {
            MR_passed = 0;
            break;
         }
      }

      if (MR_passed) break;
   }
}

}
