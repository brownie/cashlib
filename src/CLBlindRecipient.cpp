
#include "CLBlindRecipient.h"
#include "ZKP/InterpreterVerifier.h"
#include <assert.h>
#include "Timer.h"

CLBlindRecipient::CLBlindRecipient(Ptr<const GroupRSA> pk, Ptr<const Group> comGroup,
								   int lx, const vector<ZZ> &coms, 
								   int numPrivates, int numPublics) 
	: numPrivates(numPrivates), numPublics(numPublics)
{	
	// setting up all maps
	g["pkGroup"] = pk;
	g["comGroup"] = comGroup;
	v["modSize"] = pk->getModulusLength();
	v["stat"] = to_ZZ(pk->getStat());
	v["l_x"] = to_ZZ(lx);
	for (unsigned i = 0; i < coms.size(); i++) {
		string name = "c_" + lexical_cast<string>(i+1);
		v[name] = coms[i];
	}

	inputs["l"] = to_ZZ(numPrivates);
	inputs["k"] = to_ZZ(numPublics);
	
	prover.check("ZKP/examples/cl-obtain-ecash.txt", inputs, g);
}

CLBlindRecipient::CLBlindRecipient(Ptr<const GroupRSA> pk, int lx, int numPrivates,
								   int numPublics, const gen_group_map &grps,
								   const vector<CommitmentInfo> &coms) 
	: numPrivates(numPrivates), numPublics(numPublics)
{
	v["modSize"] = pk->getModulusLength();
	v["stat"] = to_ZZ(pk->getStat());
	v["l_x"] = to_ZZ(lx);
	// first set up groups
	vector<string> genNames;
	genNames.push_back("f");
	for (int i = 0; i < numPrivates+numPublics; i++) {
		genNames.push_back("g_"+lexical_cast<string>(i+1));
	}
	genNames.push_back("h");
	gen_group_map pMap(grps);
	pMap["pkGroup"] = make_pair(pk, genNames);
	for (gen_group_map::iterator it = pMap.begin(); it != pMap.end(); ++it) {
		// insert into environment
		g[it->first] = it->second.first;
	}
	// now need to insert commitments
	for (unsigned i = 0; i < coms.size(); i++) {
		v["c_"+lexical_cast<string>(i+1)] = coms[i].comValue;
	}
	
	string fname = ProgramMaker::makeCLObtain(pMap, coms);
	inputs["l"] = to_ZZ(numPrivates);
	inputs["k"] = to_ZZ(numPublics);
	prover.check(fname, inputs, g);
}

Ptr<ProofMessage> CLBlindRecipient::getC(const vector<SecretValue>& privates,
									 const hashalg_t &hashAlg) {
	if((int)privates.size() != numPrivates) 
		throw CashException(CashException::CE_SIZE_ERROR,
		"[CLBlindRecipient::getC] Number of private inputs does not match "
		"the number the recipient was constructed with");
	
	// add the private messages to the variable map, as well as the
	// randomness used to form their commitments
	for(unsigned i = 0; i < privates.size(); i++){
		string index = lexical_cast<string>(i+1);
		v["x_"+index] = privates[i].first;
		v["r_"+index] = privates[i].second;
	}
	// XXX: what about public messages?
	
	startTimer();
	prover.compute(v);
	variable_map publics = prover.getPublicVariables();
	SigmaProof proof = prover.computeProof(hashAlg);
	printTimer("[CLBlindRecipient] created proof");
	
	variable_map vals;
	variable_map pVars = prover.getEnvironment().variables;
	vals["C"] = pVars.at("C");
	vals["vprime"] = pVars.at("vprime");
	return new_ptr<ProofMessage>(vals, publics, proof);
}

bool CLBlindRecipient::verifySig(const ProofMessage &pm, int stat){
	startTimer();
	InterpreterVerifier verifier;
    verifier.check("ZKP/examples/cl-issue.txt", inputs);
	// XXX: does the verifier really not need any inputs of its own?
	// this would be a good place to incorporate public messages...
	variable_map q;
	variable_map pubs = pm.publics;
    verifier.compute(q, pubs, g);
	
	bool verified = verifier.verify(pm.proof, stat);
	printTimer("[CLBlindRecipient] verified issuer proof");
	return verified;
}

vector<ZZ> CLBlindRecipient::createSig(const vector<ZZ> &partialSig) {
	// just need to add v' to v'' (in partial signature) and we're done
	assert(partialSig.size() == 3);
	vector<ZZ> sig;
	sig.push_back(partialSig[0]);
	sig.push_back(partialSig[1]);
	ZZ vprime = prover.getEnvironment().variables.at("vprime");
	ZZ v = vprime + partialSig[2];
	sig.push_back(v);
	return sig;
}
