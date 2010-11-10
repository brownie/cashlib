
#include "Seller.h"
#include "CommonFunctions.h"
#include "VEVerifier.h"
#include <assert.h>
#include "Timer.h"

/*----------------------------------------------------------------------------*/
// Constructors
Seller::Seller(const int timeoutLength, const int timeoutTolerance, 
			   const VEPublicKey* pk, const int stat)
	: timeoutLength(timeoutLength), timeoutTolerance(timeoutTolerance), 
	  stat(stat), pk(pk), contract(NULL), escrow(NULL), inProgress(false)
{
}

Seller::Seller(EncBuffer* ct, const int timeoutLength, 
			   const int timeoutTolerance, const VEPublicKey* pk, 
			   const int stat)
	: timeoutLength(timeoutLength), timeoutTolerance(timeoutTolerance), 
	  stat(stat), pk(pk), contract(NULL), escrow(NULL), inProgress(false)
{
	ctext.push_back(ct);
}

Seller::Seller(vector<EncBuffer*> ctext, const int timeoutLength, 
			   const int timeoutTolerance, const VEPublicKey* pk, 
			   const int stat)
	: timeoutLength(timeoutLength), timeoutTolerance(timeoutTolerance), 
	  stat(stat), pk(pk), ctext(ctext), contract(NULL), escrow(NULL), 
	  inProgress(false)
{
}

Seller::Seller(const Seller& o)
	: timeoutLength(o.timeoutLength), timeoutTolerance(o.timeoutTolerance), 
	  stat(o.stat), pk(o.pk), ptext(o.ptext), ctext(o.ctext),
	  coinPrime(o.coinPrime), 
	  contract(o.contract ? new FEContract(*o.contract) : NULL),
	  escrow(o.escrow ? new VECiphertext(*o.escrow) : NULL),
	  inProgress(o.inProgress)
{
}

/*----------------------------------------------------------------------------*/
// Destructor
Seller::~Seller() {
	reset();
}

void Seller::reset() {
	inProgress = false;
	//delete escrow;
#ifdef DELETE_BUFFERS
	for (unsigned i = 0; i < ctext.size(); i++) {
		delete ctext[i];
	}
#endif
	ctext.clear();
	//delete contract; // XXX memory
}

/*----------------------------------------------------------------------------*/
// Encryption
EncBuffer* Seller::encrypt(const Buffer* pt, const cipher_t& encAlg, 
						   const string& key) {
	ptext.push_back(pt);
	// now encrypt each block in fileBlocks using K
	EncBuffer* ct = pt->encrypt(encAlg, key);
	ctext.push_back(ct);
	// return outcome of this encryption
	return ct;
}

vector<EncBuffer*> Seller::encrypt(const vector<const Buffer*>& ptext,
								   const cipher_t& encAlg) {
	string commonKey = Ciphertext::generateKey(encAlg);
	for (unsigned i = 0; i < ptext.size(); i++) {
		encrypt(ptext[i], encAlg, commonKey);
	}
	return ctext;
}

void Seller::setFiles(const Buffer* ptext, EncBuffer* ctext) {
	setFiles(CommonFunctions::vectorize<const Buffer*>(ptext),
			 CommonFunctions::vectorize<EncBuffer*>(ctext));
}

void Seller::setFiles(const vector<const Buffer*>& pt, 
					  const vector<EncBuffer*>& ct) {
	ptext = pt;
	ctext = ct;
}

/*----------------------------------------------------------------------------*/
// Selling
vector<string> Seller::sell(const BuyMessage* buyerInput, const ZZ& R, 
							const hash_t& ptHash){
	return sell(buyerInput, R, CommonFunctions::vectorize<hash_t>(ptHash));
}


vector<string> Seller::sell(const BuyMessage* buyerInput, const ZZ& R, 
							const vector<hash_t>& ptHashes) {
	if (inProgress)
		throw CashException(CashException::CE_FE_ERROR,
			"[Seller::sell] Sell called on an already working seller");
	if (ptext.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[Seller::sell] No plaintext given");
	if (ctext.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[Seller::sell] No ciphertext given");
	
	check(buyerInput, R, ptHashes);
	
	// set inProgress
	inProgress = true;
	
	vector<string> keys;
	for (unsigned i = 0; i < ctext.size(); i++) {
		keys.push_back(ctext[i]->key);
		if (ctext.size() > 1 && ctext[0]->key == ctext[1]->key)
			break;
	}
	return keys;
}

bool Seller::check(const BuyMessage* buyerInput, const ZZ& R,
				   const vector<hash_t>& ptHashes) {
	// check message
	buyerInput->check(pk, stat, R);
	
	// check contract
	contract = buyerInput->getContract();
	contract->checkTimeout(timeoutTolerance);
	contract->checkEncAlgB(ctext[0]->encAlg);
#ifdef DOUBLE_CHECK
	hash_t ptHashReal = Hash::hash(ptext, contract->getPTHashB().alg, 
								   contract->getPTHashB().key, 
								   contract->getPTHashB().type);
	assert(ptHashReal == contract->getPTHashB());
#endif
	hash_t ptHash = Hash::hash(ptHashes, contract->getPTHashB().alg, 
							   contract->getPTHashB().key, 
							   contract->getPTHashB().type);
	hash_t ctHash = Hash::hash(ctext, contract->getCTHashB().alg, 
							   contract->getCTHashB().key, 
							   contract->getCTHashB().type);
	contract->checkBHash(ptHash, ctHash);
	
	// save values
	escrow = buyerInput->getEscrow();
	coinPrime = buyerInput->getCoinPrime();
	return true;
}

/*----------------------------------------------------------------------------*/
// Resolution

//send keys and buy message to the Arbiter so they can begin resolution
pair<vector<string>, BuyMessage*> Seller::resolveI() {
	vector<string> keys(ctext.size());
	for(unsigned i = 0; i < ctext.size(); i++){
		keys[i] = ctext[i]->key;
	}
	return make_pair<vector<string>, BuyMessage*>(keys, getBuyMessage()); 
}

MerkleProof* Seller::resolveII(vector<unsigned> &challenges){
	vector<EncBuffer*> ctextBlocks;
	// send the required ciphertext blocks
	for(unsigned i = 0; i < challenges.size(); i++){
		ctextBlocks.push_back(ctext[challenges[i]]);
	}
	
	//create and send the proofs as well
	const hash_t& ct = contract->getCTHashB();
	MerkleContract* ctContract = new MerkleContract(ct.key, ct.alg);
	MerkleProver ctProver(ctext, *ctContract);
	vector<vector<hashDirect> > ctProofs = ctProver.generateProofs(challenges);

	if(ct.type == Hash::TYPE_MERKLE){
		const hash_t& pt = contract->getPTHashB();
		MerkleContract* ptContract = new MerkleContract(pt.key, pt.alg);
		MerkleProver ptProver(ptext, *ptContract);
		vector<vector<hashDirect> > ptProofs = ptProver.generateProofs(challenges);
		return new MerkleProof(ctextBlocks, ctProofs, ptProofs, 
							   ctContract, ptContract);
	} else {
		return new MerkleProof(ctextBlocks, ctProofs, ptext[0]->str(), ctContract);
	}
}

BuyMessage* Seller::getBuyMessage() const {
	return new BuyMessage(coinPrime, contract, escrow);
}

bool Seller::endorseCoin(const vector<ZZ> &endorsement) {
	return coinPrime.endorse(endorsement);
}
