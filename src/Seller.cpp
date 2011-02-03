
#include "Seller.h"
#include "CommonFunctions.h"
#include "VEVerifier.h"
#include <assert.h>
#include "Timer.h"

/*----------------------------------------------------------------------------*/
// Constructors
Seller::Seller(const int timeoutLength, const int timeoutTolerance, 
			   Ptr<const VEPublicKey> pk, const int stat)
	: timeoutLength(timeoutLength), timeoutTolerance(timeoutTolerance), 
	  stat(stat), pk(pk), contract(), escrow(), inProgress(false)
{
}

Seller::Seller(Ptr<EncBuffer> ct, const int timeoutLength, 
			   const int timeoutTolerance, Ptr<const VEPublicKey> pk, 
			   const int stat)
	: timeoutLength(timeoutLength), timeoutTolerance(timeoutTolerance), 
	  stat(stat), pk(pk), contract(), escrow(), inProgress(false)
{
	ctext.push_back(ct);
}

Seller::Seller(vector<Ptr<EncBuffer> > ctext, const int timeoutLength, 
			   const int timeoutTolerance, Ptr<const VEPublicKey> pk, 
			   const int stat)
	: timeoutLength(timeoutLength), timeoutTolerance(timeoutTolerance), 
	  stat(stat), pk(pk), ctext(ctext), contract(), escrow(), 
	  inProgress(false)
{
}

/*----------------------------------------------------------------------------*/
// Destructor
Seller::~Seller() {
//	reset();
    cout << "Seller::~Seller() going away" << endl;
}

void Seller::reset() {
	inProgress = false;
	ctext.clear();
	escrow.reset();
	contract.reset();
}

/*----------------------------------------------------------------------------*/
// Encryption
Ptr<EncBuffer> Seller::encrypt(Ptr<const Buffer> pt, const cipher_t& encAlg, 
						   const string& key) {
	ptext.push_back(pt);
	// now encrypt each block in fileBlocks using K
	Ptr<EncBuffer> ct = pt->encrypt(encAlg, key);
	ctext.push_back(ct);
	// return outcome of this encryption
	return ct;
}

vector<Ptr<EncBuffer> > Seller::encrypt(const vector<Ptr<const Buffer> >& ptext,
								   const cipher_t& encAlg) {
	string commonKey = Ciphertext::generateKey(encAlg);
	for (unsigned i = 0; i < ptext.size(); i++) {
		encrypt(ptext[i], encAlg, commonKey);
	}
	return ctext;
}

void Seller::setFiles(Ptr<const Buffer> ptext, Ptr<EncBuffer> ctext) {
	setFiles(CommonFunctions::vectorize<Ptr<const Buffer> >(ptext),
			 CommonFunctions::vectorize<Ptr<EncBuffer> >(ctext));
}

void Seller::setFiles(const vector<Ptr<const Buffer> >& pt, 
					  const vector<Ptr<EncBuffer> >& ct) {
	ptext = pt;
	ctext = ct;
}

/*----------------------------------------------------------------------------*/
// Selling
vector<string> Seller::sell(Ptr<const BuyMessage> buyerInput, const ZZ& R, 
							const hash_t& ptHash){
	return sell(buyerInput, R, CommonFunctions::vectorize<hash_t>(ptHash));
}


vector<string> Seller::sell(Ptr<const BuyMessage> buyerInput, const ZZ& R, 
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

bool Seller::check(Ptr<const BuyMessage> buyerInput, const ZZ& R,
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
pair<vector<string>, Ptr<BuyMessage> > Seller::resolveI() {
	vector<string> keys(ctext.size());
	for(unsigned i = 0; i < ctext.size(); i++){
		keys[i] = ctext[i]->key;
	}
	return make_pair<vector<string>, Ptr<BuyMessage> >(keys, getBuyMessage()); 
}

Ptr<MerkleProof> Seller::resolveII(vector<unsigned> &challenges){
	vector<Ptr<EncBuffer> > ctextBlocks;
	// send the required ciphertext blocks
	for(unsigned i = 0; i < challenges.size(); i++){
		ctextBlocks.push_back(ctext[challenges[i]]);
	}
	
	//create and send the proofs as well
	const hash_t& ct = contract->getCTHashB();
	Ptr<MerkleContract> ctContract = new_ptr<MerkleContract>(ct.key, ct.alg);
	MerkleProver ctProver(ctext, *ctContract);
	vector<vector<hashDirect> > ctProofs = ctProver.generateProofs(challenges);

	if(ct.type == Hash::TYPE_MERKLE){
		const hash_t& pt = contract->getPTHashB();
		Ptr<MerkleContract> ptContract = new_ptr<MerkleContract>(pt.key, pt.alg);
		MerkleProver ptProver(ptext, *ptContract);
		vector<vector<hashDirect> > ptProofs = ptProver.generateProofs(challenges);
		return new_ptr<MerkleProof>(ctextBlocks, ctProofs, ptProofs, 
							   ctContract, ptContract);
	} else {
		return new_ptr<MerkleProof>(ctextBlocks, ctProofs, ptext[0]->str(), ctContract);
	}
}

Ptr<BuyMessage> Seller::getBuyMessage() const {
	return new_ptr<BuyMessage>(coinPrime, contract, escrow);
}

bool Seller::endorseCoin(const vector<ZZ> &endorsement) {
	return coinPrime->endorse(endorsement);
}
