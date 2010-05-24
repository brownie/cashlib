
#ifndef _BUYER_H_
#define _BUYER_H_

#include "Wallet.h"
#include "VEPublicKey.h"
#include "BuyMessage.h"
#include "Ciphertext.h"
#include "FESetupMessage.h"

class Buyer {
	public:
		/*! constructor takes in various parameters, a wallet, and the
		 * arbiter's public key */
		Buyer(int timeoutLength, const VEPublicKey* pk, int stat);

		/*! copy constructor */
		Buyer(const Buyer &o);

		/*! destructor */
		~Buyer();
		
		/*! buy the ciphertext by sending an unendorsed coin,
		 * a verifiable escrow of the endorsement, and associated 
		 * contract */
		BuyMessage* buy(Wallet* wallet, EncBuffer* ctext, 
						const hash_t& ptHash, const ZZ &R);
		
		/*! buy multiple files */
		BuyMessage* buy(Wallet* wallet, const vector<EncBuffer*>& ctext,
						const vector<hash_t>& ptHash, const ZZ &R);

		/*! assumes setCoin() has been called */
		BuyMessage* buy(EncBuffer* ctext, const hash_t& ptHash);
		
		/*! assumes setCoin() has been called */
		BuyMessage* buy(const vector<EncBuffer*>& ctext, 
						const vector<hash_t>& ptHash);
		
		
		
		/*! pay the seller with the endorsement if the key
		 * given by the seller is correct 
		 * use same key for multiple ciphertexts 
		 * if given just one key but multiple ciphertexts */
		vector<ZZ> pay(const string &key);
		vector<ZZ> pay(const vector<string>& keys);
		bool checkKey(const vector<string>& keys);
		
		
		/*! outputs the random r used for the sessson ID */
		ZZ resolve();
		
		
		/*! get the plaintext (if stored as string in Buyer) */
		//string getFile() const { return *ptext; }
		const vector<Buffer*>& getPtext() const { return ptext; }

		/*! get the endorsement (send after key is checked) */
		const vector<ZZ>& getEndorsement() const { return endorsement; }
		
		bool canAbortLocally() {return !inProgress; };
		
		// setters
		void setTimeout() { timeout =time(NULL) + timeoutLength; }
		void setSecurity(const int newstat) {stat = newstat;}
		void setVEPublicKey(const VEPublicKey* newpk) {pk = newpk;}
		
		void reset();
		
	protected:
		void createContract();
		void makeCoin(Wallet& w, const ZZ& R); 
		VECiphertext makeEscrow();
		
	private:
		int timeout;
		int timeoutLength;
		int stat;
		const VEPublicKey* pk;

		Coin coin;
		FEContract* contract;

		// ciphertext to decrypt, received from Seller
		vector<EncBuffer*> ctext;
		// saves output of decrypt() if Buy protocol is successful
		vector<Buffer*> ptext;
		
		ZZ r;
		vector<ZZ> endorsement;
		bool inProgress;
};

#endif /*BUYER_H_*/
