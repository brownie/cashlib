
#ifndef _BANK_H_
#define _BANK_H_

#include "GroupPrime.h"
#include "GroupRSA.h"

// XXX: why does this class exist? seems like we are using BankTool and
// BankWithdrawTool but not this class

class Bank {

	public:
		Bank(const GroupPrime* grpPrime, const GroupRSA* secret)
			: groupPrime(grpPrime), secretKey(secret) {}

		Bank(const Bank &o)
			: groupPrime(o.groupPrime), secretKey(o.secretKey) {}

		~Bank() { delete groupPrime; delete secretKey; }

		/*! returns a random number from prime-order group */
		ZZ randomNumber();

		/*! on input a partial commitment A', outputs a full commitment A */
		ZZ fullCommitment(const ZZ &part, const ZZ &bankPart);

	private:
		const GroupPrime* groupPrime;
		const GroupRSA* secretKey;
};

#endif /*_BANK_H_*/
