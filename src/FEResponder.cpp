
#include "FEResponder.h"
#include "VEVerifier.h"
#include "CashException.h"

/*----------------------------------------------------------------------------*/
// Constructors
FEResponder::FEResponder(const int timeoutLength, const int timeoutTolerance, 
			 			 const VEPublicKey* pk, const VEPublicKey* regularpk, 
			 			 const int stat)
	: timeoutLength(timeoutLength), timeoutTolerance(timeoutTolerance), 
	  stat(stat), verifiablePK(pk), regularPK(regularpk), contract(NULL), 
	  escrow(NULL), initiatorSignPK(NULL), exchangeType(TYPE_NONE),
	  message(NULL)
{
}
	
FEResponder::FEResponder(const FEResponder& o)
	: timeoutLength(o.timeoutLength), timeoutTolerance(o.timeoutTolerance), 
	  stat(o.stat), verifiablePK(o.verifiablePK), regularPK(o.regularPK),
	  ptextB(o.ptextB), ctextB(o.ctextB), ctextA(o.ctextA), ptextA(o.ptextA), 
	  contract(o.contract ? new FEContract(*o.contract) : NULL),
	  escrow(o.escrow ? new VECiphertext(*o.escrow) : NULL),
	  initiatorSignPK(o.initiatorSignPK ? new Signature::Key(*o.initiatorSignPK) : NULL),
	  message(o.message ? new FEMessage(*o.message) : NULL)
{
}

/*----------------------------------------------------------------------------*/
// Destructor
FEResponder::~FEResponder() {
	reset();
	delete escrow;
	delete initiatorSignPK;
}

void FEResponder::reset() {
	exchangeType = TYPE_NONE;
	
	for (unsigned i = 0; i < ptextA.size(); i++) {
		delete ptextA[i];
	}
	for (unsigned i = 0; i < ctextB.size(); i++) {
		delete ctextB[i];
	}
	ctextB.clear();
	ctextA.clear();
	ptextB.clear();
	ptextA.clear();
	
	delete contract;
	delete message;
}

/*----------------------------------------------------------------------------*/
// Setup
bool FEResponder::setup(const FESetupMessage *msg, const ZZ& R) {
	// check escrow
	msg->check(verifiablePK, stat, R);
	
	// set parameters from FESetupMessage
	coinPrime = msg->getCoinPrime();
	escrow = msg->getEscrow();
	initiatorSignPK = new Signature::Key(*msg->getPK());
	
	return true;
}
/*----------------------------------------------------------------------------*/
// Start Round
EncBuffer* FEResponder::startRound(const Buffer* ptextR, 
								   const cipher_t& encAlgR) {
	return startRound(CommonFunctions::vectorize<const Buffer*>(ptextR), 
					  encAlgR)[0];
}

vector<EncBuffer*> FEResponder::startRound(const vector<const Buffer*>& ptextR,
										  const cipher_t& encAlgR) {
	setResponderFiles(ptextR, encrypt(ptextR,encAlgR));
	return ctextB;
}
/*----------------------------------------------------------------------------*/
// Set Responder Files
void FEResponder::setResponderFiles(const Buffer *ptextR, EncBuffer* ctextR) {
	setResponderFiles(CommonFunctions::vectorize<const Buffer*>(ptextR),
					  CommonFunctions::vectorize<EncBuffer*>(ctextR));
}

void FEResponder::setResponderFiles(const vector<const Buffer*>& ptextR,
									const vector<EncBuffer*>& ctextR) {
	// store values
	ptextB = ptextR;
	ctextB = ctextR;
}
/*----------------------------------------------------------------------------*/
// Encryption
vector<EncBuffer*> FEResponder::encrypt(const vector<const Buffer*>& ptextR, 
										const cipher_t& encAlgR) const {
	string key = Ciphertext::generateKey(encAlgR);
	vector<EncBuffer*> ctexts;
	for (unsigned i = 0; i < ptextR.size(); i++) {
		ctexts.push_back(ptextR[i]->encrypt(encAlgR, key));
	}
	return ctexts;
}
/*----------------------------------------------------------------------------*/
// Sell
vector<string> FEResponder::sell(const FEMessage& message, 
								 const hash_t& ptHashR) {
	return sell(message, CommonFunctions::vectorize<hash_t>(ptHashR));
}

vector<string> FEResponder::sell(const FEMessage& message, 
								 const vector<hash_t>& ptHashR) {
	if (TYPE_NONE != exchangeType)
		throw CashException(CashException::CE_FE_ERROR,
		"[FEResponder::sell] Sell called on an already working FEResponder");
	
	// decide we are in buy
	exchangeType = TYPE_BUY;
	
	// check contract and signature
	FEContract contract = message.getContract();
	check(message, saveString(contract), ptHashR);
	return getKeys();
}

bool FEResponder::check(const FEMessage& msg, const string& label, 
						const vector<hash_t>& ptHashRs) {
	// save message
	message = new FEMessage(msg);
	contract = new FEContract(msg.getContract());
	string sig = msg.getSignature();
	
	// check contract
	contract->checkTimeout(timeoutTolerance);
	contract->checkEncAlgB(ctextB[0]->encAlg);
	
	// use same hash parameters as the initiator did in the contract
	const hash_t& pt = contract->getPTHashB();
	const hash_t& ct = contract->getCTHashB();
	hash_t ptHashR = Hash::hash(ptHashRs, pt.alg, pt.key, pt.type);
	hash_t ctHashR = Hash::hash(ctextB, ct.alg, ct.key, ct.type);
	contract->checkBHash(ptHashR, ctHashR);
	
	// check signature
	if (!Signature::verify(*initiatorSignPK, sig, label, regularPK->hashAlg))
		throw CashException(CashException::CE_FE_ERROR,
			"[FEResponder::check] The signature does not verify");
	
	return true;
}

vector<string> FEResponder::getKeys() const {
	// XXX: IS THIS A VALID ASSUMPTION?
	// optimization: if all ciphertexts have the same key, just output one key
	// shortcut: if two ciphertexts have the same key, assume all have 
	// the same key
	vector<string> keys;
	if (ctextB.size() > 1 && ctextB[0]->key == ctextB[1]->key)
		keys.push_back(ctextB[0]->key);
	else {
		for (unsigned i = 0; i < ctextB.size(); i++) {
			keys.push_back(ctextB[i]->key);
		}
	}
	return keys;
}
/*----------------------------------------------------------------------------*/
// Coin
bool FEResponder::endorseCoin(const vector<ZZ>& endorsement) {
	return coinPrime.endorse(endorsement);
}
/*----------------------------------------------------------------------------*/
// Give Key
vector<string> FEResponder::giveKeys(const FEMessage& signedEscrow, 
									 EncBuffer* ctI, const hash_t& ptI,
									 const hash_t& ptR) {
	return giveKeys(signedEscrow, CommonFunctions::vectorize<EncBuffer*>(ctI),
					CommonFunctions::vectorize<hash_t>(ptI),
					CommonFunctions::vectorize<hash_t>(ptR));
}

vector<string> FEResponder::giveKeys(const FEMessage& signedEscrow,
									 const vector<EncBuffer*>& ctextI,
									 const vector<hash_t>& ptHashIs,
									 const vector<hash_t>& ptHashRs) {
	if (TYPE_NONE != exchangeType)
		throw CashException(CashException::CE_FE_ERROR,
			"[FEResponder::giveKeys] GiveKeys called on an already working "
			"FEResponder");
	if (ctextI.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[FEResponder::giveKeys] No initiator ciphertext given");
	if (ptHashIs.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[FEResponder::giveKeys] No initiator plaintext hash given");
	
	// check contract and signature
	string label = CommonFunctions::vecToString(signedEscrow.getEscrow());
	check(signedEscrow, label, ptHashRs);
	
	// check initiator files
	ctextA = ctextI;
	const hash_t& ct = contract->getCTHashA();
	const hash_t& pt = contract->getPTHashA();
	hash_t ctHashI = Hash::hash(ctextA, ct.alg, ct.key, ct.type);
	hash_t ptHashI = Hash::hash(ptHashIs, pt.alg, pt.key, pt.type);
	contract->checkAHash(ptHashI, ctHashI);
	
	// decide we are doing barter
	exchangeType = TYPE_BARTER;
		
	// if all checks pass, output key
	return getKeys();
}
/*----------------------------------------------------------------------------*/
// Decrypt and check
bool FEResponder::checkKey(const string& keyI) {
	return checkKey(CommonFunctions::vectorize<string>(keyI));
}

bool FEResponder::checkKey(const vector<string>& keysI) {
	if (TYPE_BARTER != exchangeType)
		throw CashException(CashException::CE_FE_ERROR, 
							"[FEResponder::checkKey] CheckKey called on an "
							"FEResponder not bartering");
	if (keysI.empty()){
		return false;
	}

	for (unsigned i = 0; i < ctextA.size(); i++) {
		unsigned index = (keysI.size() == 1) ? 0 : i;
		Buffer* ptext = ctextA[i]->decrypt(keysI[index], contract->getEncAlgA());
		ptextA.push_back(ptext);
	}
	const hash_t& pt = contract->getPTHashA();
	hash_t ptHashI = Hash::hash(ptextA, pt.alg, pt.key, pt.type);
	return (ptHashI == pt);
}
/*----------------------------------------------------------------------------*/
// Resolutions

FEResolutionMessage* FEResponder::resolveI(){
	return new FEResolutionMessage(getMessage(), getSetupMessage(), getKeys());
}

MerkleProof* FEResponder::resolveII(vector<unsigned> &challenges){
	vector<EncBuffer*> ctextBlocks;
	//prove the ciphertext blocks
	for(unsigned i = 0; i < challenges.size(); i++){
		ctextBlocks.push_back(ctextB[challenges[i]]);
	}
	const hash_t& ct = contract->getCTHashB();
	//generate the proofs and send them to the arbiter with the blocks
	MerkleContract ctContract(ct.key, ct.alg);
	MerkleProver ctProver(ctextB, ctContract);
	hash_matrix ctProofs = ctProver.generateProofs(challenges);

	const hash_t& pt = contract->getPTHashB();
	if(pt.type == Hash::TYPE_MERKLE){
			MerkleContract ptContract(pt.key, pt.alg);
			MerkleProver ptProver = MerkleProver(ptextB, ptContract);
			hash_matrix ptProofs = ptProver.generateProofs(challenges);
			return new MerkleProof(ctextBlocks, ctProofs, ptProofs, 
								   new MerkleContract(ctContract), 
								   new MerkleContract(ptContract));
	} else {
		return new MerkleProof(ctextBlocks, ctProofs, ptextB[0]->str(), 
							   new MerkleContract(ctContract));
	}
}

MerkleProof* FEResponder::resolveIII(vector<string> &keys){
	if(checkKey(keys)){
		// XXX: not really sure what I should be returning if they do check
		// out as additional communication with the Arbiter is not needed
		return new MerkleProof;
	} else{
		return proveIncorrectKeys(keys);
	}
}

bool FEResponder::resolveIV(vector<ZZ> &endorsement){
	return endorseCoin(endorsement);
}

MerkleProof* FEResponder::proveIncorrectKeys(const vector<string> &keys) {
	// make sure we are in barter
	if (exchangeType != TYPE_BARTER) {
		throw CashException(CashException::CE_FE_ERROR,
		"[FEResponder::proveIncorrectKeys] Trying to run a barter function "
		"in a buy protocol");
	}
	// XXX: should we change this?
	// for now I am just generating a challenge using the first index
	// if the key is wrong, this will be wrong with high probability
	vector<unsigned> challenges;
	challenges.push_back(0);
	vector<EncBuffer*> ctextBlock;
	ctextBlock.push_back(ctextA[0]);
	const hash_t& ct = contract->getCTHashA();
	MerkleContract ctContract(ct.key, ct.alg);
	MerkleProver ctProver(ctextA, ctContract);
	hash_matrix ctProofs = ctProver.generateProofs(challenges);
	const hash_t& pt = contract->getPTHashB();
	if(pt.type == Hash::TYPE_MERKLE){
			MerkleContract ptContract(pt.key, pt.alg);
			MerkleProver ptProver = MerkleProver(ptextB, ptContract);
			hash_matrix ptProofs = ptProver.generateProofs(challenges);
			return new MerkleProof(ctextBlock, ctProofs, ptProofs, 
								   new MerkleContract(ctContract), 
								   new MerkleContract(ptContract));
	} else {
		return new MerkleProof(ctextBlock, ctProofs, ptextB[0]->str(), 
							   new MerkleContract(ctContract));
	}
}

FESetupMessage* FEResponder::getSetupMessage() const {
	return new FESetupMessage(coinPrime, escrow, *initiatorSignPK);
}

