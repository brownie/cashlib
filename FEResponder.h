
#ifndef _FERESPONDER_H_
#define _FERESPONDER_H_

#include "Coin.h"
#include "MerkleProof.h"
#include "MerkleProver.h"
#include "FEResolutionMessage.h"

/*! \brief This class can run the response side of either the buy or barter
 * protocols */

class FEResponder {
	public:
		static const int TYPE_NONE = -1;
		static const int TYPE_BUY = 0;
		static const int TYPE_BARTER = 1;
		
		/*! constructor takes in various parameters and the public key of
		 * the arbiter */
		FEResponder(const int timeoutLength, const int timeoutTolerance, 
					const VEPublicKey* pk, const VEPublicKey* repk, 
					const int stat);
		
		/*! copy constructor */
		FEResponder(const FEResponder &o);
		
		/*! destructor */
		~FEResponder();

		/*! check a setup message received from FEInitiator */
		bool setup(const FESetupMessage* setup, const ZZ& R);
			
		/*! encrypt and start the next round (including first round) */
		EncBuffer* startRound(const Buffer* ptextR, const cipher_t& encAlgR);
		vector<EncBuffer*> startRound(const vector<const Buffer*>& ptextR,
									  const cipher_t& encAlgR);
		
		/*! set responder files (only for BT client use) */
		void setResponderFiles(const Buffer* ptextR, EncBuffer* ctextR);
		
		void setResponderFiles(const vector<const Buffer*>& ptextR,
								const vector<EncBuffer*>& ctextR);
		
		// the following should be used for sell only
		/*! if the contract is formed correctly and the signature on the
		 * contract is valid, return the decryption key */
		vector<string> sell(const FEMessage& message, const hash_t& ptHashR);
		vector<string> sell(const FEMessage& message, 
							const vector<hash_t>& ptHashR);
		
		/*! endorse the coin from the setup message using the provided
		 * endorsement.  return false if the endorsement is not correct for
		 * the coin */
		bool endorseCoin(const vector<ZZ> &endorsement);
		
		// the following should be used for barter only

		/*! if the contract is correctly and the signature on the escrow
		 * verifies, return the decryption key */
		vector<string> giveKeys(const FEMessage& signedEscrow,
								EncBuffer* ctextI, const hash_t& ptHashI,
								const hash_t& ptHashR);

		vector<string> giveKeys(const FEMessage& signedEscrow,
								const vector<EncBuffer*>& ctextI,
								const vector<hash_t>& ptHashI,
								const vector<hash_t>& ptHashR);
		
		/*! check that the key received from the initiator is valid for the
		 * ciphertext; return false if it is not */
		bool checkKey(const string& keyI);
		bool checkKey(const vector<string>& keysI);
	
		/*! Send a request to the arbiter to resolve*/
		FEResolutionMessage* resolveI();

		/*! Send proofs that you know the plaintext of the blocks 
		 * indicated by the challengs to the Arbiter */
		MerkleProof* resolveII(vector<unsigned> &challenges);

		/*! Check that the keys sent by the Arbiter enable you to decrypt 
		 * the Initiator's ciphertext.  If they don't, send a proof of this */
		MerkleProof* resolveIII(vector<string> &keys);

		/*! If the proof of the initiator's keys not decrypting the 
		 * ciphertext correctly is valid
		 * the Arbiter sends the endorsement for the coin*/
		// XXX: why does this have to be a function?
		bool resolveIV(vector<ZZ> &endorsement);
		
		// getters
		const vector<Buffer*>& getPtextA() const { return ptextA; }
		const Coin& getCoinPrime() const { return coinPrime; }
		// these are only needed for testing
		FESetupMessage* getSetupMessage() const;
		FEMessage* getMessage() const { return message; }
		
		bool canAbortLocally() {return TYPE_NONE == exchangeType;}
		
		// setters
		void setTimeout() {timeout = time(NULL) + timeoutLength;}
		void setTimeoutTolerance(const int newtimeoutTolerance) 
							{timeoutTolerance = newtimeoutTolerance;}
		void setSecurity(const int newstat) {stat = newstat;}
		void setVerifiablePublicKey(const VEPublicKey* newpk) 
							{verifiablePK = newpk;}
		void setRegularPublicKey(const VEPublicKey* newpk) {regularPK = newpk;}
		
		void reset();

	protected:
		vector<EncBuffer*> encrypt(const vector<const Buffer*>& ptextR, 
								   const cipher_t& encAlgR) const;
				
		bool check(const FEMessage& message, const string& label,
				   const vector<hash_t>& ptHashR);
		
		vector<string> getKeys() const;
		
		/*! Prove the initiator's keys are not correct for the ciphertext */
		MerkleProof* proveIncorrectKeys(const vector<string> &keys);
		
	private:
		int timeoutLength;
		int timeoutTolerance;
		int timeout;
		int stat;
		const VEPublicKey* verifiablePK;
		const VEPublicKey* regularPK;

		vector<const Buffer*> ptextB;
		vector<EncBuffer*> ctextB;
		vector</*const*/EncBuffer*> ctextA;
		vector<Buffer*> ptextA;

		Coin coinPrime;
		FEContract* contract;
		const VECiphertext* escrow;
		Signature::Key* initiatorSignPK;
		// 0 is buy, 1 is barter
		int exchangeType;
		FEMessage* message;
};

#endif /*_FERESPONDER_H_*/
