#include "Arbiter.h"
#include "VECiphertext.h"

vector<string> Arbiter::initiatorResolve(const ZZ &r){
	return buyerResolve(r);
}

vector<string> Arbiter::buyerResolve(const ZZ &r){
	vector<ZZ> rVec;
	rVec.push_back(r);
	// check to see if seller has resolved yet; if not keys won't be there
	if(getDB){
		return getDB(Hash::hash(rVec, hashAlg));
	} else {
		return vector<string>();
	}
}

vector<unsigned> Arbiter::sellerResolveI(const ResolutionPair &keyMessagePair){
	// first, store the keys
	keys = keyMessagePair.first;
	// now, unwrap and check the buyMessage, then store everything
	Ptr<BuyMessage> buyMessage = keyMessagePair.second;
	Coin coinPrime = buyMessage->getCoinPrime();
	Ptr<VECiphertext> escrow = buyMessage->getEscrow();
	// want to store the contract as well (for stage II)
	contract = buyMessage->getContract();
	// check the timeout to make sure it hasn't passed
	if(contract.checkTimeout(timeoutTolerance)) {
		endorsement = verifiableDecrypter->decrypt(escrow.getCiphertext(), 
												   saveString(contract), hashAlg);
		// make sure the endorsement on the coin is valid
		if(coinPrime.verifyEndorsement(endorsement)){
			// construct verifiers based on the data in the contract and 
			// return a set of challenges
			hash_t ptHash = contract.getPTHashB();
			hash_t ctHash = contract.getCTHashB();
			ptVerifier = new_ptr<MerkleVerifier>(
                ptHash, 
                contract.getNumPTHashBlocksB(), 
                MerkleContract(ptHash.key,ptHash.alg));
			ctVerifier = new_ptr<MerkleVerifier>(
                ctHash, 
                contract.getNumCTHashBlocksB(), 
                MerkleContract(ctHash.key,ctHash.alg));
			return ptVerifier->getChallenges();
		} else {
			throw CashException(CashException::CE_FE_ERROR,
				"[Arbiter::sellerResolveI] invalid endorsement");
		}
	} else {
		throw CashException(CashException::CE_FE_ERROR, 
			"[Arbiter::sellerResolveI] contract has expired");
	}
}

vector<ZZ> Arbiter::sellerResolveII(Ptr<const MerkleProof> proof){
	// check the proof against the keys provided in Stage I
	if(verifyKeys(proof)){
		if(updateDB){
			vector<ZZ> rVec;
			rVec.push_back(contract.getID());
			// store the keys (for the buyer later)
			updateDB(Hash::hash(rVec,hashAlg), keys); 
		}
		// return the endorsement to the seller
		return endorsement;
	} else {
		throw CashException(CashException::CE_FE_ERROR, 
					"[Arbiter::sellerResolveII] Seller proof is not valid");
	}
}

bool Arbiter::verifyKeys(Ptr<const MerkleProof> proof) {
	// the arbiter needs to check the encrypted blocks decrypt correctly
	bool validDecryption = true;
	vector<Ptr<EncBuffer> > cTextBlocks = proof->getCTextBlocks();
	vector<Ptr<Buffer> > decryptedBlocks(cTextBlocks.size());
	vector<hash_t> hashedBlocks(cTextBlocks.size());
	unsigned i = 0;
	// it checks the decryption in the following three steps:
	do {
		// 1. decrypt the blocks
		unsigned index = (keys.size() == 1) ? 0 : i;
		decryptedBlocks[i] = cTextBlocks[i]->decrypt(keys[index], 
													 contract.getEncAlgB());
		// 2. hash them
		hashedBlocks[i] = proof->getPTContract()->hash(decryptedBlocks[i]);
		// 3. check if they match the public plaintext hashes
		validDecryption = hashedBlocks[i] == proof->getPTextProof()[i][0].node;
		i++;
	} while(validDecryption && i < cTextBlocks.size());

	// now finish verifying using the MerkleVerifiers
	if(contract.getPTHashB().type == Hash::TYPE_MERKLE){
		return (validDecryption && 
				ctVerifier->verifyProofs(proof->getCTextProof()) && 
				ptVerifier->verifyProofs(proof->getPTextProof()));
	} else {
		hash_t ptHash = proof->getPTContract()->hash(proof->getPlaintext());
		return (validDecryption && 
				ctVerifier->verifyProofs(proof->getCTextProof()) && 
				(ptHash == message->getContract().getPTHashB()));
	}
}

bool Arbiter::verifyDecryption(Ptr<const MerkleProof> proof){
	bool validDecryption = true;
	vector<Ptr<EncBuffer> > cTextBlocks = proof->getCTextBlocks();
	vector<Ptr<Buffer> > decryptedBlocks(cTextBlocks.size());
	vector<hash_t> hashedBlocks(cTextBlocks.size());
	unsigned i = 0;
	do{
		unsigned index = (keys.size() == 1) ? 0 : i;
		decryptedBlocks[i] = cTextBlocks[i]->decrypt(keys[index], 
													 contract.getEncAlgA());
		hashedBlocks[i] = proof->getCTContract()->hash(decryptedBlocks[i]);
		validDecryption = hashedBlocks[i] == proof->getPTextProof()[i][0].node;
		i++;
	} while(validDecryption && i < cTextBlocks.size());

	hash_matrix ctProof = proof->getCTextProof();
	return (validDecryption && ctVerifier->verifyProofs(ctProof));
}

vector<unsigned> Arbiter::responderResolveI(Ptr<const FEResolutionMessage> req) {
	return responderResolveI(req->getKeys(), req->getMessage(),
							 req->getSetupMessage());
}

// this method sets up the resolve for a failed barter
vector<unsigned> Arbiter::responderResolveI(const vector<string> &ks,		
											Ptr<const FEMessage> msg, 
											Ptr<const FESetupMessage> setup) {
	// stores keys and message
	keys = ks;
	message = msg;
	Coin coinPrime = setup->getCoinPrime();
	// this is the regular encryption
	vector<ZZ> sigEscrow = message->getEscrow();
	// this is the verifiable encryption
	VECiphertext vEscrow = setup->getEscrow();
	contract = message->getContract();
	Ptr<const Signature::Key> sigPK = setup->getPK();
	
	// verify that the signature given in BarterMessage is correct
	bool sigCorrect = Signature::verify(sigPK, message->getSignature(), 
										CommonFunctions::vecToString(sigEscrow), 
										hashAlg);	
	if (!sigCorrect){
		throw CashException(CashException::CE_FE_ERROR,
				"[Arbiter::responderResolveI] signature was malformed"); 
	}

	// also need to make sure the contract hasn't expired
	if (!contract.checkTimeout(timeoutTolerance)){
		throw CashException(CashException::CE_FE_ERROR, 
			"[Arbiter::responderResolveI] contract has expired");
	}

	vector<ZZ> end = verifiableDecrypter->decrypt(vEscrow.getCiphertext(),
												  sigPK->publicKeyString(), 
												  hashAlg);
	// now verify the endorsement (and store it if it's valid)
	if(coinPrime.verifyEndorsement(end)){
		// first store endorsement
		endorsement = end;
														
		// set up merkle verifiers and return challenges
		hash_t ptHash = contract.getPTHashB();
		hash_t ctHash = contract.getCTHashB();
		ptVerifier = new_ptr<MerkleVerifier>(
            ptHash, 
            contract.getNumPTHashBlocksB(), 
            MerkleContract(ptHash.key,ptHash.alg));
		ctVerifier = new_ptr<MerkleVerifier>(
            ctHash, 
            contract.getNumCTHashBlocksB(), 
            MerkleContract(ctHash.key,ctHash.alg));
		// XXX: right now this is only returning 0 every time!!
		return ptVerifier->getChallenges();
	} else {
		throw CashException(CashException::CE_FE_ERROR,
			"[Arbiter::responderResolveI] invalid endorsement");
	}
}
	
vector<string> Arbiter::responderResolveII(Ptr<const MerkleProof> proof){
	if(verifyKeys(proof)){
		// decrypt the signature escrow
		vector<ZZ> m = message->getEscrow();
		string label = saveString(contract);
		vector<ZZ> initiatorVals = regularDecrypter->decrypt(m, label, hashAlg); 
		vector<string> initiatorKeys(initiatorVals.size());
		for(unsigned i = 0; i < initiatorVals.size(); i++){
			initiatorKeys[i] = ZZToBytes(initiatorVals[i]);
		}
		// store keys (in case we go to Stage III)
		keys = initiatorKeys;
		return initiatorKeys;
	} else {
		throw CashException(CashException::CE_FE_ERROR, 
			"[Arbiter::responderResolveII] responder's proof did not verify");
	}
}

vector<ZZ> Arbiter::responderResolveIII(Ptr<const MerkleProof> proof){
	if (!verifyDecryption(proof)){
		return endorsement;
	} else {
		throw CashException(CashException::CE_FE_ERROR,
			"[Arbiter::responderFinalResolve] the key was correct");
	}
}
