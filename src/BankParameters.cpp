#include "BankParameters.h"

BankParameters::BankParameters(const vector<Ptr<GroupRSA> > &secrets, 
                               Ptr<GroupPrime> ecashGrp,
                               const vector<int> &denoms)
	: ecashGroup(ecashGrp), 
      type(BankParameters::TYPE_SECRET),
      coinDenominations(denoms)
{
	for (unsigned i = 0; i < secrets.size(); i++) {
		secretKeys.push_back(secrets[i]);
	}

	// set up maps that associate a given key with a denomination
	for(unsigned i = 0; i < coinDenominations.size(); i++) {
		groupToDenom[secretKeys[i]] = coinDenominations[i];
		denomToGroup[coinDenominations[i]] = secretKeys[i];
	}
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

