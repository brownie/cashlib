#include "BankParameters.h"

BankParameters::BankParameters(vector<GroupRSA> &secrets, GroupPrime &ecashGrp,
							   vector<int> &denoms)
	: coinDenominations(denoms)
{
	ecashGroup = new GroupPrime(ecashGrp);
	type = BankParameters::TYPE_SECRET;
	for (unsigned i = 0; i < secrets.size(); i++) {
		secretKeys.push_back(new GroupRSA(secrets[i]));
	}

	// set up maps that associate a given key with a denomination
	for(unsigned i = 0; i < coinDenominations.size(); i++) {
		groupToDenom[secretKeys[i]] = coinDenominations[i];
		denomToGroup[coinDenominations[i]] = secretKeys[i];
	}
}

BankParameters::BankParameters(const BankParameters &o)
	: ecashGroup(new GroupPrime(*o.ecashGroup)), type(o.type), 
	  secretKeys(o.secretKeys), groupToDenom(o.groupToDenom), 
	  denomToGroup(o.denomToGroup), coinDenominations(o.coinDenominations) 
{
}

BankParameters::~BankParameters() {
	/*delete ecashGroup;
	for (unsigned i = 0 ; i < secretKeys.size() ; i++) {
		delete secretKeys[i];
	}
	secretKeys.clear();*/
}

const GroupRSA* BankParameters::getBankKey(int denomination) const {
	map<int,GroupRSA*>::const_iterator i = denomToGroup.find(denomination);
	if (i == denomToGroup.end()) {
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"[BankParameters:getBankKey] Tried to find BankKey for "
				"number (%d) not associated with a denomination.", 
				denomination);
	}
	return i->second;
}

int BankParameters::getCoinDenomination(GroupRSA* group) const {
	map<GroupRSA*,int>::const_iterator i = groupToDenom.find(group);
	if (i == groupToDenom.end()) {
		throw CashException(CashException::CE_UNKNOWN_ERROR,
			"[BankParameters:getCoinDenomination] Tried to find denomination "
			"for group not associated with a denomination.");
	}
	return i->second;
}

void BankParameters::makePublic() {
	for(unsigned i = 0 ; i < secretKeys.size(); i++) {
		secretKeys[i]->clearSecrets();
	}
	type = BankParameters::TYPE_PUBLIC;
}

