
#ifndef _VEPUBLICKEY_H_
#define _VEPUBLICKEY_H_

#include "GroupRSA.h"
#include "Hash.h"

/*! \brief This class represents the public key for a verifiable 
 * encryption scheme */

class VEPublicKey {
	public:
		VEPublicKey() {}
		
		/*! takes and stores various elements of the public key */
		VEPublicKey(const ZZ &bigN, const vector<ZZ> &aValues, const ZZ &b,
					const ZZ &d, const ZZ &e, const ZZ& f,
					const GroupRSA& group2, const hashalg_t& hashAlg,
					const string& hashKey)
			: secondGroup(group2), bigN(bigN), 
			  aValues(aValues), b(b), d(d), e(e), f(f),
			  hashAlg(hashAlg), hashKey(hashKey) { secondGroup.clearSecrets(); }

		/*! copy constructor */
		VEPublicKey(const VEPublicKey &o)
			: secondGroup(o.secondGroup), bigN(o.bigN), 
			  aValues(o.aValues), b(o.b), d(o.d), e(o.e), f(o.f), 
			  hashAlg(o.hashAlg), hashKey(o.hashKey) 
			{ secondGroup.clearSecrets(); }
		
		/*! constructor to load from file */
		VEPublicKey(const char *fname)
			{ loadFile(make_nvp("VEPublicKey", *this), fname); }
		
		// getters for all the values in the public key
		ZZ getN() const { return bigN; }
		vector<ZZ> getAValues() const { return aValues; }
		GroupRSA getSecondGroup() const { return secondGroup; }
		string getHashKey() const { return hashKey; }
		hashalg_t getHashAlg() const { return hashAlg; }
		ZZ getB() const { return b; }
		ZZ getD() const { return d; }
		ZZ getE() const { return e; }
		ZZ getF() const { return f; }
		
		void setHashKey(const string& newKey){ hashKey = newKey;}		
		void setHashAlg(const hashalg_t& newAlg){ hashAlg = newAlg;}		
		
		GroupRSA secondGroup;
		ZZ bigN;
		vector<ZZ> aValues;
		ZZ b, d, e, f;
		hashalg_t hashAlg;
		string hashKey;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar & auto_nvp(secondGroup);
			ar & auto_nvp(bigN);
			ar & auto_nvp(aValues);
			ar & auto_nvp(b);
			ar & auto_nvp(d);
			ar & auto_nvp(e);
			ar & auto_nvp(f);
			ar & auto_nvp(hashAlg);
			ar & auto_nvp(hashKey);
		}
};

#endif /*VEPUBLICKEY_H_*/
