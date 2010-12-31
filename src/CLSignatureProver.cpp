#include "CLSignatureProver.h"
#include <assert.h>
#include "Timer.h"

CLSignatureProver::CLSignatureProver(const GroupRSA* publicKey, 
									 const Group* comGroup, int lx,
									 const vector<ZZ> &coms, int numPrivates, 
									 int numPublics)
	:numPrivates(numPrivates), numPublics(numPublics)
{	
	//setting up maps
	g["pkGroup"] = publicKey;
	g["comGroup"] = comGroup;
	v["stat"] = publicKey-> getStat();
	v["l_x"] = lx;
	v["modSize"] = publicKey->getModulusLength();
	assert((int)coms.size() == numPrivates);
	for (unsigned i = 0; i < coms.size(); i++) {
		v["c_"+lexical_cast<string>(i+1)] = coms[i];
	}

	inputs["l"] = numPrivates;
	inputs["k"] = numPublics;
	prover.check(CommonFunctions::getZKPDir()+"/cl-prove-ecash.txt", inputs, g);
}

CLSignatureProver::CLSignatureProver(const GroupRSA* pk, int lx, 
									 int numPrivates, int numPublics,
									 const gen_group_map &grps,
									 const vector<CommitmentInfo> &coms) 
	: numPrivates(numPrivates), numPublics(numPublics)
{
	v["modSize"] = pk->getModulusLength();
	v["l_x"] = lx;
	v["stat"] = pk->getStat();
	vector<string> genNames;
	genNames.push_back("f");
	for (int i = 0; i < numPrivates+numPublics; i++) {
		genNames.push_back("g_"+lexical_cast<string>(i+1));
	}
	genNames.push_back("h");
	gen_group_map pMap(grps);
	pMap["pkGroup"] = make_pair(pk, genNames);
	for (gen_group_map::iterator it = pMap.begin(); it != pMap.end(); ++it) {
		g[it->first] = it->second.first;
	}
	// insert commitments
	for (unsigned i = 0; i < coms.size(); i++) {
		v["c_"+lexical_cast<string>(i+1)] = coms[i].comValue;
	}
	string fname = ProgramMaker::makeCLProve(pMap, coms);
	inputs["l"] = numPrivates;
	inputs["k"] = numPublics;
	prover.check(fname, inputs, g);
}

ProofMessage* CLSignatureProver::getProof(const vector<ZZ>& sig, 
										  const vector<SecretValue>& privates, 
										  const vector<ZZ>& publics,
										  const hashalg_t &hashAlg){
	if((int)publics.size()!=numPublics) 
		throw CashException(CashException::CE_SIZE_ERROR,
		"[CLSignatureProver::getC] Number of public inputs does not "
		"match the number the issuer was constructed with");
		
	if((int)privates.size()!=numPrivates) 
		throw CashException(CashException::CE_SIZE_ERROR,
		"[CLSignatureProver::getC] Number of private inputs does not "
		"match the number the recipient was constructed with");
		
	// set up proving environment 
	// XXX: need to check that all the public values are in the 
	// proper range, don't we?
	for(unsigned i = 0; i < publics.size(); i++)
		v["x_"+lexical_cast<string>(i+numPrivates+1)] = publics[i];
		
	for(unsigned i = 0; i < privates.size(); i++) {
		v["x_"+lexical_cast<string>(i+1)] = privates[i].first;
		v["r_"+lexical_cast<string>(i+1)] = privates[i].second;
	}
		
	v["A"] = sig[0];
	v["e"] = sig[1];
	v["v"] = sig[2];
	startTimer();
	prover.compute(v);
	printTimer("CL prover computed all values");
	return new ProofMessage(prover.getPublicVariables(), 
							prover.computeProof(hashAlg));
}

