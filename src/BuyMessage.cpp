#include "BuyMessage.h"
#include "VEVerifier.h"
#include "Timer.h"

bool BuyMessage::check(Ptr<const VEPublicKey> pk, const int stat, 
					   const ZZ& R) const {
	// check coin
	if (!coinPrime->verifyCoin() || coinPrime->getR() != R)
		throw CashException(CashException::CE_FE_ERROR,
							"[BuyMessage::check] Malformed coin");
	
	// check verifiable escrow
	startTimer();
	VEVerifier verifier(pk);
	if (!verifier.verify(*escrow, coinPrime->getEndorsementCom(),
						 coinPrime->getCashGroup(), saveString(*contract), 
						 pk->hashAlg, stat))
		throw CashException(CashException::CE_FE_ERROR,
							"[BuyMessage::check] Malformed escrow");
	printTimer("[Seller] checked verifiable escrow");
	return true;
}
