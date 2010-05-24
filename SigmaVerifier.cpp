#include "SigmaVerifier.h"

ZZ SigmaVerifier::getNewChallenge() {

	// don't perform this check for now
	/*if(!canGenerateNewChallenge())
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
