
#include "FEInitiator.h"
#include "VEProver.h"
#include "Timer.h"

/*----------------------------------------------------------------------------*/
// Constructors
FEInitiator::FEInitiator(const long timeoutLength, Ptr<const VEPublicKey> pk, 
			 			 Ptr<const VEPublicKey> regularpk, const int stat, 
						 Ptr<Signature::Key> sk)
	: timeoutLength(timeoutLength), stat(stat), verifiablePK(pk), 
	  regularPK(regularpk), contract(), signKey(sk), 
	  exchangeType(TYPE_NONE)
{}

FEInitiator::FEInitiator(const FEInitiator &o)
	: timeoutLength(o.timeoutLength), stat(o.stat), 
	  verifiablePK(o.verifiablePK), regularPK(o.regularPK), coin(o.coin), 
	  contract(o.contract), signKey(new Signature::Key(*o.signKey)), 
	  ptextA(o.ptextA), ctextA(o.ctextA), ctextB(o.ctextB), ptextB(o.ptextB),
	  r(o.r), endorsement(o.endorsement), exchangeType(o.exchangeType)
{
}

/*----------------------------------------------------------------------------*/
// Destructor
FEInitiator::~FEInitiator() {
	reset();
}

void FEInitiator::reset() {
	exchangeType = TYPE_NONE;
	ctextA.clear();
	ctextB.clear();
	ptextA.clear();
	ptextB.clear();
	contract.reset();
}

/*----------------------------------------------------------------------------*/
// Setup
Ptr<FESetupMessage> FEInitiator::setup(Ptr<Wallet> wallet, const ZZ &R, 
								   const string &signAlg) {
	makeCoin(wallet, R);
	return setup(signAlg);
}

Ptr<FESetupMessage> FEInitiator::setup(const string &signAlg) {
#ifdef TIMER
startTimer();
#endif
	// generate signature key
    if (signKey.get() == NULL)
		signKey = Signature::Key::generateKey(signAlg);
#ifdef TIMER
printTimer("Signature key generation");
#endif
	
#ifdef TIMER
startTimer();
#endif
	// set up prover for forming escrow
	VEProver prover(verifiablePK);
	
	ZZ eCom = coin.getEndorsementCom();
	// the label needs to be the public key for the signature scheme
	// create the verifiable escrow
	Ptr<VECiphertext> escrow = new_ptr<VECiphertext>(prover.verifiableEncrypt(eCom, 
											endorsement, coin.getCashGroup(), 
											signKey->publicKeyString(), 
											verifiablePK->hashAlg, stat));
#ifdef TIMER
printTimer("Verifiable escrow generation");
#endif
	return new_ptr<FESetupMessage>(coin, escrow, *signKey);
}

void FEInitiator::makeCoin(Ptr<Wallet> wallet, const ZZ& R) {
	// get a coin
#ifdef TIMER
startTimer();
#endif
    Ptr<Coin> coin = wallet->nextCoin(R);
#ifdef TIMER
printTimer("Coin generation");
#endif
	setCoin(*coin);
}

void FEInitiator::setCoin(const Coin& c) {	
	// separate coin' from endorsement
	coin = c;
	endorsement = coin.getEndorsement();
	coin.unendorse();
}

/*----------------------------------------------------------------------------*/
// Buy
Ptr<FEMessage> FEInitiator::buy(Ptr<EncBuffer> ctextR, const hash_t& ptHashR) {
	return buy(CommonFunctions::vectorize<Ptr<EncBuffer> >(ctextR), 
			   CommonFunctions::vectorize<hash_t>(ptHashR));
}

Ptr<FEMessage> FEInitiator::buy(const vector</*const*/ Ptr<EncBuffer> >& ctextR,
							const vector</*const*/ hash_t>& ptHashR) {
	if (TYPE_NONE != exchangeType)
		throw CashException(CashException::CE_FE_ERROR,
			"[FEInitiator::buy] Buy called on an already working FEInitiator");
	
	if (ctextR.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[FEInitiator::buy] No ciphertext given");
	
	if (ptHashR.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[FEInitiator::buy] No plaintext hash given");
	
	// now decide that we are running a buy protocol
	exchangeType = TYPE_BUY;
	
	// store ciphertexts
	ctextB = ctextR;
	
	// compute hashes
	hash_t ptHashMerkle = Hash::hash(ptHashR, verifiablePK->hashAlg, 
									 verifiablePK->hashKey, 
									 Hash::TYPE_MERKLE);
	hash_t ctHashMerkle = Hash::hash(ctextB, verifiablePK->hashAlg, 
									 verifiablePK->hashKey, 
									 Hash::TYPE_MERKLE);
	
	// create contract
	if (NULL == contract.get())
		createContract();
	
	// set up the contract
	contract->setPTHashB(ptHashMerkle);
	contract->setCTHashB(ctHashMerkle);
	contract->setEncAlgB(ctextB[0]->encAlg);
	contract->setPTHashBlocksB(ptHashR.size());
	contract->setCTHashBlocksB(ctextR.size());
	
	string signature = signContract();
	return new_ptr<FEMessage>(signature, contract);
}

void FEInitiator::createContract() {
	// set up session id
	r = RandomBits_ZZ(2*stat);
	ZZ id = Hash::hash(r, verifiablePK->hashAlg, verifiablePK->hashKey);
	
	long timeout = time(NULL) + timeoutLength;
	contract = new_ptr<FEContract>(timeout, id);
}

string FEInitiator::signContract() const {
	string contractStr = saveString(*contract);
	return Signature::sign(*signKey, contractStr, verifiablePK->hashAlg);
}

/*----------------------------------------------------------------------------*/
// Pay
vector<ZZ> FEInitiator::pay(const string& key) {
	return pay(CommonFunctions::vectorize<string>(key));
}

vector<ZZ> FEInitiator::pay(const vector<string>& keys) {
	if (TYPE_BUY != exchangeType)
		throw CashException(CashException::CE_FE_ERROR,
			"[FEInitiator::pay] Pay called on an FEInitiator not buying");
	
	if (keys.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[FEInitiator::pay] No key given");
	
	decryptCheck(keys);
	return endorsement;
}

bool FEInitiator::decryptCheck(const vector<string>& keysR) {
	for (unsigned i = 0; i < ctextB.size(); i++) {
		// decrypt the ciphertext using key
		unsigned index = (keysR.size() == 1) ? 0 : i;
		Ptr<Buffer> ptext = ctextB[i]->decrypt(keysR[index], contract->getEncAlgB());
		ptextB.push_back(ptext);
	}
	
	// compute hashes
	const hash_t& pt = contract->getPTHashB();
	hash_t ptHash = Hash::hash(ptextB, pt.alg, pt.key, pt.type);
	
	if (ptHash != pt)
		throw CashException(CashException::CE_FE_ERROR,
							"[FEInitiator::decryptCheck] The decrypted file was "
							"not the promised file");
	return true;
}

/*----------------------------------------------------------------------------*/
// Continue Round
Ptr<EncBuffer> FEInitiator::continueRound(Ptr<const Buffer> ptextI, 
									  const cipher_t& encAlgI) {
	return continueRound(CommonFunctions::vectorize<Ptr<const Buffer> >(ptextI), 
						 encAlgI)[0];
}

vector<Ptr<EncBuffer> > FEInitiator::continueRound(const vector<Ptr<const Buffer> >& ptextI,
											  const cipher_t& encAlgI) {
	if (TYPE_NONE != exchangeType)
		throw CashException(CashException::CE_FE_ERROR,
							"[FEInitiator::continueRound] Barter called on an "
							"already working FEInitiator");
	
	setInitiatorFiles(ptextI, encrypt(ptextI,encAlgI));
	
	// decide we are running barter
	exchangeType = TYPE_BARTER;
	return ctextA;
}

/*----------------------------------------------------------------------------*/
// Encrypt

vector<Ptr<EncBuffer> > FEInitiator::encrypt(const vector<Ptr<const Buffer> >& ptextI, 
										const cipher_t& encAlgI) const {
	if (ptextI.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[FEInitiator::encrypt] No initiator plaintext given");
	
	string key = Ciphertext::generateKey(encAlgI);
	vector<Ptr<EncBuffer> > ctexts;
	for (unsigned i = 0; i < ptextI.size(); i++) {
		ctexts.push_back(ptextI[i]->encrypt(encAlgI, key));
	}
	return ctexts;
}

void FEInitiator::setInitiatorFiles(Ptr<const Buffer> ptextI, Ptr<EncBuffer> ctextI) {
	setInitiatorFiles(CommonFunctions::vectorize<Ptr<const Buffer> >(ptextI),
					  CommonFunctions::vectorize<Ptr<EncBuffer> >(ctextI));
}

void FEInitiator::setInitiatorFiles(const vector<Ptr<const Buffer> >& ptextI,
									const vector<Ptr<EncBuffer> >& ctextI) {
	// store values
	ptextA = ptextI;
	ctextA = ctextI;
}

/*----------------------------------------------------------------------------*/
// Barter
Ptr<FEMessage> FEInitiator::barter(Ptr<EncBuffer> ctextR, const hash_t& ptHashR, 
							   const hash_t& ptHashI) {
	return barter(CommonFunctions::vectorize<Ptr<EncBuffer> >(ctextR),
				  CommonFunctions::vectorize<hash_t>(ptHashR),
				  CommonFunctions::vectorize<hash_t>(ptHashI));
}

Ptr<FEMessage> FEInitiator::barter(const vector<Ptr<EncBuffer> >& ctextR, 
							   const vector<hash_t>& ptHashR,
							   const vector<hash_t>& ptHashI) {
	if (ctextR.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[FEInitiator::barter] No responder ciphertext given");
	
	if (ptHashR.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[FEInitiator::barter] No initiator plaintext hash given");
	
	ctextB = ctextR;
	
	// create contract
	createContract();
	
	// compute hashes
	hash_t ptHashMerkleI = Hash::hash(ptHashI, verifiablePK->hashAlg, 
									  verifiablePK->hashKey, Hash::TYPE_MERKLE);
	hash_t ptHashMerkleR = Hash::hash(ptHashR, verifiablePK->hashAlg, 
									  verifiablePK->hashKey, Hash::TYPE_MERKLE);
	hash_t ctHashMerkleI = Hash::hash(ctextA, verifiablePK->hashAlg,									  				 verifiablePK->hashKey, Hash::TYPE_MERKLE);
	hash_t ctHashMerkleR = Hash::hash(ctextB, verifiablePK->hashAlg,													 verifiablePK->hashKey, Hash::TYPE_MERKLE);
	
	// set the contract
	contract->setPTHashA(ptHashMerkleI);
	contract->setCTHashA(ctHashMerkleI);
	contract->setPTHashB(ptHashMerkleR);
	contract->setCTHashB(ctHashMerkleR);
	contract->setEncAlgA(ctextA[0]->encAlg);
	contract->setEncAlgB(ctextB[0]->encAlg);
	contract->setPTHashBlocksB(ptHashR.size());
	contract->setCTHashBlocksB(ctextR.size());
	
	// optimization: if all ciphertexts have the same key, just output one key
	// shortcut: if two ciphertexts have the same key, assume all have 
	// the same key
	vector<ZZ> keys;
	if (ctextA.size() > 1 && ctextA[0]->key == ctextA[1]->key)
		keys.push_back(ZZFromBytes(ctextA[0]->key));
	else {
		for (unsigned i = 0; i < ctextA.size(); i++) {
			keys.push_back(ZZFromBytes(ctextA[i]->key));
		}
	}
	
	// now set up signature and escrow
	VEProver prover(regularPK);
	// label is the multicontract
	string label = saveString(*contract);
	vector<ZZ> escrow = prover.encrypt(keys, label, regularPK->hashAlg, stat);
	
	// need to sign on the escrow using our signature key
	string escrowStr = CommonFunctions::vecToString(escrow);
	/* TODO: When we use RSA enc as escrow, we should also sign the contract */
	string sig = Signature::sign(*signKey, escrowStr, regularPK->hashAlg);

	// now output the escrow, signature, and contract (label)
	return new_ptr<FEMessage>(escrow, sig, contract);
}

/*----------------------------------------------------------------------------*/
// Give Key
vector<string> FEInitiator::giveKeys(const string& keyR) {
	return giveKeys(CommonFunctions::vectorize<string>(keyR));
}

vector<string> FEInitiator::giveKeys(const vector<string>& keysR) {
	// first check that we really are in barter
	if (TYPE_BARTER != exchangeType)
		throw CashException(CashException::CE_FE_ERROR, 
							"[FEInitiator::giveKeys] GiveKeys called on an "
							"FEInitiator not bartering");
	
	if (keysR.empty())
		throw CashException(CashException::CE_FE_ERROR,
			"[FEInitiator::giveKeys] No key given");
	
	decryptCheck(keysR);

	vector<string> keys;
	if (ctextA.size() > 1 && ctextA[0]->key == ctextA[1]->key)
		keys.push_back(ctextA[0]->key);
	else {
		for (unsigned i = 0; i < ctextA.size(); i++) {
			keys.push_back(ctextA[i]->key);
		}
	}
	return keys;
}

ZZ FEInitiator::resolve() {
	return r;
}

