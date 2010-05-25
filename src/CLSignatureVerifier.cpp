#include "CLSignatureVerifier.h"

CLSignatureVerifier::CLSignatureVerifier(const GroupRSA* publicKey, 
										 const Group* comGroup, int lx,
										 const vector<ZZ> &coms, 
										 int numPrivates, int numPublics)
	: numPrivates(numPrivates), numPublics(numPublics)
{	
	//setting up group and input maps for constant substitution
	g["pkGroup"] = publicKey;
	g["comGroup"] = comGroup;
	v["stat"] = publicKey-> getStat();
	v["l_x"] = lx;
	v["modSize"] = publicKey->getModulusLength();
	for (unsigned i = 0; i < coms.size(); i++) {
		v["c_"+lexical_cast<string>(i+1)] = coms[i];
	}
	inputs["l"] = numPrivates;
	inputs["k"] = numPublics;
	verifier.check("ZKP/examples/cl-prove-ecash.txt", inputs);
}

CLSignatureVerifier::CLSignatureVerifier(const GroupRSA* pk, int lx,
										 int numPrivates, int numPublics,
										 const gen_group_map &groups,
										 const vector<CommitmentInfo> &coms) 
	: numPrivates(numPrivates), numPublics(numPublics)
{
	inputs["l"] = numPrivates;
	inputs["k"] = numPublics;
	v["l_x"] = lx;
	v["stat"] = pk->getStat();
	v["modSize"] = pk->getModulusLength();
	gen_group_map pMap(groups);
	vector<string> genNames;
	genNames.push_back("f");
	for (int i = 0; i < numPrivates + numPublics; i++) {
		genNames.push_back("g_"+lexical_cast<string>(i+1));
	}
	genNames.push_back("h");
	pMap["pkGroup"] = make_pair(pk, genNames);
	for (gen_group_map::iterator it = pMap.begin(); it != pMap.end(); ++it) {
		g[it->first] = it->second.first;
	}
	for (unsigned i = 0; i < coms.size(); i++) {
		v["c_"+lexical_cast<string>(i+1)] = coms[i].comValue;
	}
	string fname = ProgramMaker::makeCLProve(pMap, coms);
	verifier.check(fname, inputs);
}	

bool CLSignatureVerifier::verify(const ProofMessage* pm, int stat) {
	SigmaProof proof = pm->proof;
	variable_map publics1 = pm->proof.getCommitments();
	variable_map publics2 = pm->publics;
	verifier.compute(v, publics1, publics2, g);
	return verifier.verifyProof(proof, stat);
}

