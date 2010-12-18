
#include "Ciphertext.h"
#include "Buyer.h"
#include "VEProver.h"

#include "Timer.h"

/*----------------------------------------------------------------------------*/
// Constructors
Buyer::Buyer(int timeoutLength, Ptr<const VEPublicKey> pk, int stat)
	: timeoutLength(timeoutLength), stat(stat), pk(pk), 
	  contract(NULL), inProgress(false)
{
}

Buyer::Buyer(const Buyer& o)
	: timeoutLength(o.timeoutLength), stat(o.stat), pk(o.pk),
	  contract(o.contract? new_ptr<FEContract>(*o.contract) : NULL),
	  r(o.r), endorsement(o.endorsement), inProgress(o.inProgress)
{
}
/*----------------------------------------------------------------------------*/
// Destructor
Buyer::~Buyer() {
	reset();
}

void Buyer::reset() {
	inProgress = false;
#ifdef DELETE_BUFFERS
	for (unsigned i = 0; i < ptext.size(); i++) {
		delete ptext[i];
    }
#endif
	ptext.clear();
	delete contract;
	contract = NULL;
}
/*----------------------------------------------------------------------------*/
// Buy
Ptr<BuyMessage> Buyer::buy(Ptr<Wallet> wallet, Ptr<EncBuffer> ciphertext, 
					   const hash_t& ptHash, const ZZ& R) {
	return buy(wallet, CommonFunctions::vectorize<Ptr<EncBuffer> >(ciphertext),
			   CommonFunctions::vectorize<hash_t>(ptHash), R);
}

Ptr<BuyMessage> Buyer::buy(Ptr<Wallet> wallet, const vector<Ptr<EncBuffer> >& ctext,
					   const vector<hash_t>& ptHash, const ZZ &R) {
	startTimer();
	makeCoin(*wallet, R);
	printTimer("[Buyer::buy] created coin");
	return buy(ctext, ptHash);
}

Ptr<BuyMessage> Buyer::buy(Ptr<EncBuffer> ciphertext, const hash_t& ptHash) {
	return buy(CommonFunctions::vectorize<Ptr<EncBuffer> >(ciphertext),
			   CommonFunctions::vectorize<hash_t>(ptHash));
}

Ptr<BuyMessage> Buyer::buy(const vector<Ptr<EncBuffer> >& ct, 
					   const vector<hash_t>& ptHash) {
	if (inProgress)
		throw CashException(CashException::CE_FE_ERROR,
			"[Buyer::buy] Buy called on an already working buyer");
	if (ct.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[Buyer::buy] No ciphertext given");
	if (ptHash.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[Buyer::buy] No plaintext hash given");
	// store ciphertexts
	ctext = ct;
	
	// compute hashes
	// XXX temporary fix: use regular hashes (size-1 hashes aren't matching)
	startTimer();
	hash_t ptHashMerkle = Hash::hash(ptHash, pk->hashAlg, pk->hashKey, 
									 Hash::TYPE_MERKLE);
	hash_t ctHashMerkle = Hash::hash(ctext, pk->hashAlg, pk->hashKey, 
									 Hash::TYPE_MERKLE);
	
	// create contract
	createContract();
	
	// set up the contract
	contract->setPTHashB(ptHashMerkle);
	contract->setCTHashB(ctHashMerkle);
	contract->setEncAlgB(ctext[0]->encAlg);
	contract->setPTHashBlocksB(ptHash.size());
	contract->setCTHashBlocksB(ctext.size());
	printTimer("[Buyer::buy] created contract");
	
	startTimer();
	// set up the escrow
	Ptr<VECiphertext> escrow = new_ptr<VECiphertext>(makeEscrow());
	printTimer("[Buyer::buy] created escrow");

	// set inProgress
	inProgress = true;
	
	return new_ptr<BuyMessage>(coin, contract, escrow);
}

void Buyer::createContract() {
	// set up session id
	r = RandomBits_ZZ(2*stat);
	ZZ id = Hash::hash(r, pk->hashAlg, pk->hashKey);
	// prepare timeout
	setTimeout();
	contract = new_ptr<FEContract>(timeout, id);
}

VECiphertext Buyer::makeEscrow() {
	// now set up the verifiable encryption
	VEProver prover(pk);
	return prover.verifiableEncrypt(coin.getEndorsementCom(), endorsement, 
									coin.getCashGroup(), saveString(*contract), 
									pk->hashAlg, stat);
}

void Buyer::makeCoin(Wallet& w, const ZZ& R) {	
	// get coin from wallet
	setCoin( w.nextCoin(R) );
}

void Buyer::setCoin(const Coin& c)
{	
	coin = c;
	endorsement = coin.getEndorsement();
	coin.unendorse();
}

/*----------------------------------------------------------------------------*/
// Pay
vector<ZZ> Buyer::pay(const string& key) {
	return pay(CommonFunctions::vectorize<string>(key));
}

vector<ZZ> Buyer::pay(const vector<string>& keys) {
	checkKey(keys);
	return endorsement;
}

bool Buyer::checkKey(const vector<string>& keys) {
	if (!inProgress)
		throw CashException(CashException::CE_FE_ERROR,
			"[Buyer::pay] Pay called on a buyer not working");
	
	if (keys.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[Buyer::pay] No key given");
	
	for (unsigned i = 0; i < ctext.size(); i++) {
		// decrypt the ciphertext using key
		unsigned index = (keys.size() == 1) ? 0 : i;
		Ptr<Buffer> plaintext = ctext[i]->decrypt(keys[index], 
											contract->getEncAlgB());
		ptext.push_back(plaintext);
	}
	
	// compute hashes
	hash_t ptHash = Hash::hash(ptext, contract->getPTHashB().alg, 
							   contract->getPTHashB().key, 
							   contract->getPTHashB().type);
	
	if (ptHash != contract->getPTHashB())
		throw CashException(CashException::CE_FE_ERROR,
				"[Buyer::pay] The decrypted file was not the promised file");
	
	return true;
}

ZZ Buyer::resolve() {
	return r;
}

