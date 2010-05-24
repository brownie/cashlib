
#ifndef _VESECRETKEY_H_
#define _VESECRETKEY_H_

#include <NTL/ZZ.h>
#include "CommonFunctions.h"

/*! \brief This class represents the secret key for a verifiable 
 * encryption scheme */

class VESecretKey {
	public:
		/*! takes in various things to be stored - note that what we are
		 * storing as y and z are the last two elements in xValues */
		VESecretKey(ZZ bigP, ZZ bigQ, vector<ZZ> xValues, ZZ p, ZZ q)
			: bigP(bigP), bigQ(bigQ), p(p), q(q) {
			// don't want the entire vector, just up to last two elements
			unsigned m = xValues.size() - 2;
			xs = CommonFunctions::subvector(xValues, 0, m);
			y = xValues[m];
			z = xValues[m+1];
		}

		/*! copy constructor */
		VESecretKey(const VESecretKey &o)
			: bigP(o.bigP), bigQ(o.bigQ), xs(o.xs), y(o.y), 
			  z(o.z), p(o.p), q(o.q) {}
		
		/*! Constructor to load from file */
		VESecretKey(const char *fname)
			{ loadFile(make_nvp("VESecretKey", *this), fname); }

		// list of getters for all values
		vector<ZZ> getXValues() const { return xs; }
		ZZ getBigP() const { return bigP; }
		ZZ getBigQ() const { return bigQ; }
		ZZ getY() const { return y; }
		ZZ getZ() const { return z; }
		ZZ getP() const { return p; }
		ZZ getQ() const { return q; }
		
	private:
		ZZ bigP, bigQ;
		vector<ZZ> xs;
		ZZ y, z;
		ZZ p, q;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(bigP) & auto_nvp(bigQ) & auto_nvp(xs)
				& auto_nvp(y) & auto_nvp(z)	
				& auto_nvp(p) & auto_nvp(q);
		}
};

#endif /*VESECRETKEY_H_*/
