#include "BankParameters.h"

BankParameters::BankParameters(const vector<Ptr<GroupRSA> > &secrets, 
                               Ptr<GroupPrime> ecashGrp,
                               const vector<int> &denoms)
	: type(BankParameters::TYPE_SECRET),
      coinDenominations(denoms)
{
    ecashGroup = new_ptr<GroupPrime>(*ecashGrp);
	for (unsigned i = 0; i < secrets.size(); i++) {
		secretKeys.push_back(new_ptr<GroupRSA>(*secrets[i]));
	}

	// set up maps that associate a given key with a denomination
	for(unsigned i = 0; i < coinDenominations.size(); i++) {
		groupToDenom[secretKeys[i]] = coinDenominations[i];
		denomToGroup[coinDenominations[i]] = secretKeys[i];
	}
}

BankParameters::BankParameters(const BankParameters &o)
       : ecashGroup(new_ptr<GroupPrime>(*o.ecashGroup)), type(o.type), 
         secretKeys(o.secretKeys), groupToDenom(o.groupToDenom), 
         denomToGroup(o.denomToGroup), coinDenominations(o.coinDenominations) 
{
}

Ptr<const GroupRSA> BankParameters::getBankKey(int denomination) const {
	map<int,Ptr<GroupRSA> >::const_iterator i = denomToGroup.find(denomination);
	if (i == denomToGroup.end()) {
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"[BankParameters:getBankKey] Tried to find BankKey for "
				"number (%d) not associated with a denomination.", 
				denomination);
	}
	return i->second;
}

int BankParameters::getCoinDenomination(Ptr<GroupRSA> group) const {
	map<Ptr<GroupRSA> ,int>::const_iterator i = groupToDenom.find(group);
	if (i == groupToDenom.end()) {
		throw CashException(CashException::CE_UNKNOWN_ERROR,
			"[BankParameters:getCoinDenomination] Tried to find denomination "
			"for group not associated with a denomination.");
	}
	return i->second;
}

void BankParameters::makePublic() {
	for(unsigned i = 0 ; i < secretKeys.size(); i++) {
        // copy key, so as not to overwrite original secret key
		secretKeys[i] = new_ptr<GroupRSA>(*secretKeys[i]);
        secretKeys[i]->clearSecrets();
	}
	type = BankParameters::TYPE_PUBLIC;
}

