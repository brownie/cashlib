
#ifndef _FEINITIATOR_H_
#define _FEINITIATOR_H_

#include "Wallet.h"
#include "FESetupMessage.h"
#include "FEMessage.h"
#include "BuyMessage.h"

/*! \brief This class can run the initiator side of either the buy or barter 
 * protocols */

class FEInitiator {
	public:
		/*! use these to decide type of interaction */
		static const int TYPE_NONE = -1;
		static const int TYPE_BUY = 0;
		static const int TYPE_BARTER = 1;

		/*! constructor takes in various parameters, as well as a wallet
		 * and the public key of the arbiter */
		FEInitiator(const long timeoutLength, const VEPublicKey* pk, 
					const VEPublicKey* regularpk, const int stat, 
					const Signature::Key* signKey = NULL);

		/*! copy constructor */
		FEInitiator(const FEInitiator &o);

		/*! destructor */
		~FEInitiator();

		/*! setup: creates the keys for the signature scheme, creates the
		 * unendorsed coin, and then computes a verifiable escrow (using
		 * the arbiter's PK) on the endorsement using the public key for
		 * the signature scheme as a label */
		FESetupMessage* setup(Wallet *wallet, const ZZ &R, 
							  const string &signAlg);
		/*! assumes setCoin() has been called */
		FESetupMessage* setup(const string &signAlg);
		
		
		// the following functions should be used only for buy

		/*! receives the ciphertext and computes the contract; also outputs
		 * a signature on the contract */
		FEMessage* buy(EncBuffer* ctextR, const hash_t& ptHashR);
		
		FEMessage* buy(const vector</*const*/ EncBuffer*>& ctextsR,
					   const vector</*const*/ hash_t>& ptHashesR);
		
		/*! pay the seller with the endorsement (if key is correct) */
		vector<ZZ> pay(const string& sellerKey);
		
		vector<ZZ> pay(const vector<string>& sellerKeys);
		
		
		// the following functions should be used only for barter
		EncBuffer* continueRound(const Buffer *ptextI, const cipher_t& encAlgI);
		
		vector<EncBuffer*> continueRound(const vector<const Buffer*>& ptextI,
										 const cipher_t& encAlgI);
		
		/*! set initiator files (only for BT client use) */
		void setInitiatorFiles(const Buffer* ptextI, EncBuffer* ctextI);
		
		void setInitiatorFiles(const vector<const Buffer*>& ptextI,
							   const vector<EncBuffer*>& ctextI);
		
		
		FEMessage* barter(EncBuffer* ctextR, const hash_t& ptHashR, 
						  const hash_t& ptHashI);
		FEMessage* barter(const vector<EncBuffer*>& ctextR, 
						  const vector<hash_t>& ptHashR,
						  const vector<hash_t>& ptHashI);
			
		/*! check if the input key decrypts ctextB by checking against known
		 * hash.  if check passes, return decryption key; if not throw an
		 * exception */
		vector<string> giveKeys(const string& keyR);
		vector<string> giveKeys(const vector<string>& keysR);
		
		
		/*! output the r used the generate the session ID */
		ZZ resolve();
		
		// get vector of decrypted plaintexts from responder
		const vector<Buffer*>& getPtextB() const { return ptextB; }
		
		bool canAbortLocally() {return TYPE_NONE == exchangeType;};
		
		
		// setters
		void setCoin(const Coin& c);
		void setTimeoutLength(const long newtimeout) 
								{timeoutLength = newtimeout;}
		void setSecurity(const int newstat) {stat = newstat;}
		void setVerifiablePublicKey(const VEPublicKey* newpk) 
								{verifiablePK = newpk;}
		void setRegularPublicKey(const VEPublicKey* newpk) {regularPK = newpk;}
		void setSignatureKey(Signature::Key* newsignKey = NULL)
		{ delete signKey; signKey = newsignKey ? new Signature::Key(*newsignKey) : NULL; }
		
		void reset();

	protected:
		void makeCoin(Wallet* wallet, const ZZ& R);
		
		void createContract();
		string signContract() const;
		
		bool decryptCheck(const vector<string>& keys);
		
		vector<EncBuffer*> encrypt(const vector<const Buffer*>& ptextI, 
								   const cipher_t& encAlgI) const;
		
	private:
		long timeoutLength;
		int stat;
		const VEPublicKey* verifiablePK;
		const VEPublicKey* regularPK;

		Coin coin;
		FEContract* contract;
		Signature::Key* signKey;

		// these correspond to the initiator's files (if doing barter)
		vector<const Buffer*> ptextA;
		vector<EncBuffer*> ctextA;
		// these correspond to the responder/seller's files
		vector</*const*/ EncBuffer*> ctextB;
		vector<Buffer*> ptextB;
		
		ZZ r;
		vector<ZZ> endorsement;

		// 0 indicates buy, 1 indicates barter
		int exchangeType;
};

#endif /*_FEINITIATOR_H_*/
