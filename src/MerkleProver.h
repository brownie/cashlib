#ifndef _MERKLEPROVER_H_
#define _MERKLEPROVER_H_

#include "Merkle.h"

class MerkleProver {

	public:
		MerkleProver(const vector<Ptr<EncBuffer> > &encBuffs, 
					 const MerkleContract &contract);

		MerkleProver(const vector<Ptr<const Buffer> > &buffs, 
					 const MerkleContract &contract);

		MerkleProver(const string &str, const MerkleContract &contract);
		
		MerkleProver(const char* buff, int buffSize, 
					 const MerkleContract &contract);
		
		MerkleProver(const vector<hash_t> &hashBlocks, 
					 const MerkleContract &contract);
		
		hash_matrix generateProofs(const vector<unsigned> &challenges);
		
		unsigned getNumBlocks() const { return tree->getNumLeaves(); }
		hash_t getRoot() const { return tree->getRoot(); }
	
	private:	
		vector<hashDirect> generateProof(unsigned challenge, 
										 vector<hash_t> &m_tree);
		
		void init(const char* buff, int buffSize);
		
		hash_t computeSubTree(unsigned index, vector<hash_t> &m_tree);
		
		MerkleContract contract;
		Ptr<MerkleTree> tree;

};

#endif
