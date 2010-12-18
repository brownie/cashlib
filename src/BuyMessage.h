
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
		BuyMessage(const Coin &coinPrime, FEContract* contract, 
				   VECiphertext* escrow)
			: coinPrime(coinPrime), contract(contract), escrow(escrow) {}

		/*! copy constructor */
		BuyMessage(const BuyMessage &o) 
			: coinPrime(o.coinPrime), contract(o.contract), escrow(o.escrow) {}

		~BuyMessage() { /*delete contract;*/ delete escrow; }

		//BuyMessage(const string& s, const BankParameters *bankP)
		//	: coinPrime(bankP), contract(), escrow() {loadString(*this, s);}

		/*! check contents (verify coin and escrow) */
		bool check(const VEPublicKey* pk, const int stat, const ZZ& R) const;

		// getters
		Coin getCoinPrime() const { return coinPrime; }
		FEContract* getContract() const { return contract; }
		VECiphertext* getEscrow() const { return escrow; }

	private:
		Coin coinPrime;
		FEContract* contract;
		VECiphertext* escrow;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(coinPrime)
				& auto_nvp(contract)
				& auto_nvp(escrow);
		}
};

#endif /*BUYMESSAGE_H_*/
