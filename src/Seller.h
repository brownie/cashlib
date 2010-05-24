
#ifndef _SELLER_H_
#define _SELLER_H_

#include "Coin.h"
#include "BuyMessage.h"
#include "MerkleProof.h"
#include "MerkleProver.h"

class Seller {
	public:
		/*! constructor takes in various parameters and the arbiter's
		 * public key */
		Seller(const int timeoutLength, const int timeoutTolerance, 
			   const VEPublicKey* pk, const int stat);

		/*! constructor for when ctext has already been encrypted:
		* ctext containing key and encAlg: will be freed by Seller
		*/
		Seller(EncBuffer* ctext, const int timeout, const int timeoutTolerance, 
               const VEPublicKey* pk, const int stat);

		Seller(vector<EncBuffer*> ctext, const int timeout, 
			   const int timeoutTolerance, const VEPublicKey* pk, 
			   const int stat);
		
		/*! copy constructor */
		Seller(const Seller &o);
		
		/*! destructor */
		~Seller();

		/*! outputs the ciphertext of the file */
		EncBuffer* encrypt(const Buffer* ptext, const cipher_t& encAlg, 
						   const string& key = "");
		vector<EncBuffer*> encrypt(const vector<const Buffer*>& ptext,
								   const cipher_t& encAlgs);
		
		/*! For BT client use only (if encrypt is not called) */
		void setFiles(const Buffer* ptext, EncBuffer* ctext);
		void setFiles(const vector<const Buffer*>& ptext, 
					  const vector<EncBuffer*>& ctext);

		/*! if the message from the buyer is formed correctly, meaning
		 * the contract is correctly formed (matches seller's knowledge),
		 * and the coin is formed correctly (using info/randomness sent 
		 * by the seller),
		 * and the verifiable escrow of the endorsement verifies,
		 * then return the decryption key */
		vector<string> sell(const BuyMessage* buyerInput, const ZZ& R, 
							const hash_t& ptHash);
		vector<string> sell(const BuyMessage* buyerInput, const ZZ& R, 
							const vector<hash_t>& ptHashes);
		
		/*! Endorse the coin previously obtained in this exchange using
		 * the endorsement provided by the buyer. If the endorsement does
		 * NOT match the coin, return false */
		bool endorseCoin(const vector<ZZ> &endorsement);
        
        /*! Send the information required for resolution to the Arbiter */
		pair<vector<string>, BuyMessage*> resolveI();
		
		/* Prove knowledge to the Arbiter of the challenged blocks*/
		MerkleProof* resolveII(vector<unsigned> &challenges);
		
		// getters
		const Coin* getCoin() const { return new Coin(coinPrime); };
		BuyMessage* getBuyMessage() const;
		
		// setters
		void setTimeoutLength(const int newLength) 
						{timeoutLength = newLength;}
		void setTimeoutTolerance(const int newtimeoutTolerance) 
						{timeoutTolerance = newtimeoutTolerance;}
		void setSecurity(const int newstat) {stat = newstat;}
		void setVEPublicKey(const VEPublicKey* newpk) {pk = newpk;}
		
		bool canAbortLocally() const {return !inProgress;};
		void reset();
				
	protected:
		bool check(const BuyMessage* buyerInput, const ZZ& R,
				   const vector<hash_t>& ptHashes);
	
	private:
		int timeoutLength;
		int timeoutTolerance;
		int stat;
		const VEPublicKey* pk;

		// Seller has encrypted ptext and sent ctext to Buyer
		vector<const Buffer*> ptext;
		// Seller will exchange ctext->key for Buyer's coin
		vector<EncBuffer*> ctext;

		Coin coinPrime;
		const FEContract* contract;
		const VECiphertext* escrow;
		
		bool inProgress;
};

#endif /*_SELLER_H_*/
