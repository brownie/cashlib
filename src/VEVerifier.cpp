
#include "VEVerifier.h"
#include "ZKP/InterpreterVerifier.h"
#include "GroupSquareMod.h"
#include "Timer.h"

bool VEVerifier::verify(const VECiphertext& text, const ZZ& x, 
                        const Group *grp, const string& label, 
                        const hashalg_t& hashAlg, int stat) {
	// set everything up (a lot like the prover side)
	Environment env;
	env.variables["X"] = x;
	env.variables["Xprime"] = text.getCommitment();
	// also add a values
	vector<ZZ> as = pk->getAValues();
	for (unsigned i = 0; i < as.size(); i++) {
		string name = "a_" + lexical_cast<string>(i+1);
		env.variables[name] = as[i];
	}
	env.variables["b"] = pk->getB();
	env.variables["d"] = pk->getD();
	env.variables["e"] = pk->getE();
	env.variables["f"] = pk->getF();

	// set up all groups
	env.groups["cashGroup"] = grp;
	ZZ bigN = pk->getN();
	env.groups["RSAGroup"] = new GroupRSA("arbiter", bigN, stat);
	ZZ bigNSquared = power(bigN, 2);
	env.groups["G"] = new GroupSquareMod("arbiter", bigNSquared, stat);
	const GroupRSA* second = new GroupRSA(pk->getSecondGroup());
	env.groups["secondGroup"] = second;
	input_map inputs;
	inputs["m"] = text.getCiphertext().size() - 2;

	// okay, now that everything is all set, just run the program
	SigmaProof proof = text.getProof();
	variable_map publics = text.getPublics();
	InterpreterVerifier verifier;
	verifier.check("ZKP/examples/ve.txt", inputs, env.groups);
	verifier.compute(env.variables, proof.getCommitments(), publics);
	return verifier.verify(proof, stat);
}
