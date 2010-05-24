#ifndef _MERKLEVERIFIER_H_
#define _MERKLEVERIFIER_H_

#include "MerkleContract.h"

class MerkleVerifier {

	public:
		MerkleVerifier(const hash_t &rt, unsigned numLeaves, 
					   const MerkleContract &contract);	

		vector<unsigned> getChallenges() const { return challenges; }
		
		/*! checks to see if each proof is valid (using checkProof) */
		bool verifyProofs(const hash_matrix &proofs);

	private:	
		/*! generates pseudorandom challenges of the appropriate size */
		vector<unsigned> generateChallenges();
		
		/*! checks an individual proof to see if it is valid */
		bool checkProof(vector<hashDirect> &proof);
		
		MerkleContract contract;
		hash_t root;
		unsigned numBlocks;
		vector<unsigned> challenges;
};

#endif
