
#ifndef _BUYMESSAGE_H_
#define _BUYMESSAGE_H_

#include "Coin.h"
#include "FEContract.h"
#include "VECiphertext.h"
#include "VEPublicKey.h"

/*! \brief This class is just a container for the message sent by the 
 * Buyer in the buy protocol - it contains the unendorsed coin and the 
 * escrow of the endorsement for the coin */

class BuyMessage {
	public:
		/*! coinPrime represents the unendorsed coin, contract represents
		 * the label for the verifiable encryption, and escrow represents
		 * the verifiable escrow on the coin's endorsement */
		BuyMessage(Ptr<Coin> coinPrime, Ptr<FEContract> contract, 
				   Ptr<VECiphertext> escrow)
			: coinPrime(new_ptr<Coin>(*coinPrime)), 
              contract(new_ptr<FEContract>(*contract)), 
              escrow(new_ptr<VECiphertext>(*escrow)) {}

		/*! copy constructor */
		BuyMessage(const BuyMessage &o) 
			: coinPrime(o.coinPrime), contract(o.contract), escrow(o.escrow) {}

		BuyMessage(const string& s, Ptr<const BankParameters> params) {
			// need to set params for Coin contained in message
			loadGZString(make_nvp("BuyMessage", *this), s); 
			coinPrime->setParameters(params);
		}

		/*! check contents (verify coin and escrow) */
		bool check(Ptr<const VEPublicKey> pk, const int stat, const ZZ& R) const;

		// getters
		Ptr<Coin> getCoinPrime() const { return coinPrime; }
		Ptr<FEContract> getContract() const { return contract; }
		Ptr<VECiphertext> getEscrow() const { return escrow; }

	private:
		// XXX these should all be shared_ptr?
		Ptr<Coin> coinPrime;
		Ptr<FEContract> contract;
		Ptr<VECiphertext> escrow;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(coinPrime)
				& auto_nvp(contract)
				& auto_nvp(escrow);
		}
};

#endif /*BUYMESSAGE_H_*/
