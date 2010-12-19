#ifndef BANKTOOL_H_
#define BANKTOOL_H_

#include "SigmaProof.h"
#include "BankWithdrawTool.h"
#include "Wallet.h"
#include "BankParameters.h"

class BankTool {
	public:
		BankTool(int st, int l, int modLen, const hashalg_t &ha,
				 vector<int> &coinDenoms);

		BankTool(int st, int l, const hashalg_t &ha,
				 BankParameters bp);

		BankTool(const char *fname) 
			: bankParameters(), publicBankParameters() 
			{ loadFile(make_nvp("BankTool", *this), fname); }

		// getters
		Ptr<const BankParameters> getBankParameters() const
						{ return publicBankParameters; }
		Ptr<const GroupPrime> getCashGroup() const 
						{ return publicBankParameters->getCashGroup(); }
		const vector<Ptr<GroupRSA> > getBankPublicKeys() const 
						{ return publicBankParameters->getBankKeys(); }
		Ptr<const GroupRSA> getBankPublicKey(int denom) const 
						{ return publicBankParameters->getBankKey(denom); }

		/*! Inputs: user public key, group corresponding to public key, and
		 * wallet size.
		 * Output: a tool for interacting with a user who wants to withdraw a
		 * wallet.
		 * The BWTool returned must be freed by the caller. */
		Ptr<BankWithdrawTool> getWithdrawTool(const ZZ &userPK, int wSize, 
										  int coinDenom) const;

		/*! Input: a non interactive sigma proof of knowledge of a user's secret
		 *  key, using the public key as a commitment to the secret key.
		 * Ouputs: true if proof accepted, false if proof not accepted. */
		bool verifyIdentity(Ptr<ProofMessage> idProof, const ZZ &userPK) const;

		// functions for depositing at the bank
		/*! Returns true if a coin is formed correctly */
		bool verifyCoin(const Coin &coin) const;

		/*! Returns true if coin is double spent, false if merchant is just
		 * trying to deposit twice. */
		bool isCoinDoubleSpent(const Coin &coin, const Coin &coin2) const;

		/*! given coins that have been double-spent, identify the 
		 * public key of the user who withdrew the coins */
		ZZ identifyDoubleSpender(const Coin& coin1, const Coin& coin2) const;
		ZZ identifyDoubleSpender(const Coin& coin1, const ZZ& t2,
								 const ZZ& r2) const;

	private:
		int stat, lx;
		hashalg_t hashAlg;
		Ptr<BankParameters> bankParameters;
		Ptr<BankParameters> publicBankParameters;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(stat)
				& auto_nvp(lx)
				& auto_nvp(hashAlg)
				// XXX also save public/private key files/params?
				& auto_nvp(bankParameters)
				& auto_nvp(publicBankParameters);
		}
};

#endif // BANKTOOL_H_
