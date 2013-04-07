
#include "FEResponder.h"
#include "VEVerifier.h"
#include "CashException.h"

/*----------------------------------------------------------------------------*/
// Constructors
FEResponder::FEResponder(const int timeoutLength, const int timeoutTolerance, 
			 			 Ptr<const VEPublicKey> pk, Ptr<const VEPublicKey> regularpk, 
			 			 const int stat)
	: timeoutLength(timeoutLength), timeoutTolerance(timeoutTolerance), 
	  stat(stat), verifiablePK(pk), regularPK(regularpk), contract(), 
	  escrow(), initiatorSignPK(), exchangeType(TYPE_NONE),
	  message()
{
}
	
/*----------------------------------------------------------------------------*/
// Destructor
FEResponder::~FEResponder() {
	reset();
}

void FEResponder::reset() {
	exchangeType = TYPE_NONE;

	ctextB.clear();
	ctextA.clear();
	ptextB.clear();
	ptextA.clear();
	
	contract.reset();
	escrow.reset();
	message.reset();
}

/*----------------------------------------------------------------------------*/
// Setup
bool FEResponder::setup(Ptr<const FESetupMessage> msg, const ZZ& R) {
	// check escrow
	msg->check(verifiablePK, stat, R);
	
	// set parameters from FESetupMessage
	coinPrime = msg->getCoinPrime();
	escrow = msg->getEscrow();
	initiatorSignPK = new_ptr<Signature::Key>(*msg->getPK());
	
	return true;
}
/*----------------------------------------------------------------------------*/
// Start Round
Ptr<EncBuffer> FEResponder::startRound(Ptr<const Buffer> ptextR, 
								   const cipher_t& encAlgR) {
	return startRound(CommonFunctions::vectorize<Ptr<const Buffer> >(ptextR), 
					  encAlgR)[0];
}

vector<Ptr<EncBuffer> > FEResponder::startRound(const vector<Ptr<const Buffer> >& ptextR,
										  const cipher_t& encAlgR) {
	setResponderFiles(ptextR, encrypt(ptextR,encAlgR));
	return ctextB;
}
/*----------------------------------------------------------------------------*/
// Set Responder Files
void FEResponder::setResponderFiles(Ptr<const Buffer> ptextR, Ptr<EncBuffer> ctextR) {
	setResponderFiles(CommonFunctions::vectorize<Ptr<const Buffer> >(ptextR),
					  CommonFunctions::vectorize<Ptr<EncBuffer> >(ctextR));
}

void FEResponder::setResponderFiles(const vector<Ptr<const Buffer> >& ptextR,
									const vector<Ptr<EncBuffer> >& ctextR) {
	// store values
	ptextB = ptextR;
	ctextB = ctextR;
}
/*----------------------------------------------------------------------------*/
// Encryption
vector<Ptr<EncBuffer> > FEResponder::encrypt(const vector<Ptr<const Buffer> >& ptextR, 
										const cipher_t& encAlgR) const {
	string key = Ciphertext::generateKey(encAlgR);
	vector<Ptr<EncBuffer> > ctexts;
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
	Ptr<FEContract> contract = message.getContract();
	check(message, saveString(*contract), ptHashR);
	return getKeys();
}

bool FEResponder::check(const FEMessage& msg, const string& label, 
						const vector<hash_t>& ptHashRs) {
	// save message
	message = new_ptr<FEMessage>(msg);
	contract = msg.getContract();
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
									 Ptr<EncBuffer> ctI, const hash_t& ptI,
									 const hash_t& ptR) {
	return giveKeys(signedEscrow, CommonFunctions::vectorize<Ptr<EncBuffer> >(ctI),
					CommonFunctions::vectorize<hash_t>(ptI),
					CommonFunctions::vectorize<hash_t>(ptR));
}

vector<string> FEResponder::giveKeys(const FEMessage& signedEscrow,
									 const vector<Ptr<EncBuffer> >& ctextI,
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
		Ptr<Buffer> ptext = ctextA[i]->decrypt(keysI[index], contract->getEncAlgA());
		ptextA.push_back(ptext);
	}
	const hash_t& pt = contract->getPTHashA();
	hash_t ptHashI = Hash::hash(ptextA, pt.alg, pt.key, pt.type);
	return (ptHashI == pt);
}
/*----------------------------------------------------------------------------*/
// Resolutions

Ptr<FEResolutionMessage> FEResponder::resolveI(){
	return new_ptr<FEResolutionMessage>(getMessage(), getSetupMessage(), getKeys());
}

Ptr<MerkleProof> FEResponder::resolveII(vector<unsigned> &challenges){
	vector<Ptr<EncBuffer> > ctextBlocks;
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
			return new_ptr<MerkleProof>(ctextBlocks, ctProofs, ptProofs, 
								   new_ptr<MerkleContract>(ctContract), 
								   new_ptr<MerkleContract>(ptContract));
	} else {
		return new_ptr<MerkleProof>(ctextBlocks, ctProofs, ptextB[0]->str(), 
							   new_ptr<MerkleContract>(ctContract));
	}
}

Ptr<MerkleProof> FEResponder::resolveIII(vector<string> &keys){
	if(checkKey(keys)){
		// XXX: not really sure what I should be returning if they do check
		// out as additional communication with the Arbiter is not needed
		return new_ptr<MerkleProof>();
	} else{
		return proveIncorrectKeys(keys);
	}
}

bool FEResponder::resolveIV(vector<ZZ> &endorsement){
	return endorseCoin(endorsement);
}

Ptr<MerkleProof> FEResponder::proveIncorrectKeys(const vector<string> &keys) {
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
	vector<Ptr<EncBuffer> > ctextBlock;
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
			return new_ptr<MerkleProof>(ctextBlock, ctProofs, ptProofs, 
								   new_ptr<MerkleContract>(ctContract), 
								   new_ptr<MerkleContract>(ptContract));
	} else {
		return new_ptr<MerkleProof>(ctextBlock, ctProofs, ptextB[0]->str(), 
							   new_ptr<MerkleContract>(ctContract));
	}
}

Ptr<FESetupMessage> FEResponder::getSetupMessage() const {
	return new_ptr<FESetupMessage>(coinPrime, escrow, *initiatorSignPK);
}

