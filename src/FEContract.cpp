#include "FEContract.h"
#include "CashException.h"
#include "CommonFunctions.h"

bool FEContract::checkTimeout(int timeoutTolerance) const {
	int timeNow = time(NULL);
	// check timeout
	if (timeNow - timeoutTolerance > timeout)
			throw CashException(CashException::CE_FE_ERROR,
			"[FEContract::checkTimeout] Malformed contract (timeout is not "
			"tolerable)");
	return true;
}

bool FEContract::checkEncAlgB(const cipher_t& encAlgR) const {
	if(encAlgB != encAlgR)
		throw CashException(CashException::CE_FE_ERROR,
							"[FEContract::checkEncAlgB] Malformed contract (responder encryption algorithm %s does not match %s)", encAlgR.c_str(), encAlgB.c_str());
	return true;
}


bool FEContract::checkAFiles(const vector<Ptr<const Buffer> >& ptext, 
							 const vector</*const*/ Ptr<EncBuffer> >& ctext) const {
	return checkHashes(ptext, ctext, ptHashA, ctHashA);
}

bool FEContract::checkBFiles(const vector<Ptr<const Buffer> >& ptext, 
							 const vector</*const*/ Ptr<EncBuffer> >& ctext) const {
	return checkHashes(ptext, ctext, ptHashB, ctHashB);
}

bool FEContract::checkAFile(Ptr<const Buffer> ptext, 
							/*const*/ Ptr<EncBuffer> ctext) const {
	return checkAFiles(CommonFunctions::vectorize<Ptr<const Buffer> >(ptext), 
					   CommonFunctions::vectorize<Ptr<EncBuffer> >(ctext));
}

bool FEContract::checkBFile(Ptr<const Buffer> ptext, 
							/*const*/ Ptr<EncBuffer> ctext) const {
	return checkBFiles(CommonFunctions::vectorize<Ptr<const Buffer> >(ptext), 
					   CommonFunctions::vectorize<Ptr<EncBuffer> >(ctext));
}

bool FEContract::checkHashes(const vector<Ptr<const Buffer> >& ptext, 
							 const vector</*const*/ Ptr<EncBuffer> >& ctext, 
							 const hash_t& ptHash, const hash_t& ctHash) const {
	if (ptext.size() != ctext.size())
		throw CashException(CashException::CE_FE_ERROR,
			"[FEContract::checkHashes] Malformed contract (number of files and "
			"ciphertexts mismatch)");
	
	hash_t hash = Hash::hash(ptext, ptHash.alg, ptHash.key, ptHash.type);
	if (ptHash != hash)
		throw CashException(CashException::CE_FE_ERROR,
			"[FEContract::checkHashes] Malformed contract (plaintext hash "
			"mismatch)");
	
	hash = Hash::hash(ctext, ctHash.alg, ctHash.key, ctHash.type);
	if (ctHash != hash)
		throw CashException(CashException::CE_FE_ERROR,
			"[FEContract::checkHashes] Malformed contract (ciphertext hash "
			"mismatch)");
	return true;
}

bool FEContract::checkAHash(const hash_t& ptHash, const hash_t& ctHash) const {
	return (checkHash(ptHash, ptHashA) && checkHash(ctHash, ctHashA));
}

bool FEContract::checkBHash(const hash_t& ptHash, const hash_t& ctHash) const {
	return (checkHash(ptHash, ptHashB) && checkHash(ctHash, ctHashB));
}

bool FEContract::checkHash(const hash_t& hashGiven, 
						   const hash_t& hashStored) const {
	if (hashGiven != hashStored)
		throw CashException(CashException::CE_FE_ERROR,
				"[FEContract::checkHash] Malformed contract (hash mismatch)");
	return true;
}
