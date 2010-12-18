
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
		FESetupMessage(const Coin &coinPrime, VECiphertext* escrow, 
                	   const Signature::Key &signPK)
		: coinPrime(coinPrime), escrow(escrow), signPK(signPK.getPublicKey()) {}

		/*! copy constructor */
		FESetupMessage(const FESetupMessage &o)
			: coinPrime(o.coinPrime), escrow(o.escrow),
			  signPK(new Signature::Key(*o.signPK)) {}
		
		/*! destructor */
        ~FESetupMessage() { delete signPK; }
		
		/*! check contents (verify coin and escrow) */
		bool check(const VEPublicKey* pk, const int stat, const ZZ& R) const;
		
		/*! getters for coin', escrow, and signature PK */
		Coin getCoinPrime() const { return coinPrime; }
		VECiphertext* getEscrow() const { return escrow; }
		const Signature::Key* getPK() const { return signPK; }
				
	private:
		Coin coinPrime;
		VECiphertext* escrow;
		Signature::Key* signPK;

        friend class boost::serialization::access;
        template <class Archive> 
        void serialize(Archive& ar, const unsigned int ver) {
            ar  & auto_nvp(coinPrime)
                & auto_nvp(escrow)
                & auto_nvp(signPK);
        }
		
};

#endif /*SETUPMESSAGE_H_*/
