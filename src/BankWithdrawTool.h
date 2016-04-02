#ifndef BANKWITHDRAWTOOL_H_
#define BANKWITHDRAWTOOL_H_

#include "BankParameters.h"
#include "ZKP/Environment.h"

class BankWithdrawTool {
	public:
		BankWithdrawTool(Ptr<const BankParameters> bp, const ZZ &userPK, 
						 int stat, int lx, int wSize, int denom,
						 const hashalg_t &hashAlg);
		
		BankWithdrawTool(const BankWithdrawTool &original);
		
		// computes and stores random number from Z*_primeOrder
		ZZ getBankContribution() { return bankContribution; }

		// input: a commitment to s'
		// output: A commitment to s = s' + r'
		// r' is the bank's contribution of randomness
		void computeFullCommitment(const ZZ &partialCommitment);

		// This method will return the bank's signature if the sigma proofs
		// are verified, and will throw exceptions if they are not verified
		Ptr<ProofMessage> sign(Ptr<ProofMessage> id, Ptr<ProofMessage> cl);

		int getWalletSize() const { return walletSize; }
		int getDenom() const { return coinDenom; }

	private:
		Ptr<const BankParameters> bankParameters;
		ZZ userPublicKey;
		int stat, lx, walletSize, coinDenom;
		ZZ bankContribution;
		hashalg_t hashAlg;

		// the full commitment to s = s' + r' which is computed in
		// getFullCommitment(partialCommitment)
		ZZ fullCommitment;
};

#define NUM_WALLET_SIZES 7


#endif // BANKWITHDRAWTOOL_H_
