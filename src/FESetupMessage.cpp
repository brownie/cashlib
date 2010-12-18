
#include "FESetupMessage.h"
#include "VEVerifier.h"

bool FESetupMessage::check(Ptr<const VEPublicKey> pk, const int stat, 
						   const ZZ& R) const {
	// check coin
	if (!coinPrime.verifyCoin() || coinPrime.getR() != R)
		throw CashException(CashException::CE_FE_ERROR,
							"[FESetupMessage::check] Malformed coin");
	
	// check verifiable escrow
	string label = signPK->publicKeyString();
	VEVerifier verifier(pk);
	if (!verifier.verify(*escrow, coinPrime.getEndorsementCom(),
		       			 coinPrime.getCashGroup(), label, pk->hashAlg, stat))
		throw CashException(CashException::CE_FE_ERROR,
							"[FESetupMessage::check] Malformed escrow");
	
	return true;
}
