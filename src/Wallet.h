
#ifndef _WALLET_H_
#define _WALLET_H_

#include "Coin.h"

class Wallet {
	public:
		Wallet(const ZZ &sk, const ZZ &sIn, const ZZ &tIn, int size,
			   int d, Ptr<const BankParameters> b, int st, int l, 
			   const hashalg_t &alg, const vector<ZZ> &sig);
		
		Wallet(const Wallet &o);

		/*! constructor to load from file */
		Wallet(const char* fname, Ptr<const BankParameters> bp)
			: params(bp) 
			{ loadFile(make_nvp("Wallet", *this), fname); }

		/*! gets the next index for the coin
		 *  this increments the numCoinsUsed counter by one */
		int nextCoinIndex();

		/*! returns the next coin to use */
		Ptr<Coin> nextCoin(const ZZ &R);

		/*! returns a coin for a given index */
        Ptr<Coin> newCoin(const ZZ &R, int coinIndex);
		void newCoin(Coin& coin, const ZZ &R, int coinIndex);

		bool replaceCoin(const ZZ &index);

		// some accessors
		int getWalletSize() const { return walletSize; }
		int getNumCoinsUsed() const { return numCoinsUsed; }
		int getNumCoinsLeft() const { return walletSize - numCoinsUsed; }
		int getDenomination() const { return coinDenom; }
		int getRemainingValue() const { return getNumCoinsLeft() * coinDenom; }
		bool empty() const { return (walletSize == numCoinsUsed); }

	private:
		ZZ sk_u, s, t;
		int walletSize;
		int coinDenom;
		int numCoinsUsed;

		Ptr<const BankParameters> params; 
		int stat;
		int lx;
		hashalg_t hashAlg;
		vector<ZZ> signature;
		vector<int> spendOrder;

		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int ver) {
			ar	& auto_nvp(sk_u) // XXX: should be encrypted
				& auto_nvp(s)
				& auto_nvp(t)
				& auto_nvp(walletSize)
				& auto_nvp(coinDenom)
				& auto_nvp(numCoinsUsed)
				& auto_nvp(stat)
				& auto_nvp(lx)
				& auto_nvp(hashAlg)
				& auto_nvp(signature)
				& auto_nvp(spendOrder)
				;
		}
};

#endif /*_WALLET_H_*/
