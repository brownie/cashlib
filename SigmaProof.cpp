
#include "SigmaProof.h"

SigmaProof::SigmaProof(const SigmaProof &o)
  : randomizedProofs(o.randomizedProofs), responses(o.responses),
	hashAlg(o.hashAlg), commitments(o.commitments)
{
}

ZZ SigmaProof::computeChallenge() const {
	ZZ result;
	vector<ZZ> randProofValues;
	for (var_map::const_iterator it = randomizedProofs.begin();
								 it != randomizedProofs.end(); ++it) {
		randProofValues.push_back(it->second);
	}
	result = Hash::hash(randProofValues, hashAlg);
	return result;
}

void SigmaProof::dump() const {
	cout << "SigmaProof (hashAlg " << hashAlg << ")" << endl;
	for (var_map::const_iterator it = randomizedProofs.begin();
								 it != randomizedProofs.end(); ++it) 
		cout << "rproofs " << it->first << " : " << it->second << endl;
	for (var_map::const_iterator it = responses.begin(); 
								 it != responses.end(); ++it) 
		cout << "responses " << it->first << " : " << it->second << endl;
	for (var_map::const_iterator it = commitments.begin();
								 it != commitments.end(); ++it) 
		cout << "commitments " << it->first << " : " << it->second << endl;
}
