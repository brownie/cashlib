#ifndef MERKLEPROOF_H_
#define MERKLEPROOF_H_

#include "MerkleContract.h"

/*! \brief This class is just a storage container for the two Merkle 
 * proofs we need for fair exchange resolutions */

class MerkleProof {
	public:
		MerkleProof() : ctContract(0), ptContract(0) {empty=true;}
		
		/*! ctextBlocks represent the blocks of the ciphertext that we are
		 * proving are correct; ctextProof is the proof that the Merkle hash
		 * of the ciphertext was formed correctly; fileProof is the proof
		 * that the Merkle hash of the plaintext was formed correctly 
		 * the contracts store the hashalg and key */
		MerkleProof(const vector<EncBuffer*> &ctextBlocks, 
					const hash_matrix &ctextProof, 
					const hash_matrix &fileProof, 
					MerkleContract* ctContract, MerkleContract* ptContract);
		
		MerkleProof(const vector<EncBuffer*> &ctextBlocks, 
					const hash_matrix &ctextProof, const string &plaintext, 
					MerkleContract* ctContract);
		
		MerkleProof(const vector<EncBuffer*> &ctextBlocks, 
					const hash_matrix &ctextProof, MerkleContract* ctContract);

		MerkleProof(const MerkleProof &o);

		// getters
		vector<EncBuffer*> getCTextBlocks() const { return ctextBlocks; }
		hash_matrix getCTextProof() const { return ctextProof; }
		hash_matrix getPTextProof() const { return fileProof; }
		string getPlaintext() const { return plaintext; }
		MerkleContract* getPTContract() const { return ptContract; }
		MerkleContract* getCTContract() const { return ctContract; }
		bool isEmpty() const { return empty; }

	private:
		vector<EncBuffer*> ctextBlocks;
		hash_matrix ctextProof;
   		hash_matrix fileProof;
   		string plaintext;
   		MerkleContract* ctContract;
   		MerkleContract* ptContract;
		bool empty;
		
		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(ctextBlocks)
				& auto_nvp(ctextProof)
				& auto_nvp(fileProof)
				& auto_nvp(plaintext)
				& auto_nvp(ctContract)
				& auto_nvp(ptContract)
				& auto_nvp(empty);
		}
};

#endif /*MERKLEPROOF_H_*/
