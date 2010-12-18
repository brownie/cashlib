
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
		Buyer(int timeoutLength, Ptr<const VEPublicKey> pk, int stat);

		/*! copy constructor */
		Buyer(const Buyer &o);

		/*! destructor */
		~Buyer();
		
		/*! buy the ciphertext by sending an unendorsed coin,
		 * a verifiable escrow of the endorsement, and associated 
		 * contract */
		Ptr<BuyMessage> buy(Ptr<Wallet> wallet, Ptr<EncBuffer> ctext, 
						const hash_t& ptHash, const ZZ &R);
		
		/*! buy multiple files */
		Ptr<BuyMessage> buy(Ptr<Wallet> wallet, const vector<Ptr<EncBuffer> >& ctext,
						const vector<hash_t>& ptHash, const ZZ &R);

		/*! assumes setCoin() has been called */
		Ptr<BuyMessage> buy(Ptr<EncBuffer> ctext, const hash_t& ptHash);
		
		/*! assumes setCoin() has been called */
		Ptr<BuyMessage> buy(const vector<Ptr<EncBuffer> >& ctext, 
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
		const vector<Ptr<Buffer> >& getPtext() const { return ptext; }

		/*! get the endorsement (send after key is checked) */
		const vector<ZZ>& getEndorsement() const { return endorsement; }
		
		bool canAbortLocally() {return !inProgress; };
		
		// setters
		void setCoin(const Coin& coin);
		void setTimeout() { timeout =time(NULL) + timeoutLength; }
		void setSecurity(const int newstat) {stat = newstat;}
		void setVEPublicKey(Ptr<const VEPublicKey> newpk) {pk = newpk;}
		
		void reset();
		
	protected:
		void createContract();
		void makeCoin(Wallet& w, const ZZ& R); 
		VECiphertext makeEscrow();
		
	private:
		int timeout;
		int timeoutLength;
		int stat;
		Ptr<const VEPublicKey> pk;

		Coin coin;
		Ptr<FEContract> contract;

		// ciphertext to decrypt, received from Seller
		vector<Ptr<EncBuffer> > ctext;
		// saves output of decrypt() if Buy protocol is successful
		vector<Ptr<Buffer> > ptext;
		
		ZZ r;
		vector<ZZ> endorsement;
		bool inProgress;
};

#endif /*BUYER_H_*/
