
#ifndef _FESETUPMESSAGE_H_
#define _FESETUPMESSAGE_H_

#include "Coin.h"
#include "VECiphertext.h"
#include "Signature.h"
#include "VEPublicKey.h"

/*! \brief This class is a wrapper for the setup message send in Step 1 of the
 * 	barter protocol */

class FESetupMessage {

	public:
		/*! constructor takes in unendorsed coin, escrow (encryption of
		 * the endorsement for the coin), and the public key
		 * for the initiator's signature scheme (label for the 
		 * verifiable escrow) */
		FESetupMessage(const Coin &coinPrime, Ptr<VECiphertext> escrow, 
                	   const Signature::Key &signPK)
		: coinPrime(coinPrime), escrow(escrow), signPK(signPK.getPublicKey()) {}

		/*! copy constructor */
		FESetupMessage(const FESetupMessage &o)
			: coinPrime(o.coinPrime), escrow(o.escrow),
			  signPK(new_ptr<Signature::Key>(*o.signPK)) {}
		
		FESetupMessage(const string& s, Ptr<const BankParameters> params) {
			// need to set params for Coin contained in message
			loadGZString(make_nvp("FESetupMessage", *this), s); 
			coinPrime.setParameters(params);
		}
		
		/*! check contents (verify coin and escrow) */
		bool check(Ptr<const VEPublicKey> pk, const int stat, const ZZ& R) const;
		
		/*! getters for coin', escrow, and signature PK */
		Coin getCoinPrime() const { return coinPrime; }
		Ptr<VECiphertext> getEscrow() const { return escrow; }
		Ptr<const Signature::Key> getPK() const { return signPK; }
				
	private:
		Coin coinPrime;
		Ptr<VECiphertext> escrow;
		Ptr<Signature::Key> signPK;

        friend class boost::serialization::access;
        template <class Archive> 
        void serialize(Archive& ar, const unsigned int ver) {
            ar  & auto_nvp(coinPrime)
                & auto_nvp(escrow)
                & auto_nvp(signPK);
        }
		
};

#endif /*SETUPMESSAGE_H_*/
