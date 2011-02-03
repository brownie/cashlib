#ifndef _BANKPARAMETERS_H_
#define _BANKPARAMETERS_H_

#include "NTL/ZZ.h"
#include "GroupPrime.h"
#include "GroupRSA.h"
#include <map>

/*! \brief This class is a container for the parameters created and
 * published by the Bank */

class BankParameters {
	
	public:
		/*! this class stores the bank's public keys, the group used
		 * for e-cash, and a list of possible coin denominations */
		BankParameters(const vector<Ptr<GroupRSA> >& secretKey, 
                       Ptr<GroupPrime> ecash, 
					   const vector<int>& denoms);

		/*! constructor to load from file */
		BankParameters(const char* fname) 
			{	loadFile(make_nvp("BankParameters", *this), fname); }

        /*! copy constructor */
        BankParameters(const BankParameters &o);

		/*! determines whether RSA groups have had secrets cleared or not */
		static const int TYPE_SECRET = 1;
		static const int TYPE_PUBLIC = 0;
		int getType() const { return type; }

		/*! getters */
		Ptr<const GroupPrime> getCashGroup() const { return ecashGroup; }
		const vector<Ptr<GroupRSA> > getBankKeys() const { return secretKeys; }
		vector<int> getDenominations() const { return coinDenominations; }
		Ptr<const GroupRSA> getBankKey(int coinDenomination) const;
		int getCoinDenomination(Ptr<GroupRSA> group) const;

		/*! clear all secrets and set type to public */
		void makePublic();

	private:
		BankParameters() : type(1) {}

		Ptr<GroupPrime> ecashGroup;
		int type;
		vector<Ptr<GroupRSA> > secretKeys;
		// XXX: do we want this to be hardcoded?
		const static int stat = 80;
		map<Ptr<GroupRSA>, int> groupToDenom;
		map<int, Ptr<GroupRSA> > denomToGroup;
		vector<int> coinDenominations;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(type) // XXX save text of "public"/"secret"?
				// XXX stat?
				& auto_nvp(ecashGroup)
				& auto_nvp(secretKeys)
				& auto_nvp(groupToDenom)
				& auto_nvp(denomToGroup)
				& auto_nvp(coinDenominations);
		}
};

#endif /*_BANKPARAMETERS_H_*/
