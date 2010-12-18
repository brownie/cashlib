
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
					Ptr<const VEPublicKey> pk, Ptr<const VEPublicKey> repk, 
					const int stat);
		
		/*! copy constructor */
		FEResponder(const FEResponder &o);
		
		/*! destructor */
		~FEResponder();

		/*! check a setup message received from FEInitiator */
		bool setup(Ptr<const FESetupMessage> setup, const ZZ& R);
			
		/*! encrypt and start the next round (including first round) */
		Ptr<EncBuffer> startRound(Ptr<const Buffer> ptextR, const cipher_t& encAlgR);
		vector<Ptr<EncBuffer> > startRound(const vector<Ptr<const Buffer> >& ptextR,
									  const cipher_t& encAlgR);
		
		/*! set responder files (only for BT client use) */
		void setResponderFiles(Ptr<const Buffer> ptextR, Ptr<EncBuffer> ctextR);
		
		void setResponderFiles(const vector<Ptr<const Buffer> >& ptextR,
								const vector<Ptr<EncBuffer> >& ctextR);
		
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
								Ptr<EncBuffer> ctextI, const hash_t& ptHashI,
								const hash_t& ptHashR);

		vector<string> giveKeys(const FEMessage& signedEscrow,
								const vector<Ptr<EncBuffer> >& ctextI,
								const vector<hash_t>& ptHashI,
								const vector<hash_t>& ptHashR);
		
		/*! check that the key received from the initiator is valid for the
		 * ciphertext; return false if it is not */
		bool checkKey(const string& keyI);
		bool checkKey(const vector<string>& keysI);
	
		/*! Send a request to the arbiter to resolve*/
		Ptr<FEResolutionMessage> resolveI();

		/*! Send proofs that you know the plaintext of the blocks 
		 * indicated by the challengs to the Arbiter */
		Ptr<MerkleProof> resolveII(vector<unsigned> &challenges);

		/*! Check that the keys sent by the Arbiter enable you to decrypt 
		 * the Initiator's ciphertext.  If they don't, send a proof of this */
		Ptr<MerkleProof> resolveIII(vector<string> &keys);

		/*! If the proof of the initiator's keys not decrypting the 
		 * ciphertext correctly is valid
		 * the Arbiter sends the endorsement for the coin*/
		// XXX: why does this have to be a function?
		bool resolveIV(vector<ZZ> &endorsement);
		
		// getters
		const vector<Ptr<Buffer> >& getPtextA() const { return ptextA; }
		const Coin& getCoinPrime() const { return coinPrime; }
		// these are only needed for testing
		Ptr<FESetupMessage> getSetupMessage() const;
		Ptr<FEMessage> getMessage() const { return message; }
		
		bool canAbortLocally() {return TYPE_NONE == exchangeType;}
		
		// setters
		void setTimeout() {timeout = time(NULL) + timeoutLength;}
		void setTimeoutTolerance(const int newtimeoutTolerance) 
							{timeoutTolerance = newtimeoutTolerance;}
		void setSecurity(const int newstat) {stat = newstat;}
		void setVerifiablePublicKey(Ptr<const VEPublicKey> newpk) 
							{verifiablePK = newpk;}
		void setRegularPublicKey(Ptr<const VEPublicKey> newpk) {regularPK = newpk;}
		
		void reset();

	protected:
		vector<Ptr<EncBuffer> > encrypt(const vector<Ptr<const Buffer> >& ptextR, 
                                        const cipher_t& encAlgR) const;
				
		bool check(const FEMessage& message, const string& label,
				   const vector<hash_t>& ptHashR);
		
		vector<string> getKeys() const;
		
		/*! Prove the initiator's keys are not correct for the ciphertext */
		Ptr<MerkleProof> proveIncorrectKeys(const vector<string> &keys);
		
	private:
		int timeoutLength;
		int timeoutTolerance;
		int timeout;
		int stat;
		Ptr<const VEPublicKey> verifiablePK;
		Ptr<const VEPublicKey> regularPK;

		vector<Ptr<const Buffer> > ptextB;
		vector<Ptr<EncBuffer> > ctextB;
		vector<Ptr<EncBuffer> > ctextA; // const?
		vector<Ptr<Buffer> > ptextA;

		Coin coinPrime;
		Ptr<FEContract> contract;
		Ptr<VECiphertext> escrow;
		Ptr<Signature::Key> initiatorSignPK;
		// 0 is buy, 1 is barter
		int exchangeType;
		Ptr<FEMessage> message;
};

#endif /*_FERESPONDER_H_*/
