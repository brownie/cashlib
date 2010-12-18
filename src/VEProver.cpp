
#include "VEProver.h"
#include "CommonFunctions.h"
#include "GroupSquareMod.h"
#include "ZKP/InterpreterProver.h"
#include <assert.h>
#include "Timer.h"

vector<ZZ> VEProver::encrypt(const vector<ZZ> &messages, const string &label,
							 const hashalg_t &hashAlg, int stat) {
	Environment e;
	// first create and give in our group
	ZZ bigN = pk->getN();
	e.groups["RSAGroup"] = make_shared<GroupRSA>("arbiter", bigN, stat);
	ZZ bigNSquared = power(bigN, 2);
	e.groups["G"] = make_shared<GroupSquareMod>("arbiter", bigNSquared, stat);
	// now add in elements
	e.variables["f"] = pk->getF();
	e.variables["b"] = pk->getB();
	vector<ZZ> as = pk->getAValues();
	// XXX: only add as many a values as there as message values
	// should come up with something better for this
	assert(as.size() >= messages.size());
	for (unsigned i = 0; i < messages.size(); i++) {
		string name = "a_" + lexical_cast<string>(i+1);
		e.variables[name] = as[i];
	}
	// now add x_i's
	for (unsigned i = 0; i < messages.size(); i++) {
		string name = "x_" + lexical_cast<string>(i+1);
		e.variables[name] = messages[i];
	}
	input_map inputs;
	int m = messages.size();
	inputs["m"] = m;	

	// now do the computation: this gives us r, u_i, v
	InterpreterProver calculator;
	calculator.check("ZKP/examples/encrypt.txt", inputs, e.groups);
	startTimer();
	calculator.compute(e.variables);
	printTimer("Computed stuff for normal encryption");

	// get new environment
	e = calculator.getEnvironment();
	// now need to compute w = abs([d * e^hash(...)]^r mod N^2)
	ZZ d = pk->getD();
	ZZ eVal = pk->getE();
	e.variables["d"] = d;
	e.variables["e"] = eVal;
	// need hash vector to be u_1, ..., u_m, v
	// set initial size for ciphertext so we can order it
	vector<ZZ> ciphertext(m+1);
	for (variable_map::iterator it = e.variables.begin();
								it != e.variables.end(); ++it) {
		if (it->first.find("u_") != string::npos) {
			// XXX: this is string parsing -- very undesirable!
			int index = lexical_cast<int>(it->first.substr(2));
			ciphertext[index-1] = it->second;
		} else if (it->first.find("v") != string::npos) {
			ciphertext[m] = it->second;
		}
	}
	string hashKey = pk->getHashKey();
	ZZ hash = ZZFromBytes(Hash::hash(ciphertext, label,
													  hashAlg, hashKey));
	e.variables["hash"] = hash;
	ZZ eHash = PowerMod(eVal, hash, bigNSquared);
	ZZ de = MulMod(d, eHash, bigNSquared);
	ZZ r = e.variables.at("r");
	ZZ w = CommonFunctions::abs(PowerMod(de, r, bigNSquared), bigNSquared);
	// insert w into environment
	e.variables["w"] = w;

	// save environment
	env = e;
	
	// now output full ciphertext u_1,...,u_m,v,w
	ciphertext.push_back(w);
	return ciphertext;
}

VECiphertext VEProver::verifiableEncrypt(const ZZ &commitment,
										 const vector<ZZ> &opening, 
										 Ptr<const Group> grp, const string &label, 
										 const hashalg_t &hashAlg, int stat) {
	// if we run encrypt, environment will have most of the information we need
	vector<ZZ> ciphertext = encrypt(opening, label, hashAlg, stat);

	// now need some more group information
	env.groups["cashGroup"] = grp;
	Ptr<const GroupRSA> second = make_shared<GroupRSA>(pk->getSecondGroup());
	env.groups["secondGroup"] = second;

	env.variables["X"] = commitment;
	
	// again want to bind m, right?
	input_map inputs;
	inputs["m"] = opening.size();

	InterpreterProver prover;
	prover.check("ZKP/examples/ve.txt", inputs, env.groups);
	startTimer();
	prover.compute(env.variables);
	printTimer("Computed values for verifiable encryption");
	startTimer();
	SigmaProof proof = prover.computeProof(hashAlg);
	printTimer("Computed proof for verifiable encryption");
	Environment e = prover.getEnvironment();

	variable_map publics = prover.getPublicVariables();
	// can take a lot of stuff out of this map before giving to verifier
	publics.erase("b");
	publics.erase("d");
	publics.erase("e");
	publics.erase("f");
	for (unsigned i = 0; i < opening.size(); i++) {
		publics.erase("a_"+lexical_cast<string>(i+1));
	}
	publics.erase("X");
	publics.erase("Xprime");
	// XXX: there is repetition between ciphertext and publics, but having
	// squared values in publics saves the verifier from doing some
	// computation... is that worth it?

	VECiphertext result(ciphertext, e.variables.at("Xprime"), publics, proof);
	return result;
}

