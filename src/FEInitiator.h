
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
		FEInitiator(const long timeoutLength, Ptr<const VEPublicKey> pk, 
					Ptr<const VEPublicKey> regularpk, const int stat, 
					Ptr<Signature::Key> signKey = Ptr<Signature::Key>());

		/*! destructor */
		~FEInitiator();

		/*! setup: creates the keys for the signature scheme, creates the
		 * unendorsed coin, and then computes a verifiable escrow (using
		 * the arbiter's PK) on the endorsement using the public key for
		 * the signature scheme as a label */
		Ptr<FESetupMessage> setup(Ptr<Wallet> wallet, const ZZ &R, 
							  const string &signAlg);
		/*! assumes setCoin() has been called */
		Ptr<FESetupMessage> setup(const string &signAlg);
		
		
		// the following functions should be used only for buy

		/*! receives the ciphertext and computes the contract; also outputs
		 * a signature on the contract */
		Ptr<FEMessage> buy(Ptr<EncBuffer> ctextR, const hash_t& ptHashR);
		
		Ptr<FEMessage> buy(const vector</*const*/ Ptr<EncBuffer> >& ctextsR,
					   const vector</*const*/ hash_t>& ptHashesR);
		
		/*! pay the seller with the endorsement (if key is correct) */
		vector<ZZ> pay(const string& sellerKey);
		
		vector<ZZ> pay(const vector<string>& sellerKeys);
		
		
		// the following functions should be used only for barter
		Ptr<EncBuffer> continueRound(Ptr<const Buffer> ptextI, const cipher_t& encAlgI);
		
		vector<Ptr<EncBuffer> > continueRound(const vector<Ptr<const Buffer> >& ptextI,
										 const cipher_t& encAlgI);
		
		/*! set initiator files (only for BT client use) */
		void setInitiatorFiles(Ptr<const Buffer> ptextI, Ptr<EncBuffer> ctextI);
		
		void setInitiatorFiles(const vector<Ptr<const Buffer> >& ptextI,
							   const vector<Ptr<EncBuffer> >& ctextI);
		
		
		Ptr<FEMessage> barter(Ptr<EncBuffer> ctextR, const hash_t& ptHashR, 
						  const hash_t& ptHashI);
		Ptr<FEMessage> barter(const vector<Ptr<EncBuffer> >& ctextR, 
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
		const vector<Ptr<Buffer> >& getPtextB() const { return ptextB; }
		
		bool canAbortLocally() {return TYPE_NONE == exchangeType;};
		
		
		// setters
		void setCoin(const Coin& c);
		void setTimeoutLength(const long newtimeout) 
								{timeoutLength = newtimeout;}
		void setSecurity(const int newstat) {stat = newstat;}
		void setVerifiablePublicKey(Ptr<const VEPublicKey> newpk) 
								{verifiablePK = newpk;}
		void setRegularPublicKey(Ptr<const VEPublicKey> newpk) {regularPK = newpk;}
		void setSignatureKey(Ptr<Signature::Key> sk=Ptr<Signature::Key>())
                                { signKey = sk; }
		void setExchangeType(int et) { exchangeType = et; }
		
		void reset();

	protected:
		void makeCoin(Ptr<Wallet> wallet, const ZZ& R);
		
		void createContract();
		string signContract() const;
		
		bool decryptCheck(const vector<string>& keys);
		
		vector<Ptr<EncBuffer> > encrypt(const vector<Ptr<const Buffer> >& ptextI, 
								   const cipher_t& encAlgI) const;
		
	private:
		long timeoutLength;
		int stat;
		Ptr<const VEPublicKey> verifiablePK;
		Ptr<const VEPublicKey> regularPK;

		Coin coin;
		Ptr<FEContract> contract;
		Ptr<Signature::Key> signKey;

		// these correspond to the initiator's files (if doing barter)
		vector<Ptr<const Buffer> > ptextA;
		vector<Ptr<EncBuffer> > ctextA;
		// these correspond to the responder/seller's files
		vector<Ptr</*const*/ EncBuffer> > ctextB;
		vector<Ptr<Buffer> > ptextB;
		
		ZZ r;
		vector<ZZ> endorsement;

		// 0 indicates buy, 1 indicates barter
		int exchangeType;
};

#endif /*_FEINITIATOR_H_*/
