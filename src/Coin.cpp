
#include "Coin.h"
#include "ZKP/InterpreterProver.h"
#include "ZKP/InterpreterVerifier.h"
#include "MultiExp.h"
#include "CLSignatureProver.h"
#include "CLSignatureVerifier.h"
#include "Timer.h"

Coin::Coin(const BankParameters* params, int wSize, int index,
		   const ZZ &skIn, const ZZ &sIn, const ZZ &tIn, 
		   const vector<ZZ> &clSig, int st, int l, 
		   const ZZ &rVal, int denom, const hashalg_t &ha) 
	: stat(st), lx(l), coinDenom(denom), parameters(params), walletSize(wSize), 
	  coinIndex(index), sk_u(skIn), s(sIn), t(tIn), R(rVal), hashAlg(ha), 
	  signature(clSig)
{  
	// first do all the stuff to prove S and T are correctly formed
	group_map g;
	variable_map v;
	g["cashGroup"] = parameters->getCashGroup();
	v["sk_u"] = sk_u;
	v["s"] = s;
	v["t"] = t;
	v["J"] = coinIndex;
	
	InterpreterProver prover;
	prover.check("ZKP/examples/ecash.txt", g);
	prover.compute(v);
	Environment env = prover.getEnvironment();

	// want to store all commitments
	B = env.getCommitmentValue("sk_u");
	C = env.getCommitmentValue("s");
	D = env.getCommitmentValue("t");
	S = env.variables.at("S");
	T = env.variables.at("T");
	// also want to store endorsement information
	y = env.variables.at("y");
	endorsement.push_back(env.variables.at("x1"));
	endorsement.push_back(env.variables.at("x2"));
	endorsement.push_back(env.variables.at("r_y"));

	coinProof = ProofMessage(prover.getPublicVariables(), 
		   					 prover.computeProof(hashAlg));

	// now need to do CL stuff
	// public message is the wallet size W
	// secret messages are sk_u, s, and t
	const GroupRSA* pk = parameters->getBankKey(coinDenom);
	const GroupPrime* comGroup = parameters->getCashGroup();
	vector<ZZ> coms;
	coms.push_back(B);
	coms.push_back(C);
	coms.push_back(D);

	startTimer();
	CLSignatureProver clProver(pk, comGroup, lx, coms, 3, 1);
	vector<SecretValue> privates;
	privates.push_back(make_pair(sk_u, env.variables.at("r_B")));
	privates.push_back(make_pair(s, env.variables.at("r_C")));
	privates.push_back(make_pair(t, env.variables.at("r_D")));
	vector<ZZ> publics;
	publics.push_back(walletSize);
	ProofMessage* pm = clProver.getProof(signature, privates, publics,
										 hashAlg);
	printTimer("[Coin] got proof for CL");
	clProof = *pm;
}

Coin::Coin(const Coin &o) 
	: stat(o.stat), lx(o.lx), coinDenom(o.coinDenom), parameters(o.parameters), 
	  walletSize(o.walletSize), coinIndex(o.coinIndex), sk_u(o.sk_u), s(o.s), 
	  t(o.t), R(o.R), hashAlg(o.hashAlg), B(o.B), C(o.C), D(o.D), S(o.S), 
	  T(o.T), endorsement(o.endorsement), signature(o.signature), y(o.y), 
	  coinProof(o.coinProof), clProof(o.clProof)
{
}	

bool Coin::verifyEndorsement(const vector<ZZ> &endorse) {
	
	vector<ZZ> bases;
	const GroupPrime* cashGroup = parameters->getCashGroup();
	ZZ mod = cashGroup->getModulus();
	bases.push_back(cashGroup->getGenerator(3)); // h1
	bases.push_back(cashGroup->getGenerator(4)); // h2
	bases.push_back(cashGroup->getGenerator(0)); // f
	// now do the multiexp
	ZZ result = MultiExp(bases, endorse, mod);
	return (result == y);
}

bool Coin::endorse(const vector<ZZ> &e) {
	if (verifyEndorsement(e)) {
		endorsement = e;
		return true;
	} else {
		return false;
	}
}

bool Coin::verifyCoin() const {
	// first check the ST part of the proof (coinProof)
	InterpreterVerifier verifier;
	group_map cashG;
	cashG["cashGroup"] = parameters->getCashGroup();
	startTimer();
	verifier.check("ZKP/examples/ecash.txt", cashG);
	printTimer("[Coin] Verifier checked ST part");
	variable_map cashV;
	variable_map coinPub = coinProof.publics;
	SigmaProof cashProof = coinProof.proof;
	verifier.compute(cashV, coinPub);
	bool ecashVerified = verifier.verify(cashProof, stat);

	// now do CL stuff		
	startTimer();
	const GroupRSA* pk = parameters->getBankKey(coinDenom);
	const GroupPrime* comGroup = parameters->getCashGroup();

	vector<ZZ> coms;
	coms.push_back(B);
	coms.push_back(C);
	coms.push_back(D);

	startTimer();
	CLSignatureVerifier clVerifier(pk, comGroup, lx, coms, 3, 1);
	printTimer("[Coin] Verifier checked CL part");
	bool clVerified = clVerifier.verify(&clProof, stat);
	printTimer("[Coin] Verified CL possession");
	return (ecashVerified && clVerified);
}

ZZ Coin::getSPrime() const {
	ZZ g = parameters->getCashGroup()->getGenerator(1);
	ZZ mod = parameters->getCashGroup()->getModulus();
	return MulMod(S, InvMod(PowerMod(g, endorsement[0], mod), mod), mod);
}

ZZ Coin::getTPrime() const {
	ZZ g = parameters->getCashGroup()->getGenerator(1);
	ZZ mod = parameters->getCashGroup()->getModulus();
	return MulMod(T, InvMod(PowerMod(g, endorsement[1], mod), mod), mod);
}

hash_t Coin::hash() const {
    return Hash::hash(ZZToBytes(S), Hash::SHA1, string(), 
					  Hash::TYPE_PLAIN);
}
