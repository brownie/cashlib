#include "SigmaVerifier.h"

ZZ SigmaVerifier::getNewChallenge() {
	/* XXX comment this out until we decide- do we want the user to control
	 * whether the check is performed or not? I am guessing we do want them
	 * to be able to do that, because this check could be slow.
	 * -Thea 2/16/09
	 *
	if(!canGenerateNewChallenge())
	{
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"[SigmaVerifier::getNewChallenge] Not allowed to generate a "
				"challenge at this point in the algorithm");
	}
	*/
	return generateChallenge();
}

ZZ SigmaVerifier::generateChallenge() {
	ZZ chal = RandomLen_ZZ(2 * stat);
	setChallenge(chal);
	return chal;
}
