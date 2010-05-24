
#include "Wallet.h"

Wallet::Wallet(const ZZ &sk, const ZZ &sIn, const ZZ &tIn, int size,
			   int d, const BankParameters* bp, int st, int l, 
			   const hashalg_t &alg, const vector<ZZ> &sig)
	: sk_u(sk), s(sIn), t(tIn), walletSize(size), coinDenom(d), 
	  numCoinsUsed(0), params(bp), stat(st), lx(l), hashAlg(alg), 
	  signature(sig)
{
	// generate a list of the indices that haven't yet been added to
	// the spendOrder vector
	for(int i = 0; i < walletSize; i++) {
		//spendOrder[i] = i;
		spendOrder.push_back(i);
	}

	// fill the spendOrder vector
	// On iteration i, remaining free indices are values
	// remainingIndices[0], ..., remainingIndices[walletSize - i - 1]
	for(int i = 0; i < walletSize; i++) {
		int index = RandomBnd(walletSize);
		int temp = spendOrder[i];
		spendOrder[i] = spendOrder[index];
		spendOrder[index] = temp;
	}
#ifdef DEBUG
	for(int i = 0; i < walletSize; i++) {
		cout << spendOrder[i] << " ";
	}
#endif //DEBUG
}

Wallet::Wallet(const Wallet &o) 
	: sk_u(o.sk_u), s(o.s), t(o.t), walletSize(o.walletSize),
	  coinDenom(o.coinDenom), numCoinsUsed(o.numCoinsUsed), params(o.params),
	  stat(o.stat), lx(o.lx), hashAlg(o.hashAlg), signature(o.signature),
	  spendOrder(o.spendOrder)
{
}

int Wallet::nextCoinIndex() {
	if(numCoinsUsed >= walletSize) {
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"Tried to get the next coin from a wallet from which every "
				"coin had already been spent");
	}
	return spendOrder[numCoinsUsed++];
}

Coin Wallet::nextCoin(const ZZ &rValue) {
	int i = nextCoinIndex();
	Coin* c = newCoin(rValue, i);
	Coin ret(*c);
	delete c;
	return ret;
}

Coin* Wallet::newCoin(const ZZ &rValue, int coinIndex) {
	return new Coin(params, walletSize, coinIndex, sk_u, s, t, signature, stat, 
					lx, rValue, coinDenom, hashAlg);
}

bool Wallet::replaceCoin(ZZ &index) {
	// check that 0 <= indexArg < walletSize
	if(index < 0 || index >= walletSize) {
		return false;
	}
	
	// search through "coin spend order" to see if that coin index has been 
	// withdrawn yet or not
	for(unsigned i = numCoinsUsed; i < spendOrder.size(); i++) {
		if(spendOrder[i] == index) {
			return false;
		}
	}
	numCoinsUsed--;
	spendOrder[numCoinsUsed] = to_int(index);
	return true;
}
