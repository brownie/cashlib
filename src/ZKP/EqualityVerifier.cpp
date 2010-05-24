
#include "EqualityVerifier.h"
#include "ASTNode.h"
#include <assert.h>
#include "MultiExp.h"

#define DEBUG 0

bool EqualityVerifier::canGenerateNewChallenge() const {
	// XXX for now, let's leave this class to experienced users and 
	// have no checks
	return true;
}

bool EqualityVerifier::verify(variable_map &response) {

	for (dlr_map::const_iterator it = env.descriptions.begin(); 
								 it != env.descriptions.end(); ++it) {
		DLRepresentation cd = it->second;
		// Check if  rProof_i * C_i ^ c = SUM(i = 0:n) base_i ^ response_i % mod
		ZZ mod = env.groups.at(cd.group)->getModulus();
		ZZ rProofBase = getRandomizedProofs().at(cd.toString());

		ZZ commitmentBase = env.variables.at(cd.left->toString());
		ZZ rCom = PowerMod(commitmentBase, challenge, mod);
		ZZ leftSide = MulMod(rProofBase, rCom, mod);

		vector<string> baseNames;
		vector<ZZ> bases;
		vector<ZZ> exps;
		for(unsigned j = 0; j < cd.bases.size(); j++) {		
			string baseName = cd.bases[j]->toString();
			baseNames.push_back(baseName);
			bases.push_back(env.variables.at(baseName));
			exps.push_back(response.at(cd.exps[j]->toString()));
		}
		ZZ rightSide = env.multiExp(baseNames, bases, exps, mod);
		//ZZ rightSide = MultiExp(bases, exps, mod);
		if(leftSide != rightSide) {
			cout << "******************************************" << endl;
			cout << "failed to verify: " << endl << cd.toString() << endl;
			cout << leftSide << endl << " != " << endl << rightSide << endl;
			cout << "randomized proof is " << rProofBase << endl;
			cout << "commitment is " << commitmentBase << endl;
			cout << "******************************************" << endl;
			return false;
		}
		else {
#if DEBUG
			cout << "VERIFIED commitment " << cd.toString() << endl;
#endif
		}

	}
	// if we got here, all equations were verified!
	return true;
}
