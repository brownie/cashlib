
#include "CLBlindIssuer.h"
#include "ZKP/InterpreterProver.h"
#include "Timer.h"

CLBlindIssuer::CLBlindIssuer(const GroupRSA* sk, const Group* comGroup, 
							 int lx, const vector<ZZ> &coms,
							 int numPrivates, int numPublics)
	: numPrivates(numPrivates), numPublics(numPublics)
{	
	// setting up the variable map based on inputs	
	inputs["l"] = numPrivates;
	inputs["k"] = numPublics;
	v["l_x"] = lx;
	v["stat"] = sk->getStat();
	v["modSize"] = sk->getModulusLength();	  
	for (unsigned i = 0; i < coms.size(); i++) {
		v["c_"+lexical_cast<string>(i+1)] = coms[i];
	}
	g["pkGroup"] = sk;	
	g["comGroup"] = comGroup;

	verifier.check(CommonFunctions::getZKPDir()+"/cl-obtain-ecash.txt", inputs, g);	
}

CLBlindIssuer::CLBlindIssuer(const GroupRSA* sk, int lx, int numPrivates,
							 int numPublics, const gen_group_map &groups,
							 const vector<CommitmentInfo> &coms) 
	: numPrivates(numPrivates), numPublics(numPublics)
{
	inputs["l"] = numPrivates;
	inputs["k"] = numPublics;
	v["l_x"] = lx;
	v["stat"] = sk->getStat();
	v["modSize"] = sk->getModulusLength();
	gen_group_map pMap(groups);
	vector<string> genNames;
	genNames.push_back("f");
	for (int i = 0; i < numPrivates + numPublics; i++) {
		genNames.push_back("g_"+lexical_cast<string>(i+1));
	}
	genNames.push_back("h");
	pMap["pkGroup"] = make_pair(sk, genNames);
	for (gen_group_map::iterator it = pMap.begin(); it != pMap.end(); ++it) {
		g[it->first] = it->second.first;
	}
	for (unsigned i = 0; i < coms.size(); i++) {
		v["c_"+lexical_cast<string>(i+1)] = coms[i].comValue;
	}
	string fname = ProgramMaker::makeCLObtain(pMap, coms);
	verifier.check(fname, inputs, g);
}

CLBlindIssuer::CLBlindIssuer(const CLBlindIssuer &o) 
	: inputs(o.inputs), g(o.g), numPrivates(o.numPrivates), 
	  numPublics(o.numPublics), v(o.v), verifier(o.verifier)
{
}

ProofMessage* CLBlindIssuer::getPartialSignature(const ZZ &C, 
												 const vector<ZZ>& publics, 
												 const ProofMessage &pm, 
												 int stat, 
												 const hashalg_t &hashAlg) {
	// at this point, verifier will know which program it is working with
	variable_map proverPublics = pm.proof.getCommitments();
	variable_map pv = pm.publics;
    verifier.compute(v, proverPublics, pv);

	SigmaProof proof = pm.proof;
	startTimer();
	bool verified = verifier.verify(proof, stat);
	printTimer("[CLBlindIssuer] verified recipient proof");
	if(verified){
		// XXX: should we be checking that public values are in the 
		// proper range? same goes for recipient side of things too...
		if((int)publics.size() != numPublics) 
			throw CashException(CashException::CE_SIZE_ERROR,
			"[CLBlindIssuer::getPartialSignature] Number of public inputs "
			"does not match the number the issuer was constructed with");

		startTimer();
		InterpreterProver prover;
		prover.check(CommonFunctions::getZKPDir()+"/cl-issue.txt", inputs, g);

		// want to keep the same inputs and groups, but need a new variable 
		// map for doing issue program
		ZZ lx = v.at("l_x");
		v.clear();
		const GroupRSA* grp = (GroupRSA*) g.at("pkGroup");
		v["l_x"] = lx;
		v["stat"] = grp->getStat();
		v["modSize"] = grp->getModulusLength();
		v["C"] = C;
		for(unsigned i = 0; i < publics.size(); i++)
			v["x_"+lexical_cast<string>(i+numPrivates+1)] = publics[i];
		prover.compute(v);
	
		// set up partial signature to include in message for recipient
		variable_map partialSig;
		variable_map pVars = prover.getEnvironment().variables;
		partialSig["A"] = pVars.at("A");
		partialSig["e"] = pVars.at("e");
		partialSig["vdoubleprime"] = pVars.at("vdoubleprime");
		
		variable_map p = prover.getPublicVariables();
		SigmaProof  pr = prover.computeProof(hashAlg);
		printTimer("[CLBlindIssuer] computed issuer proof");
		return new ProofMessage(partialSig, p, pr);
	} else {
		 throw CashException(CashException::CE_PARSE_ERROR,
                "[CLBlindIssuer::getPartialSignature] Proof did not verify");
	}
}
