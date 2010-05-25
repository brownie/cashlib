/*!
 * \brief This is a class designed to hold some common static functions that
 * can be used in multiple parts of the library.
 */

#ifndef COMMONFUNCTIONS_H_
#define COMMONFUNCTIONS_H_

#include "Group.h"

class CommonFunctions {
	
	public:
		static bool isTTP(const string &name);
	
		// XXX: there has to be a better (i.e. faster) way of doing this!
		static vector<long> binaryRep(const ZZ &n) {
			vector<long> bits;
			for (long i = 0; i < NumBits(n); i++) {
				if (bit(n, i) != 0)
					bits.push_back(i);
			}
			return bits;
		}

		static string vecToBytes(const vector<ZZ>& vec) {
			size_t sz = 0;
			for (unsigned i=0; i < vec.size(); i++)
				sz += NTL::NumBytes(vec[i]);
			unsigned char buf[sz];
			unsigned pos = 0;
			for (unsigned i=0; i < vec.size(); i++) {
				size_t n = NTL::NumBytes(vec[i]);
				NTL::BytesFromZZ((unsigned char *)buf + pos, vec[i], n);
				pos += n;
			}
			assert(pos == sz);
			return string((char *)&buf, sz);
		}

		static string vecToString(const vector<ZZ> &vec) {
			ostringstream out;
			for (unsigned i = 0; i < vec.size(); i++)  {
				out << vec[i];
			}
			return out.str();
		}

		/**
		 * Decomposing a positive integer into a sum of four squares
		 * sub functions: decompose a prime into two squares
		 * 					compute jacobi symbol
		 * 					find integer square root
		 * algorithm obtained from http://www.schorn.ch/howto.html
		 *  which looks like a somewhat reputable source.
		 * 	has an acm email address, papers published, explanantion of alg.
		 */

		 // p is prime and p = 1 mod 4
		static vector<ZZ> decomposePrime(const ZZ &p, int stat);

		// I found python decompose int code.
		// This method will attempt to use that program.
		static vector<ZZ> decompose(const ZZ &a);

		/*! returns part of vec from start to length-1 */
		static vector<ZZ> subvector(const vector<ZZ> &vec, unsigned start, 
									unsigned length);

		// absolute value function for Camenisch-Shoup verifiable encryption
		static ZZ abs(const ZZ& x, const ZZ& bigNsquared);

		template<class T>
		static vector<T> vectorize(const T& element) 
			{ vector<T> vec(1, element); return vec;}
};

#endif /*COMMONFUNCTIONS_H_*/
