#ifndef MERKLEPROOF_H_
#define MERKLEPROOF_H_

#include "MerkleContract.h"

/*! \brief This class is just a storage container for the two Merkle 
 * proofs we need for fair exchange resolutions */

class MerkleProof {
	public:
		MerkleProof() : ctContract(), ptContract() {empty=true;}
		
		/*! ctextBlocks represent the blocks of the ciphertext that we are
		 * proving are correct; ctextProof is the proof that the Merkle hash
		 * of the ciphertext was formed correctly; fileProof is the proof
		 * that the Merkle hash of the plaintext was formed correctly 
		 * the contracts store the hashalg and key */
		MerkleProof(const vector<Ptr<EncBuffer> > &ctextBlocks, 
					const hash_matrix &ctextProof, 
					const hash_matrix &fileProof, 
					Ptr<MerkleContract> ctContract, Ptr<MerkleContract> ptContract);
		
		MerkleProof(const vector<Ptr<EncBuffer> > &ctextBlocks, 
					const hash_matrix &ctextProof, const string &plaintext, 
					Ptr<MerkleContract> ctContract);
		
		MerkleProof(const vector<Ptr<EncBuffer> > &ctextBlocks, 
					const hash_matrix &ctextProof, Ptr<MerkleContract> ctContract);

		MerkleProof(const MerkleProof &o);

		// getters
		vector<Ptr<EncBuffer> > getCTextBlocks() const { return ctextBlocks; }
		hash_matrix getCTextProof() const { return ctextProof; }
		hash_matrix getPTextProof() const { return fileProof; }
		string getPlaintext() const { return plaintext; }
		Ptr<MerkleContract> getPTContract() const { return ptContract; }
		Ptr<MerkleContract> getCTContract() const { return ctContract; }
		bool isEmpty() const { return empty; }

	private:
		vector<Ptr<EncBuffer> > ctextBlocks;
		hash_matrix ctextProof;
   		hash_matrix fileProof;
   		string plaintext;
   		Ptr<MerkleContract> ctContract;
   		Ptr<MerkleContract> ptContract;
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
