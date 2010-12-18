
#ifndef _BANK_H_
#define _BANK_H_

#include "GroupPrime.h"
#include "GroupRSA.h"

class Bank {

	public:
		Bank(Ptr<const GroupPrime> grpPrime, Ptr<const GroupRSA> secret)
			: groupPrime(grpPrime), secretKey(secret) {}

		Bank(const Bank &o)
			: groupPrime(o.groupPrime), secretKey(o.secretKey) {}

		/*! returns a random number from prime-order group */
		ZZ randomNumber();

		/*! on input a partial commitment A', outputs a full commitment A */
		ZZ fullCommitment(const ZZ &part, const ZZ &bankPart);

	private:
		Ptr<const GroupPrime> groupPrime;
		Ptr<const GroupRSA> secretKey;
};

#endif /*_BANK_H_*/
