#ifndef USERWITHDRAWTOOL_H_
#define USERWITHDRAWTOOL_H_

#include "Wallet.h"
#include "SigmaProof.h"
#include "CLBlindRecipient.h"
#include "BankParameters.h"

/*! \brief This class is used for carrying out the user's side of the 
 * withdraw protocol */

class UserWithdrawTool {

	public:
		UserWithdrawTool(int stat, int lx, const BankParameters *bp, 
                         const ZZ &userPK, const ZZ &userSK, 
						 const hashalg_t &hashAlg, int walletSize, int denom);
		
		UserWithdrawTool(const UserWithdrawTool &original);
		
		~UserWithdrawTool();

		/*! stores the vector 
		 * (r_sku, A_sku, s', r_s', A_s', t, r_t, A_t) where : 
		 * r_sku = randomness in opening of commitment to user secret key
		 * A_sku = commitment to user secret key
		 * s'    = first secret picked from Z*_prime order
		 * r_s'  = randomness in opening of commitment to s'
		 * A_s'  = commitment to s'
		 * t     = second secret picked from Z*_prime order
		 * r_t   = randomness in opening of commitment to t
		 * A_t   = commitment to t
		 *
		 * returns A_s'
		 */
		ZZ createPartialCommitment();

		/*! returns a SigmaProof that the first secret in the commitment 
		 * C (from the CLBlindRecipient) is the user's secret Key */
		ProofMessage* initiateSignature(const ZZ &bankContribution);

		/*! return individual commitments to private messages */
		vector<ZZ> getIndividualCommitments();

		/*! return randomized proof from signature recipient */
		ProofMessage* getCLProof() const;

		/*! verifies bank's PoKoDLR of 1/e and returns signature if 
		 * it is valid */
		vector<ZZ> verify(const ProofMessage &proofEInverse);
		
		/*! returns a wallet with signature from bank
		 * will throw exception if bank has not proved knowledge 
		 * of 1/e */
		Wallet getWallet(const vector<ZZ> &signature) const;

		ZZ getUserPublicKey() const { return userPublicKey; }
		int getDenomination() const { return coinDenom; }

	private:
		/*! creates and stores CLBlindRecipient that interacts with
		 * the bank to get the signature on the user secret key, s, t, and the
		 * wallet size */
		void createSignatureRecipient();

		/*! returns a SigmaProof that the A_sku commitment in the partial 
		 * commitment is the user's secret Key */
		ProofMessage* preSignatureProof();

		ZZ s; // stored so we can use it to get wallet
		int stat, lx;
		const BankParameters *bankParameters;
		const GroupPrime *userKeyGroup;

		ZZ userSecretKey;
		ZZ userPublicKey;

		hashalg_t hashAlg;
		int walletSize;
		int coinDenom;
	
		vector<ZZ> partialCommitment;
		ZZ bankContribution;

		CLBlindRecipient *signatureRecipient;
				
		// store whether or not bank's PoK of 1/e is valid
		bool verified;
};

#endif /*_USERWITHDRAWTOOL_H_*/
