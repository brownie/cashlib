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
		BankParameters(vector<GroupRSA> &secretKey, GroupPrime &ecash, 
					   vector<int> &denoms);

		/*! constructor to load from file */
		BankParameters(const char* fname) 
			{	loadFile(make_nvp("BankParameters", *this), fname); }

		/*! copy constructor */
		BankParameters(const BankParameters &original);

		/*! destructor */
		~BankParameters();

		/*! determines whether RSA groups have had secrets cleared or not */
		static const int TYPE_SECRET = 1;
		static const int TYPE_PUBLIC = 0;
		int getType() const { return type; }

		/*! getters */
		const GroupPrime* getCashGroup() const { return ecashGroup; }
		const vector<GroupRSA*> getBankKeys() const { return secretKeys; }
		vector<int> getDenominations() const { return coinDenominations; }
		const GroupRSA* getBankKey(int coinDenomination) const;
		int getCoinDenomination(GroupRSA* group) const;

		/*! clear all secrets and set type to public */
		void makePublic();

	private:
		BankParameters() : ecashGroup(0), type(1) {}

		GroupPrime* ecashGroup;
		int type;
		vector<GroupRSA*> secretKeys;
		// XXX: do we want this to be hardcoded?
		const static int stat = 80;
		map<GroupRSA*,int> groupToDenom;
		map<int,GroupRSA*> denomToGroup;
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
