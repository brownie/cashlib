#include "MerkleVerifier.h"
#include <algorithm>
#define numChallenges 22
#define MIN(x, y) ( (x) < (y) ? (x) : (y) )

MerkleVerifier::MerkleVerifier(const hash_t &rt, unsigned numLeaves, 
							   const MerkleContract &contract) 
	: contract(contract)
{
	numBlocks = numLeaves;
	root = rt;
	challenges = generateChallenges();
}

vector<unsigned> MerkleVerifier::generateChallenges() {
	unsigned nChallenges = MIN(numBlocks, numChallenges);	
	vector<unsigned> toReturn;
	while(toReturn.size() < nChallenges) {
		unsigned rand = NTL::RandomBnd(numBlocks);
		vector<unsigned>::iterator it = std::find(toReturn.begin(), 
												  toReturn.end(), rand);
		if (it != toReturn.end())
			continue;
		toReturn.push_back(NTL::RandomBnd(numBlocks));
	}
	return toReturn;
}

bool MerkleVerifier::verifyProofs(const hash_matrix &proofs){
	bool valid = true;
	unsigned i = 0;
	do {
		vector<hashDirect> ith = proofs[i];
		valid = checkProof(ith);
		i++;
	} while(valid && i < proofs.size());
	return valid;
}

bool MerkleVerifier::checkProof(vector<hashDirect> &proof) {
	if(proof.size() == 1){
		proof[0].node.type = Hash::TYPE_MERKLE;
		return (proof[0].node == root);
	}
	for(unsigned i = 0; i < proof.size()-1; i++) {
		if(proof[i+1].path[0]){
			proof[i+1].node = contract.hash(proof[i].node, proof[i+1].node);
		} else {
			proof[i+1].node = contract.hash(proof[i+1].node, proof[i].node);
		}
	}
	proof[proof.size()-1].node.type = Hash::TYPE_MERKLE;
	return (proof[proof.size()-1].node == root);
}

