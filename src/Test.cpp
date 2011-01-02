/**
 * This is the test file where the main method is.
 * Keep the test methods organized so that it is easy to test different things.
 */

#include <iostream>  // I/O
#include <fstream>   // file I/O
#include <sstream>
#include <iomanip>   // format manipulation
#include <stdlib.h>
#include <string>
#include <assert.h>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <NTL/ZZ.h>

#include "Timer.h"
#include "base64.h"
#include "MultiExp.h"
#include "CashException.h"
#include "CommonFunctions.h"
#include "FourSquares.h"
#include "Group.h"
#include "GroupPrime.h"
#include "GroupRSA.h"
#include "Hash.h"
#include "Serialize.h"
#include "SigmaProof.h"
#include "SigmaProver.h"
#include "SigmaVerifier.h"

#include "ZKP/ASTTVisitor.h"
#include "ZKP/ZKPParser.hpp"
#include "ZKP/ZKPLexer.hpp"
#include "ZKP/InterpreterProver.h"
#include "ZKP/InterpreterVerifier.h"
#include "ZKP/ForExpander.h"
#include "ZKP/ConstantSub.h"
#include "ZKP/ConstantProp.h"
#include "ZKP/Printer.h"

#include "CLBlindRecipient.h"
#include "CLBlindIssuer.h"
#include "CLSignatureProver.h"
#include "CLSignatureVerifier.h"
#include "VEProver.h"
#include "VEVerifier.h"
#include "VEDecrypter.h"

#include "UserWithdrawTool.h"
#include "BankWithdrawTool.h"
#include "UserTool.h"
#include "BankTool.h"
#include "Coin.h"
#include "Arbiter.h"

#define MAX_TIMERS 20

using namespace std;

double* testGroupPrime();
double* testGroupRSA();
double* testSophie();
double* testClone();
double* testFor();
double* testConstSub();
double* testProofInteraction();
double* testCLProver();
double* testCLGroups();
double* testVE();
double* testWithdraw();
double* testCoin();
double* testBuy();
double* testBarter();
double* testBuyWithSetup();
double* testBuyResolution();
double* testBarterResolution();
double* testSerializeAbstract();
double* testMultiExp();

double* multiTest();

double* createParameters();
double* loadParameters();

struct test_desc_t { double* (*func)(); const char *desc; };
test_desc_t test_funcs[] = { 
	{ createParameters, "Generate and save groups and parameters"},
	{ loadParameters, "Load groups and parameters"},
	{ testGroupPrime, "GroupPrime" },
	{ testGroupRSA, "GroupRSA" },
	{ testSophie, "Sophie prime generation" },
	{ testClone, "Clone sub-trees" },
	{ testFor, "For expansion" },
	{ testConstSub, "Constant substitution" },
	{ testProofInteraction, "Various proofs" },
	{ testCLProver, "CL signatures" },
	{ testCLGroups, "CL signatures, part II" },
	{ testVE, "Verifiable encryption" },
	{ testWithdraw, "Withdraw" },
	{ testCoin, "Coin" },
	{ testBuy, "Buy" },
	{ testBarter, "Barter" },
	{ testBuyWithSetup, "Buy with setup" },
	{ testBuyResolution, "Buy resolution" },
	{ testBarterResolution, "Barter resolution" },
	{ testSerializeAbstract, "Test serialization of derived pointers"},
	{ testMultiExp, "Test multi-exp"},
	// add new tests here 
	{ multiTest, "Multi-tester" },
};

#define ARRAYLEN(x) (sizeof(x)/sizeof(x[0]))

double* runTest(int i) {
	if ((size_t)i < ARRAYLEN(test_funcs) && i >= 0)
		return test_funcs[i].func();
	else
		cout << "Invalid test number entered!" << endl;
	return 0;
}

void printTests() {
	cout << "-----------------------------" << endl;
	cout << "Please enter the number of the test you wish to try :" << endl;

	for (size_t i=0; i < ARRAYLEN(test_funcs); i++)
		cout << "  " << i << ": " << test_funcs[i].desc << endl;

	cout << "Press Ctrl-D to exit." << endl; // better exit
}

int main(int argc , const char* argv[]) {
	// XXX: want to incorporate a different random seed every time?
	SetSeed(to_ZZ(0));

	/*setprecision(5);
	resetiosflags(ios::scientific);
	setiosflags(ios::fixed);*/
	//cout.precision(5); // .00001 precision

	while(true) {
		printTests();

		int i;
        if (argc == 2) { i = atoi(argv[1]); }
		else { cin >> i; if(cin.eof()) return 0; }

		runTest(i);

		if (argc > 1) break;
	}
	return 0;
}

void dumpVarMap(const variable_map& v) {
	map<string, ZZ> m;
	pair<string, ZZ> p;
	copy(v.begin(), v.end(), inserter(m, m.begin()));
	foreach(p, m) cout << p.first << " = " << p.second << endl;
}

void dumpProofMessage(const ProofMessage& pm) {
	cout << "ProofMessage VARS" << endl;
	dumpVarMap(pm.vars);
	cout << "ProofMessage PUBLICS" << endl;
	dumpVarMap(pm.publics);
}

double* createParameters() {
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	int security = 80; // set to 80 or 128

	cout << "Enter desired security level (80 or 128): ";
	cin >> security;

	int lx = 2*security;
	int primeOrderLength = 0,
		primeModLength = 0,
		rsaModLength = 0;
	Hash::alg_t hashAlg;
	string encAlg = "aes-128-cbc";
	string sec = lexical_cast<string>(security);
	int m = 3;

	switch(security) {
	case 80:
		primeModLength = 1024; primeOrderLength = 160;
		rsaModLength = 1024;
		hashAlg = Hash::SHA1;
		break;
	case 128:
		primeModLength = 1024; primeOrderLength = 256;
		rsaModLength =  2048;
		hashAlg = Hash::SHA256;
		break;
	default:
		cerr << "ERROR: incorrect security parameter. Please set it to "
				"either 80 or 128" << endl;
		return timers;
	}

	// create Arbiter params
	//BankTool bankTool(("tool."+sec+".bank").c_str());
	//	VEPublicKey vepk, repk;
	//	loadFile(vepk, "public."+sec+".arbiter");
	//	loadFile(repk, "public.regular."+sec+".arbiter");
	
	startTimer();
	VEDecrypter ve(m, rsaModLength, security);
	saveFile(make_nvp("VEPublicKey", *ve.getPK()), "public."+sec+".arbiter");
	saveFile(make_nvp("VESecretKey", *ve.getSK()), "secret."+sec+".arbiter");
	timers[timer++] = printTimer(timer, "Arbiter public and secret keys "
										"created and saved");

	startTimer();
	m = 1024;
	VEDecrypter re(m, rsaModLength, security);
	saveFile(make_nvp("VEPublicKey", *re.getPK()), "public.regular."+sec+".arbiter");
	saveFile(make_nvp("VESecretKey", *re.getSK()), "secret.regular."+sec+".arbiter");
	timers[timer++] = printTimer(timer, "Arbiter public and secret keys (for "
										"non-verifiable encryption) created " 
			   							"and saved");

	// create BankTool (holds public/secret keys)
	startTimer();
	vector<int> coinDenominations;
	for (int i=1; i <= 1024; i <<= 1) coinDenominations.push_back(i);
	
	BankTool bankTool(security, lx, primeModLength, hashAlg, coinDenominations);
	saveFile(make_nvp("BankTool", bankTool), "tool."+sec+".bank");
	timers[timer++] = printTimer(timer, "Bank public and secret keys created "
										"and saved");

	// save BankParams from BankTool public params
	const BankParameters* bankParameters = bankTool.getBankParameters();
	saveFile(make_nvp("BankParameters", *bankParameters), "bank."+sec+".params");
	
	// save a new user
	startTimer();
	UserTool userTool(security, lx, bankParameters, 
					  *ve.getPK(), *re.getPK(), hashAlg);
	saveFile(make_nvp("UserTool",userTool), "tool."+sec+".user");
	timers[timer++] = printTimer(timer, "User created and saved");
	return timers;
}

double* loadParameters() {
	double* timers = new double[MAX_TIMERS];
	string sec = "";
	cout << "Enter desired security level (80 or 128): ";
	cin >> sec;
	if (!(sec == "80" || sec == "128")) {
		cerr << "ERROR: incorrect security parameter.  Please set it to "
				"either 80 or 128" << endl;
		return timers;
	}

	VEPublicKey vepk(("public."+sec+".arbiter").c_str());
	VEPublicKey pk(("public.regular."+sec+".arbiter").c_str());

	BankTool bankTool(("tool."+sec+".bank").c_str());
	BankParameters bp(("bank."+sec+".params").c_str());
	const BankParameters* params = &bp;

	cout << "Number of denominations is " << params->getDenominations().size()
		 << endl;

	const GroupPrime* cashGrp = params->getCashGroup();
	cout << "Number of generators for cash group is " << 
			cashGrp->getGenerators().size() << endl;

	cout << "generators for cash group are: " << endl;
	for (unsigned i = 0; i < cashGrp->getGenerators().size(); i++) {
		cout << i+1 << "-th generator is " << cashGrp->getGenerator(i) << endl;
	}

	cout << "generators for bank public key for denom 512 are:" << endl;
	const GroupRSA* bankPK = params->getBankKey(512);
	for (unsigned i = 0; i < bankPK->getGenerators().size(); i++) {
		cout << i+1 << "-th generator is " << bankPK->getGenerator(i) << endl;
	}
	return timers;
}

double* testGroupPrime() {
	double* timers = new double[MAX_TIMERS];
	GroupPrime gp("bank", 1024, 160, 80);

	cout << "Info for GroupPrime" << endl;
	gp.debug();

	cout << endl << "Info for copy of GroupPrime. Should be identical" << endl;
	GroupPrime gp2(gp);
	gp2.debug();

	// save to text archive
	cout << "Archive gp1:" << endl << saveString(gp) << endl;
	cout << "Archive gp2:" << endl << saveString(gp2);
	//assert(saveString(gp) == saveString(gp2)); // XXX doesn't hold
	return timers;
}

double* testGroupRSA() {
	double* timers = new double[MAX_TIMERS];
	GroupRSA gp("bank", 1024, 80);

	cout << "Info for GroupRSA" << endl;
	gp.debug();

	// XXX: same thing for saving/loading
	cout << endl << "Info for copy of GroupRSA. Should be identical" << endl;
	GroupRSA gp2(gp);
	gp2.debug();
	return timers;
}

double* multiTest() {
	double* timers = new double[MAX_TIMERS];
	printTests();
	int testNo, numRepeats = 20;
	cin >> testNo;
	if(cin.eof()) exit(0);

	// XXX: for now, don't store first test results
	double** alltimers = new double*[numRepeats-1];

	for (int i = 0; i < numRepeats; i++) {
		cout << "-----------------------------------------------------" << endl;
		if (i == 0)
			runTest(testNo);
		if (i != 0)
			alltimers[i-1] = runTest(testNo);
		cout << "-----------------------------------------------------" << endl;
    }

	double max[MAX_TIMERS];
	double min[MAX_TIMERS];
	double avg[MAX_TIMERS];

	for (int i = 0; i < MAX_TIMERS; i++) {
		max[i] = 0;
		min[i] = 999999999;
		avg[i] = 0;
    }

	for (int i = 0; i < numRepeats - 1; i++) {
		for (int j = 0; j < MAX_TIMERS; j++) {
			if (max[j] < alltimers[i][j])
				max[j] = alltimers[i][j];
			if (min[j] > alltimers[i][j])
				min[j] = alltimers[i][j];
			avg[j] += alltimers[i][j] / (numRepeats-1);
		}
    }

	cout << "Test Results\t\tMAX\t\tMIN\t\tAVG" << endl;
	for (int i = 0; i < MAX_TIMERS; i++) {
		cout << (i+1) << "th result:\t\t" << max[i] << "\t\t" << min[i] << "\t\t" << avg[i] << endl;
    }

	for (int i = 0; i < numRepeats - 1; i++) {
		delete[] alltimers[i];
	}
	delete[] alltimers;
	return timers;
}

double* testSophie() {
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	startTimer();
	ZZ p = GenGermainPrime_ZZ(326);
	timers[timer++] = printTimer(timer, "Generated safe prime");
	ZZ pp = 2*p + 1;
	cout << "p: " << p << endl
		 << "p': " << pp << endl;
	cout << "p prime: " << (ProbPrime(p) ? "yes" : "no") << endl;
	cout << "p' prime: " << (ProbPrime(pp) ? "yes" : "no") << endl;
	return timers;
}

double* testClone() {
	double* timers = new double[MAX_TIMERS];
	istringstream iss(string("x * a ^ b_i + -2"));
	ZKPLexer* lexer = new ZKPLexer(iss);
	ZKPParser* parser = new ZKPParser(*lexer);
	ASTExprPtr n = parser->expr();
	
	Printer print;
	cout << print.unparse(n) << endl;

	print.clear();
	ASTExprPtr nn = n->clone();
	cout << print.unparse(nn) << endl;
	return timers;
}

double* testFor() {
	double* timers = new double[MAX_TIMERS];
	istringstream iss(string("for(i, 1:3, &&, c_i := (g^x_i) * (h^r_i))"));
	ZKPLexer* lexer = new ZKPLexer(iss);
	ZKPParser* parser = new ZKPParser(*lexer);
	ASTNodePtr n = parser->spec();
	
	Printer print;
	cout << print.unparse(n) << endl;
	
	Environment e;
	ForExpander f(e);
	print.clear();
	n->visit(f);
	cout << print.unparse(n) << endl;
	return timers;
}

double* testConstSub() {
	double* timers = new double[MAX_TIMERS];
	istringstream iss(string("computation:  given:  group: G = <f, g[1:l+k],h> "
							"exponents in G: x[1:l], r[1:l]compute: random "
							"integer in [0,2^(modSize + stat)): vprime C := "
							"h^vprime * for(i, 1:l, *, g_i^x_i) proof: given: "
							"group: G = <f, g[1:l+k], h>  elements in G: c[1:l]" 
							" for(i, 1:l, commitment to x_i: c_i = g_i^x_i * "
							"h^r_i) exponents in G: x[l+1:k] integers: l_x "
							"prove knowledge of: exponents in G: x[1:l], "
							"r[1:l], vprime such that: for(i, 1:l, range: "
							"-(2^l_x - 1) <= x_i < 2^l_x) C =  h^vprime * "
							"for(i, 1:l, *, c_i * h^(-r_i))"));
	ZKPLexer* lexer = new ZKPLexer(iss);
	ZKPParser* parser = new ZKPParser(*lexer);
	ASTNodePtr n = parser->spec();
	
	Printer print;
	cout << print.unparse(n) << endl;
	
	input_map input;
	input["l"] = to_ZZ(3);
	input["k"] = to_ZZ(2);
	input["modSize"] = to_ZZ(5);
	input["stat"]=to_ZZ(10);
	input["l_x"] = 15;
	ConstantSub f(input);
	ConstantProp g(input);
	
	while(g.subAgain()||f.anotherPass()){
		f.reset();
		print.clear();
		n->visit(f);
		cout << print.unparse(n) << endl;
		g.reset();
		print.clear();
		n->visit(g);
		cout << print.unparse(n) << endl;
	}
	
	Environment e;
	ForExpander fe(e);
	
	n->visit(fe);
	print.clear();
	cout<<print.unparse(n)<<endl;
	return timers;
}

double* testProofInteraction() {
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	hashalg_t hashAlg = Hash::SHA1;
	int stat = 80;
	// load up our parameters
	BankTool bankTool("tool.80.bank");
	BankParameters bp("bank.80.params");
	const GroupPrime* cashG = bp.getCashGroup();
	// just get PK for an arbitrary denomination
	const GroupRSA* rangeG = bp.getBankKey(512);

	group_map pgrps;
	variable_map pvars;
	InterpreterProver p;
	string str;
	variable_map vvars;
	InterpreterVerifier v;
	group_map vgrps;

	startTimer();
	pgrps["G"] = cashG;
	p.check("ZKP/examples/dlr.txt", pgrps);
	timers[timer++] = printTimer(timer, "Prover checked DLR program");
	startTimer();
	p.compute(pvars);
	timers[timer++] = printTimer(timer, "Prover computed values for DLR program");
	startTimer();
	SigmaProof proof = p.computeProof(hashAlg);
	variable_map publics = proof.getCommitments();
	variable_map pv = p.getPublicVariables();
	timers[timer++] = printTimer(timer, "Prover computed proof for DLR program");
	cout << "DLR proof size: " << saveGZString(proof).size() << endl;

	vgrps["G"] = cashG;
	startTimer();
	v.check("ZKP/examples/dlr.txt", vgrps);
	timers[timer++] = printTimer(timer, "Verifier checked DLR program");
	startTimer();
	v.compute(vvars, publics, pv);
	timers[timer++] = printTimer(timer, "Verifier computed values for DLR "
										"program");
	startTimer();
	bool verified = v.verify(proof, stat);
	timers[timer++] = printTimer(timer, "Verifier verified proof for DLR program");

	if (verified)
		cout << "DLR proof verified successfully" << endl;
	else
		cout << "DLR proof failed to verify" << endl;
	cout << "-------------------------------------------" << endl;

	// now do multiplication
	pgrps.clear();
	pvars.clear();
	pgrps["G"] = cashG;
	startTimer();
	p.check("ZKP/examples/multiplication.txt", pgrps);
	timers[timer++] = printTimer(timer, "Prover checked multiplication program");
	startTimer();
	p.compute(pvars);
	timers[timer++] = printTimer(timer, "Prover computed values for "
										"multiplication program");
	startTimer();
	SigmaProof proof2 = p.computeProof(hashAlg);
	variable_map publics2 = proof2.getCommitments();
	variable_map pv2 = p.getPublicVariables();
	timers[timer++] = printTimer(timer, "Prover computed proof for "
										"multiplication program");
	cout << "Multiplication proof size: " << saveGZString(proof2).size() << endl;

	vgrps.clear();
	vvars.clear();
	vgrps["G"] = cashG;
	startTimer();
	v.check("ZKP/examples/multiplication.txt", vgrps);
	timers[timer++] = printTimer(timer, "Verifier checked multiplication program");
	startTimer();
	v.compute(vvars, publics2, pv2);
	timers[timer++] = printTimer(timer, "Verifier computed values for "
										"multiplication program");
	startTimer();
	bool verified2 = v.verify(proof2, stat);
	timers[timer++] = printTimer(timer, "Verifier verified proof for "
										"multiplication program");

	if (verified2)
		cout << "Multiplication proof verified successfully" << endl;
	else
		cout << "Multiplication proof failed to verify" << endl;
	cout << "-------------------------------------------" << endl;

	// now do range
	pgrps.clear();
	pvars.clear();
	pgrps["G"] = rangeG;
	pvars["W"] = to_ZZ(100);
	pvars["J"] = to_ZZ(51);
	startTimer();
	p.check("ZKP/examples/range.txt", pgrps);
	timers[timer++] = printTimer(timer, "Prover checked range program");
	startTimer();
	p.compute(pvars);
	timers[timer++] = printTimer(timer, "Prover computed values for range "
										"program");
	startTimer();
	SigmaProof proof3 = p.computeProof(hashAlg);
	variable_map publics3 = proof3.getCommitments();
	variable_map pv3 = p.getPublicVariables();
	timers[timer++] = printTimer(timer, "Prover computed proof for range program");
	cout << "Range proof size: " << saveGZString(proof3).size() << endl;

	vgrps.clear();
	vvars.clear();
	vgrps["G"] = rangeG;
	vvars["W"] = to_ZZ(100);
	startTimer();
	v.check("ZKP/examples/range.txt", vgrps);
	timers[timer++] = printTimer(timer, "Verifier checked range program");
	startTimer();
	v.compute(vvars, publics3, pv3);
	timers[timer++] = printTimer(timer, "Verifier computed values for range "
										"program");
	startTimer();
	bool verified3 = v.verify(proof3, stat);
	timers[timer++] = printTimer(timer, "Verifier verified proof for range "
										"program");

	if (verified3)
		cout << "Range proof verified successfully" << endl;
	else
		cout << "Range proof failed to verify" << endl;
	cout << "-------------------------------------------" << endl;
	return timers;
}

double* testCLProver(){
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	//set up security parameters	
	int stat = 80;	
	int lx = 2*stat;
	int RSALength = 1024;
	hashalg_t hashAlg = Hash::SHA1;
	
	int numPublics = 2;
	int numPrivates = 3;
	//create secret key
	//creating group adds f as a generator
	GroupRSA* sk = new GroupRSA("first", RSALength, stat);
	for (int i = 0; i < numPrivates + numPublics; i++) {
		sk->addNewGenerator();
	}
	//add final generator h
	sk->addNewGenerator();

	//create public key from copying secret key and clearing secrets
	GroupRSA* pk = new GroupRSA(*sk);
	pk->clearSecrets();

	//create random public messages
	//public message size does not matter
	vector<ZZ> publics;
	for(int x = 0; x<numPublics; x++)
		publics.push_back(RandomBits_ZZ(lx-1));
	
	//create random private messages and their commitments
	const BankParameters* bp = new BankParameters("bank.80.params");
	const GroupPrime* comGroup = bp->getCashGroup();
	startTimer();
	vector<pair<ZZ,ZZ> > secretExps;	
	vector<ZZ> coms;
	vector<ZZ> bases;
	bases.push_back(comGroup->getGenerator(1)); 
	bases.push_back(comGroup->getGenerator(2));
	ZZ mod = comGroup->getModulus();
	for (int i = 0; i < numPrivates; i++) {
		// form commitment to private variable
		vector<ZZ> exps;
		ZZ msg = RandomBits_ZZ(lx-1);
		ZZ random = pk->randomExponent();
		exps.push_back(msg);
		exps.push_back(random);
		ZZ com = MultiExp(bases, exps, mod);
		// push back commitment and push back pair of secrets
		coms.push_back(com);
		secretExps.push_back(make_pair(msg, random));
	}
	// blind recipient initiates obtaining CLSignature
	startTimer();
	CLBlindRecipient recip(pk, comGroup, lx, coms, numPrivates, numPublics);
	timers[timer++] = printTimer(timer, "Created CL recipient");
	startTimer();
	ProofMessage* cVprimeProof = recip.getC(secretExps, hashAlg);
	timers[timer++] = printTimer(timer, "Recipient sent proof of knowledge "
										"of values in commitments");
	cout << "Recipient proof size: " << saveGZString(*cVprimeProof).size() <<endl;
	ProofMessage pmsz; loadGZString(pmsz, saveGZString(*cVprimeProof));

	// blind issuer has access to the secret key	
	startTimer();
	CLBlindIssuer issu(sk, comGroup, lx, coms, numPrivates, numPublics);
	timers[timer++] = printTimer(timer, "Created CL issuer");
	
	startTimer();
	ZZ C = cVprimeProof->vars.at("C");
	ProofMessage* partialSigProof = issu.getPartialSignature(C, publics, 
															 *cVprimeProof,
															 stat, hashAlg);
	timers[timer++] = printTimer(timer, "Issuer verified recipient's proof "
										"and sent partial signature");
	cout << "Issuer proof size: " << saveGZString(*partialSigProof).size() <<endl;
	
	startTimer();
	bool partialSigVerified = recip.verifySig(*partialSigProof, stat);
	timers[timer++] = printTimer(timer, "Recipient verified partial signature");
	if (partialSigVerified)
		cout << "Obtaining the CL signature was successful" << endl;
	else
		cout << "Obtaining the CL signature failed" << endl;
	
	variable_map partialSig = partialSigProof->vars;
	vector<ZZ> partial;
	partial.push_back(partialSig.at("A"));
	partial.push_back(partialSig.at("e"));
	partial.push_back(partialSig.at("vdoubleprime"));

	//the recipient sets up the signature from the partial one received from
	//the bank
	startTimer();
	vector<ZZ> signature = recip.createSig(partial);
	timers[timer++] = printTimer(timer, "Recipient used partial signature "
										"to create full one");

	// recipient then proves they have the signature to a 
	// third party who verifies
	startTimer();
	CLSignatureProver prover(pk, comGroup, lx, coms, numPrivates, numPublics);
	timers[timer++] = printTimer(timer, "Created CL prover");
	startTimer();
	ProofMessage* publicProof = prover.getProof(signature, secretExps, 
												publics, hashAlg);
	timers[timer++] = printTimer(timer, "Prover sent proof of possession of "
										"CL signature");
	cout << "Possession proof size: " <<saveGZString(*publicProof).size()<<endl;

	startTimer();
	CLSignatureVerifier verifier(pk, comGroup, lx, coms, numPrivates, 
								 numPublics);
	timers[timer++] = printTimer(timer, "Created CL verifier");
	startTimer();
	bool verified = verifier.verify(publicProof, stat);
	timers[timer++] = printTimer(timer, "Verifier checked prover's proof");
	if(verified){
		cout<<"Proving possession of a CL signature succeeded"<<endl;
	}  else {
		cout<< "CL signature verification failed"<<endl;
	}
	return timers;
}

double* testCLGroups() {
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	int stat = 80, modLength = 1024, numPublics = 1, numPrivates = 3;
	hashalg_t hashAlg = Hash::SHA1;

	// want to have a different group for each commitment
	GroupPrime* group1 = new GroupPrime("cash", modLength, 2*stat, stat);
	group1->addNewGenerator();
	GroupPrime* group2 = new GroupPrime("cash", modLength, 2*stat, stat);
	group2->addNewGenerator();
	GroupPrime* group3 = new GroupPrime("cash", modLength, 2*stat, stat);
	group3->addNewGenerator();
	vector<Group*> grps;
	grps.push_back(group1);
	grps.push_back(group2);
	grps.push_back(group3);

	// also need RSA group for CL signature stuff
	GroupRSA* pk = new GroupRSA("bank", modLength, stat); // f
	pk->addNewGenerator(); // g_1
	pk->addNewGenerator(); // g_2
	pk->addNewGenerator(); // g_3
	pk->addNewGenerator(); // g_4
	pk->addNewGenerator(); // h

	GroupRSA* sk = new GroupRSA(*pk);
	pk->clearSecrets();

	// now set up commitments and such
	vector<pair<ZZ,ZZ> > secretExps;
	for (int i = 0; i < numPrivates; i++) {
		ZZ x = RandomBits_ZZ(2*stat-1);
		ZZ r = RandomBits_ZZ(2*stat-1);
		secretExps.push_back(make_pair(x,r));
	}

	vector<ZZ> publics;
	for (int i = 0; i < numPublics; i++) {
		publics.push_back(RandomBits_ZZ(2*stat-1));
	}

	gen_group_map groups;
	vector<CommitmentInfo> coms;
	for (unsigned i = 0; i < grps.size(); i++) {
		string index = lexical_cast<string>(i+1);
		vector<ZZ> bases = grps[i]->getGenerators();
		vector<ZZ> exps;
		exps.push_back(secretExps[i].first);
		exps.push_back(secretExps[i].second);
		ZZ com = MultiExp(bases, exps, grps[i]->getModulus());

		vector<string> genNames;
		genNames.push_back("g"+index);
		genNames.push_back("h"+index);
		string grpName = "G"+index;
		CommitmentInfo info(com, genNames, grpName);

		groups[grpName] = make_pair(grps[i], genNames);
		coms.push_back(info);
	}

	startTimer();
	CLBlindRecipient recipient(pk, 2*stat, numPrivates, numPublics,
							   groups, coms);
	timers[timer++] = printTimer(timer, "CLBlindRecipient created");

	startTimer();
	CLBlindIssuer issuer(sk, 2*stat, numPrivates, numPublics, groups, coms);
	timers[timer++] = printTimer(timer, "CLBlindIssuer created");

	startTimer();
	ProofMessage* initial = recipient.getC(secretExps, hashAlg);
	timers[timer++] = printTimer(timer, "Initial proof from recipient created");
	ZZ C = initial->vars.at("C");

	startTimer();
	ProofMessage* bankProof = issuer.getPartialSignature(C, publics, *initial, 
														 stat, hashAlg);
	timers[timer++] = printTimer(timer, "Issuer's proof created");

	startTimer();
	bool verified = recipient.verifySig(*bankProof, stat);
	timers[timer++] = printTimer(timer, "Recipient verified issuer's proof");

	if (verified)
		cout << "Obtaining the CL signature was successful" << endl;
	else
		cout << "Obtaining the CL signature failed" << endl;

	vector<ZZ> partialSig;
	partialSig.push_back(bankProof->vars.at("A"));
	partialSig.push_back(bankProof->vars.at("e"));
	partialSig.push_back(bankProof->vars.at("vdoubleprime"));
	vector<ZZ> sig = recipient.createSig(partialSig);

	startTimer();
	CLSignatureProver prover(pk, 2*stat, numPrivates, numPublics,
							 groups, coms);
	timers[timer++] = printTimer(timer, "CL signature prover created");

	startTimer();
	ProofMessage* sigProof = prover.getProof(sig, secretExps, publics,
											 hashAlg);
	timers[timer++] = printTimer(timer, "Proof of knowledge of CL signature "
										"created");

	startTimer();
	CLSignatureVerifier verifier(pk, 2*stat, numPrivates, numPublics,
								 groups, coms);
	timers[timer++] = printTimer(timer, "CL signature verifier created");

	startTimer();
	bool sigVerified = verifier.verify(sigProof, stat);
	timers[timer++] = printTimer(timer, "Verifier verified the prover's PoK");

	if (sigVerified)
		cout << "Proving possession of a CL signature was successful" << endl;
	else
		cout << "Proving possession of a CL signature failed" << endl;
	return timers;
}

double* testVE() {
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	int stat = 80, modLength = 1024, m = 3;
	hashalg_t hashAlg = Hash::SHA1;

	// later will just save and load, but for now make keys from scratch
	VEDecrypter decrypter(m, modLength, stat);
	VEPublicKey* pk = decrypter.getPK();

	// set up the prover
	VEProver prover(pk);

	// need all commitment values and such
	// XXX: need test where this group can be of any form
	GroupRSA* rsaGroup = new GroupRSA("bank", modLength, stat); // f_3
	rsaGroup->addNewGenerator(); // gprime
	rsaGroup->addNewGenerator(); // hprime
	rsaGroup->addNewGenerator(); // f_1
	rsaGroup->addNewGenerator(); // f_2
	ZZ N = pk->getN();
	vector<ZZ> exponents;
	exponents.push_back(RandomBnd(N/2)); // x_1
	exponents.push_back(RandomBnd(N/2)); // x_2
	exponents.push_back(RandomBnd(N/2)); // x_3
	
	vector<ZZ> bases;
   	bases.push_back(rsaGroup->getGenerator(3));
   	bases.push_back(rsaGroup->getGenerator(4));
   	bases.push_back(rsaGroup->getGenerator(0));
	// first just do normal verifiable encryption 
	ZZ com = MultiExp(bases, exponents, rsaGroup->getModulus()); // X
	startTimer();
	VECiphertext ciphertext = prover.verifiableEncrypt(com, exponents, rsaGroup,
													   "sarah", hashAlg, stat);
	timers[timer++] = printTimer(timer, "First (normal) verifiable "
										"encryption completed");
	cout << "Escrow size: " << saveGZString(ciphertext).size() << endl;

	// also try it out with -x_1, x_2, x_3 (so still okay range though)
	vector<ZZ> negExps = exponents;
	ZZ neg;
	mpz_neg(MPZ(neg), MPZ(exponents[0]));
	negExps[0] = neg;

	ZZ negCom = MultiExp(bases, negExps, rsaGroup->getModulus());
	startTimer();
	VECiphertext negC = prover.verifiableEncrypt(negCom, negExps, rsaGroup,
												 "sarah", hashAlg, stat);	
	timers[timer++] = printTimer(timer, "Second (negative exponent) "
										"verifiable encryption completed");

	// also try it out with one of the values too big
	vector<ZZ> bigExps = negExps;
	ZZ bigX = RandomBnd(N);
	while (bigX < N/2) {
		bigX = RandomBnd(N);
	}
	bigExps[2] = bigX;
	ZZ bigCom = MultiExp(bases, bigExps, rsaGroup->getModulus());
	startTimer();
	VECiphertext bigC = prover.verifiableEncrypt(bigCom, bigExps, rsaGroup,
		   										 "sarah", hashAlg, stat);	
	timers[timer++] = printTimer(timer, "Third (too big exponent) verifiable "
										"encryption completed");

	// also try it with a prime-order group
	GroupPrime* primeGroup = new GroupPrime("bank", modLength, 2*stat, stat);
	primeGroup->addNewGenerator();
	primeGroup->addNewGenerator();
	primeGroup->addNewGenerator();
	primeGroup->addNewGenerator();

	// use same exponents as in first test
	vector<ZZ> primeBases;
   	primeBases.push_back(primeGroup->getGenerator(3));
   	primeBases.push_back(primeGroup->getGenerator(4));
   	primeBases.push_back(primeGroup->getGenerator(0));
	ZZ primeCom = MultiExp(primeBases, exponents, primeGroup->getModulus());
	startTimer();
	VECiphertext primeC = prover.verifiableEncrypt(primeCom, exponents, 
												   primeGroup, "sarah",
											   	   hashAlg, stat);
	timers[timer++] = printTimer(timer, "Fourth (prime-order group) "
										"verifiable encryption completed");	

	// ------------ VERIFICATIONS -----------------------------

	VEVerifier verifier(pk);
	startTimer();
	bool verified = verifier.verify(ciphertext, com, rsaGroup, "sarah", 
									hashAlg, stat);
	timers[timer++] = printTimer(timer, "First (normal) verifiable "
										"encryption verified");
	if (verified)
		cout << "Verifiable encryption worked successfully!" << endl;
	else
		cout << "Proof for verifiable encryption failed to verify" << endl;

	startTimer();
	bool negV = verifier.verify(negC, negCom, rsaGroup, "sarah", hashAlg, stat);
	timers[timer++] = printTimer(timer, "Second (negative exponent) "
										"verifiable encryption verified");
	if (negV)
		cout << "Second verifiable encryption (with negative exponent) worked" 
			 << endl;
	else
		cout << "Second verifiable encryption (with negative exponent) failed" 
			 << endl;

	startTimer();
	bool bigV = verifier.verify(bigC, bigCom, rsaGroup, "sarah", hashAlg, stat);
	timers[timer++] = printTimer(timer, "Third (too big exponent) verifiable "
										"encryption verified");
	if (bigV)
		cout << "Third verifiable encryption (with too big exponent) passed "
				"but wasn't supposed to!" << endl;
	else
		cout << "Third verifiable encryption (with too big exponent) failed "
				"and should have" << endl;

	startTimer();
	bool primeV = verifier.verify(primeC, primeCom, primeGroup, "sarah",
								  hashAlg, stat);
	timers[timer++] = printTimer(timer, "Fourth (prime-order group) verifiable "
										"encryption verified");
	if (primeV)
		cout << "Fourth verifiable encryption (with a prime-order group) passed"
			 << endl;
	else
		cout << "Fourth verifiable encryption (with a prime-order group) failed"
		     << endl;
	
	// also try giving the verifier the wrong group, and the wrong commitment
	startTimer();
	bool badGV = verifier.verify(ciphertext, com, primeGroup, "sarah",
								 hashAlg, stat);
	timers[timer++] = printTimer(timer, "Verifiable encryption with bad group "
										"verified");
	if (badGV)
		cout << "Verifiable encryption with bad group verified (but shouldn't "
				"have)" << endl;
	else
		cout << "Verifiable encryption with bad group failed (and should have)"
			 << endl;

	startTimer();
	bool badCV = verifier.verify(ciphertext, negCom, rsaGroup, "sarah",
								 hashAlg, stat);
	timers[timer++] = printTimer(timer, "Verifiable encryption with wrong "
										"commitment verified");
	if (badCV)
		cout << "Verifiable encryption with bad commitment verified (but "
				"shouldn't have)" << endl;
	else
		cout << "Verifiable encryption with bad commitment failed to verify "
				"(and should have)" << endl;
	return timers;
}

double* testWithdraw() {
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	hashalg_t hashAlg = Hash::SHA1;
	int walletSize = 100, coinDenom = 512;
	vector<int> denoms;
	denoms.push_back(1);
	denoms.push_back(2);
	denoms.push_back(4);
	denoms.push_back(8);
	denoms.push_back(16);
	denoms.push_back(32);
	denoms.push_back(64);
	denoms.push_back(128);
	denoms.push_back(256);
	denoms.push_back(512);
	denoms.push_back(1024);

	int stat=80;
	string statName = lexical_cast<string>(stat);
	
	// load bank and user from file
	BankTool bankTool("tool.80.bank");
	const BankParameters* params = new BankParameters("bank.80.params");
	UserTool userTool("tool.80.user", params, "public.80.arbiter",
					  "public.regular.80.arbiter");
/*
	startTimer();
	BankTool bankTool(stat, lx, modLen, hashAlg, denoms);
	timers[timer++] = printTimer(timer, "BankTool created");
	const BankParameters *params = bankTool.getBankParameters();

	// this is PK used for verifiable encryption
	startTimer();
	VEDecrypter decrypter(m, modLen, stat);
	VEPublicKey vepk = *decrypter.getPK();
	VESecretKey vesk = *decrypter.getSK();
	timers[timer++] = printTimer(timer, "Arbiter public and secret keys created");
	
	// this is PK used for regular encryption
	startTimer();
	VEDecrypter regularDecrypter(m, modLen, stat);
	VEPublicKey pk = *regularDecrypter.getPK();
	VESecretKey sk = *regularDecrypter.getSK();
	timers[timer++] = printTimer(timer, "Arbiter regular public and secret "
										"keys created");

	// set up user
	startTimer();
	UserTool userTool(stat, lx, params, vepk, pk, hashAlg);
	timers[timer++] = printTimer(timer, "UserTool created");
*/

	// step 1: user sends bank the public key and desired wallet size
	ZZ userPK = userTool.getPublicKey();
	// also uses tool for withdrawing
	UserWithdrawTool* uwTool = userTool.getWithdrawTool(walletSize, coinDenom);
	// also sends partial commitment (in partial commitment to s')
	startTimer();
	ZZ sPrimeCom = uwTool->createPartialCommitment();
	timers[timer++] = printTimer(timer, "User created partial commitment");
	cout << "Partial commitment size: " <<saveGZString(sPrimeCom).size()<<endl;

	// step 2: now bank needs withdraw tool as well
	BankWithdrawTool* bwTool = bankTool.getWithdrawTool(userPK, walletSize,
														coinDenom);
	// given commitment to s', computes full commitment to s = s' + r'
	startTimer();
	bwTool->computeFullCommitment(sPrimeCom);
	timers[timer++] = printTimer(timer, "Bank computed full commitment");
	// now bank will send r' back to user
	ZZ bankPart = bwTool->getBankContribution();
	cout << "Bank contribution size: " << saveGZString(bankPart).size() << endl;

	// step 3: now, the user sends bank a proof of identity and a proof
	// from the CL signature protocol
	startTimer();
	ProofMessage* idProof = uwTool->initiateSignature(bankPart);
	ProofMessage* clProof = uwTool->getCLProof();
	timers[timer++] = printTimer(timer, "User created proof of identity, as "
										"well as all commitments");
	cout << "ID proof size: " << saveGZString(*idProof).size() << endl;
	cout << "CL proof size: " << saveGZString(*clProof).size() << endl;

	// step 4: the bank will sign the user's message and send back the
	// signature
	// bank also needs to send a proof of knowledge of 1/e
	startTimer();
	ProofMessage* pm = bwTool->sign(idProof, clProof);
	timers[timer++] = printTimer(timer, "Bank created PoK of 1/e and partial "
										"signature");
	cout << "Bank proof size: " << saveGZString(*pm).size() << endl;

	// step 5: user verifies the bank's PoK; if it correct then it stores
	// the signature and uses it to get a wallet
	startTimer();
	vector<ZZ> partialSig = uwTool->verify(*pm);
	timers[timer++] = printTimer(timer, "User verified bank's proof");

	startTimer();
	Wallet wallet = uwTool->getWallet(partialSig);
	timers[timer++] = printTimer(timer, "User successfully withdrew a wallet");

	// now save the wallet to be used in later tests
	// also save parameters here (for now, at least)
	/*saveFile(make_nvp("bank", bankTool), "tool."+statName+".bank");
	saveFile(make_nvp("bank", *params), "bank."+statName+".params");
	saveFile(make_nvp("user", userTool), "tool."+statName+".user");

	saveFile(vepk, "public."+statName+".arbiter");
	saveFile(vesk, "secret."+statName+".arbiter");
	saveFile(pk, "public.regular."+statName+".arbiter");
	saveFile(sk, "secret.regular."+statName+".arbiter");
	*/
	saveFile(make_nvp("Wallet", wallet), ("wallet."+statName).c_str());

	// XXX: having some seg fault issues here...
	delete uwTool;
	delete bwTool;

	// also like to make sure that coin is valid here
	vector<ZZ> contractInfo;
	contractInfo.push_back(123456789);
	ZZ rVal = Hash::hash(contractInfo, hashAlg);

	startTimer();
	Coin coin = wallet.nextCoin(rVal);
	timers[timer++] = printTimer(timer, "Got a coin from the wallet");

	coin.unendorse();
	bool coinVerified = coin.verifyCoin();
	if (coinVerified)
		cout << "Coin successfully verified and we're done!" << endl;
	else
		cout << "Coin failed to verify" << endl;
	return timers;
}

double* testCoin() {
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	hashalg_t hashAlg = Hash::SHA1;

	startTimer();
	// load bank and user from file
	BankTool bankTool("tool.80.bank");
	const BankParameters* params = new BankParameters("bank.80.params");
	UserTool userTool("tool.80.user", params, "public.80.arbiter",
					  "public.regular.80.arbiter");
	// also load wallet 
	Wallet wallet("wallet.80", params);
	timers[timer++] = printTimer(timer, "Loaded params");

	// now want to spend a coin from the wallet
	// dummy contract
	vector<ZZ> contractInfo;
	contractInfo.push_back(12345);
	ZZ rVal = Hash::hash(contractInfo, hashAlg);

	startTimer();
	Coin coin = wallet.nextCoin(rVal);
	timers[timer++] = printTimer(timer, "Got coin from wallet");
	cout << "Coin size: " << saveGZString(coin).size() << endl;

	string coinstr = saveGZString(coin);
	startTimer();
	Coin coinGZ(coinstr, params);
	timers[timer++] = printTimer(timer, "Deserialized coin from binary");

	vector<ZZ> endorsement = coin.getEndorsement();
	//coin.unendorse();	
	// trying to verify an unendorsed coin
	//startTimer();
	//bool coinVerified = coin.verifyCoin();
	//timers[timer++] = printTimer(timer, "Checked unendorsed coin");
	//if (coinVerified)
	//	cout << "Coin is valid (verified successfully)" << endl;
	//else
	//	cout << "Coin verification failed" << endl;
	//coin.endorse(endorsement);
	startTimer();
	bool coinVerified2 = coin.verifyCoin();
	timers[timer++] = printTimer(timer, "Checked endorsed coin");
	if (coinVerified2)
		cout << "Endorsed coin is valid" << endl;
	else
		cout << "Endorsed coin failed to verify" << endl;

	// okay, now we want to deposit the coin
	startTimer();
	bool bankVerified = bankTool.verifyCoin(coin);
	timers[timer++] = printTimer(timer, "Bank finished verifying the coin");
	if (bankVerified)
		cout << "Bank successfully verified the coin" << endl;
	else
		cout << "Bank failed to verify the coin" << endl;

	// now try to deposit it twice
	startTimer();
	bool doubleSpent = bankTool.isCoinDoubleSpent(coin, coin);
	timers[timer++] = printTimer(timer, "Bank checked to see if coin is "
										"double spent");
	if (doubleSpent)
		cout << "Bad: bank thinks coin is double spent but it's not" << endl;
	else
		cout<<"Good: bank just thinks merchant is depositing coin twice"<<endl;

	// now let's double spend a coin
	vector<ZZ> contractInfo2;
	contractInfo2.push_back(12346);
	ZZ rVal2 = Hash::hash(contractInfo2, hashAlg);
	ZZ sameIndex = coin.getIndex();
	wallet.replaceCoin(sameIndex);

	startTimer();
	Coin coin2 = wallet.nextCoin(rVal2);
	timers[timer++] = printTimer(timer, "Withdrew the same coin from the wallet");
	startTimer();
	bool isDoubleSpent = bankTool.isCoinDoubleSpent(coin, coin2);
	timers[timer++] = printTimer(timer, "Bank checked to see if coin is double "
										"spent");
	if (isDoubleSpent)
		cout << "Good: bank thinks coin is double spent" << endl;
	else
		cout << "Bad: bank doesn't think coin is double spent" << endl;

	startTimer();
	ZZ spenderKey = bankTool.identifyDoubleSpender(coin, coin2);
	timers[timer++] = printTimer(timer, "Bank tried to identify double spender");
	if (spenderKey == userTool.getPublicKey())
		cout << "Good: bank correctly identified double spender" << endl;
	else 
		cout << "Bad: bank incorrectly identified double spender" << endl;
	return timers;
}

double* testBuy() {	
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	int timeoutLength = 60 * 60 * 24, timeoutTolerance = 60 * 60;
	int stat = 80;
	hashalg_t hashAlg = Hash::SHA1;
	cipher_t encAlg = "aes-128-ctr";

	const BankParameters* params = new BankParameters("bank.80.params");
	Wallet wallet("wallet.80", params);
	VEPublicKey vepk("public.80.arbiter");

	ZZ R = RandomBits_ZZ(params->getCashGroup()->getOrderLength());

	// just use random garbage for file
	Buffer* ptext = new Buffer(string("randomdata01234567890123456789"));
	Hash::hash_t ptHash = ptext->hash(hashAlg, string(), Hash::TYPE_PLAIN);

	// create buyer and seller objects
	Buyer buyer(timeoutLength, &vepk, stat);
	Seller seller(timeoutLength, timeoutTolerance, &vepk, stat);	

	// step 1: seller gives ciphertext to buyer
	startTimer();
	EncBuffer* ctext = seller.encrypt(ptext, encAlg);
	// XXX: serializing ciphertexts doesn't really work
	timers[timer++] = printTimer(timer, "Seller created ciphertext");

	// step 2: buyer creates contract and verifiable escrow 
	startTimer();
	BuyMessage* buyMessage = buyer.buy(&wallet, ctext, ptHash, R);
	timers[timer++] = printTimer(timer, "Buyer created buy message");

	cout << "Buy message size: " << saveGZString(*buyMessage).size() << endl;
	cout << " Coin size: " << saveGZString(buyMessage->getCoinPrime()).size()
		 << endl;
	cout << " Contract size: " << saveGZString(buyMessage->getContract()).size()
		 << endl;
	cout << " Escrow size: " << saveGZString(buyMessage->getEscrow()).size() 
		 << endl;

	// step 3: seller checks contract and escrow, returns key(s) if valid 
	startTimer();
	vector<string> key = seller.sell(buyMessage, R, ptHash);
	timers[timer++] = printTimer(timer, "Seller checked the buy message and "
										"sent decryption key");

	// step 4: buyer checks the key and returns the endorsement if valid
	startTimer();
	vector<ZZ> endorsement = buyer.pay(key);
	timers[timer++] = printTimer(timer, "Buyer decrypted the file and sent "
										"the endorsement");

	// step 5: seller endorses the coin
	startTimer();
	bool okay = seller.endorseCoin(endorsement);
	timers[timer++] = printTimer(timer, "Seller endorsed the coin and the "
										"transaction is complete");

	if (okay)
		cout << "Buy protocol ran successfully!" << endl;
	else
		cout << "Buy protocol failed" << endl;

	delete ptext;
	delete buyMessage;
	return timers;
}

double* testBarter() {	
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	int timeoutLength = 60 * 60 * 24, timeoutTolerance = 60 * 60;
	int stat = 80;
	hashalg_t hashAlg = Hash::SHA1;	
	cipher_t encAlg = "aes-128-ctr", signAlg = "DSA";
	int hashType = Hash::TYPE_PLAIN;
	
	const BankParameters* params = new BankParameters("bank.80.params");
	Wallet wallet("wallet.80", params);
	VEPublicKey vepk("public.80.arbiter");
	VEPublicKey pk("public.regular.80.arbiter");

	string trackerHashKey = vepk.getHashKey();
	ZZ R = RandomBits_ZZ(params->getCashGroup()->getOrderLength());

	// get random files for Alice and Bob
	char bufA[1024], bufB[1024];
	RAND_pseudo_bytes((unsigned char*) bufA, sizeof(bufA));
	RAND_pseudo_bytes((unsigned char*) bufB, sizeof(bufB));
	Buffer* aData = new Buffer(bufA, sizeof(bufA));
	Buffer* bData = new Buffer(bufB, sizeof(bufB));
	hash_t aHash = aData->hash(hashAlg, trackerHashKey, hashType);
	hash_t bHash = bData->hash(hashAlg, trackerHashKey, hashType);

	// want to generate a signing key 
	startTimer();
	Signature::Key* signKey = Signature::Key::generateKey(signAlg);
	timers[timer++] = printTimer(timer, "Signature key generated");

	// create initiator and responder objects
	FEInitiator alice(timeoutLength, &vepk, &pk, stat, signKey);
	FEResponder bob(timeoutLength, timeoutTolerance, &vepk, &pk, stat);

	// step 1: Alice sends Bob a setup message
	startTimer();
	FESetupMessage* setupMsg = alice.setup(&wallet, R, signAlg);
	timers[timer++] = printTimer(timer, "Alice created setup message");
	cout << "Setup size: " << saveGZString(*setupMsg).size() << endl;

	// step 2: Bob checks setup message and outputs his file ciphertext
	startTimer();
	bool setupOkay = bob.setup(setupMsg, R);
	if (setupOkay)
		cout << "The setup message was valid" << endl;
	else
		cout << "Verification of the setup message failed" << endl;
	EncBuffer* bCipher = bob.startRound(bData, encAlg);
	timers[timer++] = printTimer(timer, "Bob checked setup message and "
										"sent back his ciphertext");
	cout << "Bob ciphertext size: " << saveGZString(*bCipher).size() << endl;

	// step 3: Alice sends her ciphertext
	startTimer();
	EncBuffer* aCipher = alice.continueRound(aData, encAlg);
	timers[timer++] = printTimer(timer, "Alice sent back her own ciphertext");
	cout << "Alice ciphertext size: " << saveGZString(*aCipher).size() << endl;

	// step 4: Alice continues by preparing and sending a contract and
	// an escrow of her key (and her signature on it)
	startTimer();
	FEMessage* message = alice.barter(bCipher, bHash, aHash);
	timers[timer++] = printTimer(timer, "Alice sent the contract for bartering");
	cout << "Message size: " << saveGZString(*message).size() << endl;

	// step 5: Bob receives Alice's ciphertext and contract and checks 
	// the validity of the contract; if valid he sends his key
	startTimer();
	vector<string> bKey = bob.giveKeys(*message, aCipher, aHash, bHash);
	timers[timer++] = printTimer(timer, "Bob checked the contract and sent "
										"his decryption key");
	cout << "Bob key size: " << saveGZString(bKey).size() << endl;

	// step 6: Alice checks Bob's keys and sends back her own key
	startTimer();
	vector<string> aKey = alice.giveKeys(bKey);
	timers[timer++] = printTimer(timer, "Alice sent her decryption key");
	cout << "Alice key size: " << saveGZString(aKey).size() << endl;

	// step 7: Bob checks Alice's key
	startTimer();
	bool okay = bob.checkKey(aKey);
	timers[timer++] = printTimer(timer, "Bob checked Alice's key and the "
										"protocol is complete");

	if (okay)
		cout << "First barter protocol ran successfully!" << endl;
	else
		cout << "First barter protocol failed" << endl;

	// now run a second file exchange using the same setup
	alice.reset();
	bob.reset();

	// use new files
	Buffer* aData2 = new Buffer(string("thisismysuperraddataalice00"));
	Buffer* bData2 = new Buffer(string("thisismysuperraddatabob0000"));
	hash_t aHash2 = aData2->hash(hashAlg, trackerHashKey, hashType);
	hash_t bHash2 = bData2->hash(hashAlg, trackerHashKey, hashType);

	startTimer();
	EncBuffer* bCipher2 = bob.startRound(bData2, encAlg);
	timers[timer++] = printTimer(timer, "Bob sent his second ciphertext");

	startTimer();
	EncBuffer* aCipher2 = alice.continueRound(aData2, encAlg);
	timers[timer++] = printTimer(timer, "Alice sent her second ciphertext");

	startTimer();
	FEMessage* message2 = alice.barter(bCipher2, bHash2, aHash2);
	timers[timer++] = printTimer(timer, "Alice sent her message for the "
										"second set of files");

	startTimer();
	vector<string> bKey2 = bob.giveKeys(*message2, aCipher2, aHash2, bHash2);
	timers[timer++] = printTimer(timer, "Bob checked the message and sent "
										"his second key");

	startTimer();
	vector<string> aKey2 = alice.giveKeys(bKey2);
	timers[timer++] = printTimer(timer, "Alice sent her second key");

	startTimer();
	bool okay2 = bob.checkKey(aKey2);
	timers[timer++] = printTimer(timer, "Bob checked Alice's key and the "
										"second barter is complete");

	if (okay2)
		cout << "Bartering on the second set of files was successful!" << endl;
	else
		cout << "Bartering on the second set of files failed" << endl;

	delete aData;
	delete bData;
	delete setupMsg;
	delete message;
	delete aData2;
	delete bData2;
	delete signKey;
	delete message2;
	return timers;
}

double* testBuyWithSetup() {
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	int stat = 80;
	int timeoutLength = 60 * 60 * 24, timeoutTolerance = 60 * 60;
	hashalg_t hashAlg = Hash::SHA1;
	string signAlg = "DSA", encAlg = "aes-128-ctr";
	int hashType = Hash::TYPE_PLAIN;

	const BankParameters* params = new BankParameters("bank.80.params");
	Wallet wallet("wallet.80", params);
	VEPublicKey vepk("public.80.arbiter");
	VEPublicKey pk("public.regular.80.arbiter");

	string trackerHashKey = vepk.getHashKey();
	ZZ R = RandomBits_ZZ(params->getCashGroup()->getOrderLength());

	// get random files for Alice and Bob
	char bufB[1024];
	RAND_pseudo_bytes((unsigned char*) bufB, sizeof(bufB));
	Buffer* data = new Buffer(bufB, sizeof(bufB));
	hash_t hash = data->hash(hashAlg, trackerHashKey, hashType);

	// want to generate a signing key 
	startTimer();
	Signature::Key* signKey = Signature::Key::generateKey(signAlg);
	timers[timer++] = printTimer(timer, "Signature key generated");
	
	// create initiator and responder objects
	FEInitiator alice(timeoutLength, &vepk, &pk, stat, signKey);
	FEResponder bob(timeoutLength, timeoutTolerance, &vepk, &pk, stat);

	// step 1: Alice sends Bob a setup message
	startTimer();
	FESetupMessage* setupMsg = alice.setup(&wallet, R, signAlg);
	timers[timer++] = printTimer(timer, "Alice created setup message");
	cout << "Setup size: " << saveGZString(*setupMsg).size() << endl;

	// step 2: Bob checks setup message and outputs his ciphertext
	startTimer();
	bob.setup(setupMsg, R);
	EncBuffer* ctext = bob.startRound(data, encAlg);
	timers[timer++] = printTimer(timer, "Bob checked setup message and "
										"output ciphertext");
	cout << "Ciphertext size: " << saveGZString(*ctext).size() << endl;

	// step 3: Alice decides to buy
	startTimer();
	FEMessage* msg = alice.buy(ctext, hash);
	timers[timer++] = printTimer(timer, "Alice decided to buy and sent contract");
	cout << "Message size: " << saveGZString(*msg).size() << endl;

	// step 4: Bob checks contract and outputs keys
	startTimer();
	vector<string> key = bob.sell(*msg, hash);
	timers[timer++] = printTimer(timer, "Bob checked all contract info and "
										"output keys");
	cout << "Key size: " << saveGZString(key).size() << endl;

	// step 5: Alice checks Bob's keys and returns endorsement
	startTimer();
	vector<ZZ> endorsement = alice.pay(key);
	timers[timer++] = printTimer(timer, "Alice checked keys and output "
										"endorsement");
	cout << "Endorsement size: " << saveGZString(endorsement).size() << endl;

	// step 6: Bob endorses coin
	startTimer();
	bool done = bob.endorseCoin(endorsement);
	timers[timer++] = printTimer(timer, "Bob endorsed the coin, protocol is done");

	if (done)
		cout << "Buy with setup was successful!" << endl;
	else
		cout << "Buy with setup failed" << endl;
	return timers;
}

double* testBuyResolution()  {
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	int timeoutLength = 60 * 60 * 24, timeoutTolerance = 60 * 60;
	int stat = 80;
	hashalg_t hashAlg = Hash::SHA1;
	int hashType = Hash::TYPE_PLAIN;
	cipher_t encAlg = "aes-128-ctr";

	VEPublicKey vepk("public.80.arbiter");
	VESecretKey vesk("secret.80.arbiter");
	VEDecrypter veDecrypter(&vepk, &vesk);

	VEPublicKey pk("public.80.arbiter");
	VESecretKey sk("secret.80.arbiter");
	VEDecrypter decrypter(&pk, &sk);

	BankTool bankTool("tool.80.bank");
	const BankParameters* params = new BankParameters("bank.80.params");
	Wallet wallet("wallet.80", params);

	string trackerHashKey = vepk.getHashKey();
	ZZ R = RandomBits_ZZ(params->getCashGroup()->getOrderLength());
	
	char buf[1024];
	RAND_pseudo_bytes((unsigned char*) buf, sizeof(buf));
	Buffer* ptext = new Buffer(buf, sizeof(buf));
	hash_t ptHash = ptext->hash(hashAlg, trackerHashKey, hashType);
	
	// now let's create our buyer and seller objects
	Buyer buyer(timeoutLength, &vepk, stat);
	Seller seller(timeoutLength, timeoutTolerance, &vepk, stat);
	
	// step 1: seller gives ciphertext to buyer
	startTimer();
	EncBuffer* ctext = seller.encrypt(ptext, encAlg);
	timers[timer++] = printTimer(timer, "Seller created ciphertext");

	// step 2: buyer creates contract and verifiable escrow 
	startTimer();
	BuyMessage* buyMessage = buyer.buy(&wallet, ctext, ptHash, R);
	timers[timer++] = printTimer(timer, "Buyer created buy message");

	// step 3: seller checks contract and escrow, returns key(s) if valid 
	startTimer();
	vector<string> key = seller.sell(buyMessage, R, ptHash);
	timers[timer++] = printTimer(timer, "Seller checked the buy message "
										"and sent decryption key");

	// just run this so we can make sure key is valid
	buyer.pay(key);

	// so let's say arbiter needs to get involved
	Arbiter arbiter(&veDecrypter, &decrypter, hashAlg, timeoutTolerance);

	// step 1: seller sends over relevant info to act as request
	startTimer();
	ResolutionPair sellerReq = seller.resolveI();
	timers[timer++] = printTimer(timer, "Seller sent contract and key "
										"information to the arbiter");

	// step 2: the arbiter stores information, checks contract, and comes
	// up with a challenge
	startTimer();
	vector<unsigned> chal = arbiter.sellerResolveI(sellerReq);
	timers[timer++] = printTimer(timer, "The arbiter sent a challenge back "
										"to the seller");
	// XXX: this is only ever 0!
	cout << "challenge is: ";
	for (unsigned i = 0; i < chal.size(); i++) {
		cout << chal[i] << " ";
	}
	cout << endl;

	// step 3: the seller prepares a proof that his keys are valid 
	startTimer();
	MerkleProof* proof = seller.resolveII(chal);
	timers[timer++] = printTimer(timer, "The seller sent his proof back to "
										"the arbiter");

	// step 4: the arbiter checks the proof and decrypts the escrow to
	// get the buyer's endorsement (if the proof is valid)
	startTimer();
	vector<ZZ> endorsement = arbiter.sellerResolveII(proof);
	timers[timer++] = printTimer(timer, "The arbiter returned the buyer's "
										"endorsement");

	// step 5: finally, the seller tries to endorse the coin
	startTimer();
	bool okay = seller.endorseCoin(endorsement);
	timers[timer++] = printTimer(timer, "The seller attempted to endorse "
										"the coin");
	if (okay)
		cout << "The arbiter successfully resolved the problem!" << endl;
	else
		cout << "The arbiter failed to resolve the conflict" << endl;
	return timers;
}

double* testBarterResolution() {	
	double* timers = new double[MAX_TIMERS];
	int timer = 0;
	int timeoutLength = 60 * 60 * 24, timeoutTolerance = 60 * 60;
	int stat = 80;
	hashalg_t hashAlg = Hash::SHA1;	
	cipher_t encAlg = "aes-128-ctr", signAlg = "DSA";
	int hashType = Hash::TYPE_PLAIN;
	
	const BankParameters* params = new BankParameters("bank.80.params");
	Wallet wallet("wallet.80", params);

	VEPublicKey vepk("public.80.arbiter");
	VESecretKey vesk("secret.80.arbiter");
	VEDecrypter veDecrypter(&vepk, &vesk);

	VEPublicKey pk("public.regular.80.arbiter");
	VESecretKey sk("secret.regular.80.arbiter");
	VEDecrypter decrypter(&vepk, &vesk);

	string trackerHashKey = vepk.getHashKey();
	ZZ R = RandomBits_ZZ(params->getCashGroup()->getOrderLength());

	// get random files for Alice and Bob
	char bufA[1024], bufB[1024];
	RAND_pseudo_bytes((unsigned char*) bufA, sizeof(bufA));
	RAND_pseudo_bytes((unsigned char*) bufB, sizeof(bufB));
	Buffer* aData = new Buffer(bufA, sizeof(bufA));
	Buffer* bData = new Buffer(bufB, sizeof(bufB));
	hash_t aHash = aData->hash(hashAlg, trackerHashKey, hashType);
	hash_t bHash = bData->hash(hashAlg, trackerHashKey, hashType);

	// want to generate a signing key 
	startTimer();
	Signature::Key* signKey = Signature::Key::generateKey(signAlg);
	timers[timer++] = printTimer(timer, "Signature key generated");

	// create initiator and responder objects
	FEInitiator alice(timeoutLength, &vepk, &pk, stat, signKey);
	FEResponder bob(timeoutLength, timeoutTolerance, &vepk, &pk, stat);

	// step 1: Alice sends Bob a setup message
	startTimer();
	FESetupMessage* setupMsg = alice.setup(&wallet, R, signAlg);
	timers[timer++] = printTimer(timer, "Alice created setup message");
	
	// step 2: Bob checks setup message and outputs his file ciphertext
	startTimer();
	bool setupOkay = bob.setup(setupMsg, R);
	if (setupOkay)
		cout << "The setup message was valid" << endl;
	else
		cout << "Verification of the setup message failed" << endl;
	EncBuffer* bCipher = bob.startRound(bData, encAlg);
	timers[timer++] = printTimer(timer, "Bob checked setup message and sent "
										"back his ciphertext");

	// step 3: Alice sends her ciphertext
	startTimer();
	EncBuffer* aCipher = alice.continueRound(aData, encAlg);
	timers[timer++] = printTimer(timer, "Alice sent back her own ciphertext");

	// step 4: Alice continues by preparing and sending a contract and
	// an escrow of her key (and her signature on it)
	startTimer();
	FEMessage* message = alice.barter(bCipher, bHash, aHash);
	timers[timer++] = printTimer(timer, "Alice sent the contract for bartering");

	// step 5: Bob receives Alice's ciphertext and contract and checks 
	// the validity of the contract; if valid he sends his key
	startTimer();
	vector<string> bKey = bob.giveKeys(*message, aCipher, aHash, bHash);
	timers[timer++] = printTimer(timer, "Bob checked the contract and sent "
										"his decryption key");

	// just do this to make sure Bob's key is valid
	alice.giveKeys(bKey);

	// now let's say we need to get arbiter involved
	Arbiter arbiter(&veDecrypter, &decrypter, hashAlg, timeoutTolerance);

	// step 1: responder sends request to arbiter
	startTimer();
	FEResolutionMessage* req = bob.resolveI();
	timers[timer++] = printTimer(timer, "Bob sent resolution request to "
										"the arbiter");

	// step 2: arbiter sends challenge to responder
	startTimer();
	vector<unsigned> chal = arbiter.responderResolveI(req);
	timers[timer++] = printTimer(timer, "Arbiter sent challenge to Bob");

	// step 3: Bob sends proof that his keys are valid
	startTimer();
	MerkleProof* proof = bob.resolveII(chal);
	timers[timer++] = printTimer(timer, "Bob sent a proof that his keys "
										"were valid");

	// step 4: if the proof is valid, arbiter will give the initiator's
	// keys
	startTimer();
	vector<string> aKey = arbiter.responderResolveII(proof);
	timers[timer++] = printTimer(timer, "Arbiter send back Alice's key");

	bool keyOkay = bob.checkKey(aKey);
	if (keyOkay) {
		// want to make key bad to test the second stage as well
		vector<string> badKey;
		badKey.push_back(Ciphertext::generateKey(encAlg));
		aKey = badKey;
		cout << "First stage of barter resolution worked!" << endl;
	}
	else {
		// otherwise key was already bad so we can move on to second
		// stage without changing anything
		cout << "Barter resolution failed (key not valid)" << endl;
	}
	
	// step 5: if Alice's key wasn't valid, Bob needs to prove this to
	// the arbiter
	startTimer();
	MerkleProof* badKeyProof = bob.resolveIII(aKey);

	// step 6: finally, if Bob's proof (of Alice's bad key) is correct,
	// the arbiter will give him the endorsement
	arbiter.setKeys(aKey);
	startTimer();
	vector<ZZ> endorsement = arbiter.responderResolveIII(badKeyProof);
	timers[timer++] = printTimer(timer, "Arbiter returned the endorsement "
										"for Alice's coin");

	bool coinOkay = bob.resolveIV(endorsement);
	if (coinOkay)
		cout << "Final stage of barter resolution worked!" << endl;
	else
		cout << "Barter resolution failed (endorsement not valid)" << endl;
	return timers;
}

BOOST_CLASS_EXPORT(ASTNode)
BOOST_CLASS_EXPORT(ASTExpr)
BOOST_CLASS_EXPORT(ASTExprIdentifier)
BOOST_CLASS_EXPORT(ASTBinaryOp)
BOOST_CLASS_EXPORT(ASTPow)

double* testSerializeAbstract() {
	double* timers = new double[MAX_TIMERS];
	// test serializing base and derived pointers
	istringstream iss(string("g^x"));
	ZKPLexer* lexer = new ZKPLexer(iss);
	ZKPParser* parser = new ZKPParser(*lexer);
	ASTExprPtr n = parser->expr();
	
	cout << "type of expr: " << type_to_str(typeid(*n)) << endl;

	ASTPowPtr pn = dynamic_pointer_cast<ASTPow>(n);
	assert(pn);
	string ps = saveString(pn);

	string s = saveString(n);
	ASTExprPtr nn;
	loadString(nn, s);

	cout << "loaded type: " << type_to_str(typeid(*nn)) << endl;

	ZZ z = -1, zz = 0;
	saveXML(make_nvp("z",z), "z.xml");
	loadXML(make_nvp("z",zz), "z.xml");
	cout << "zz: " << zz << endl;
	return timers;
}

double* testMultiExp() {
	double* timers = new double[MAX_TIMERS];
	int timer = 0;

	//create random private messages and their commitments
	cout << "generating group ..." << endl;
	GroupPrime gp("bank", 1024, 160, 80);
	gp.addNewGenerator();
	gp.addNewGenerator();
	gp.addNewGenerator();

	vector<ZZ> bases;
	bases.push_back(gp.getGenerator(1)); 
	bases.push_back(gp.getGenerator(2));
	bases.push_back(gp.getGenerator(3)); 
	ZZ mod = gp.getModulus();

	size_t ROUNDS = 400;
	vector<vector<ZZ> > exps(ROUNDS);
	
	for (size_t r = 0; r < ROUNDS; r++) {
		vector<ZZ> row(bases.size());
		for (size_t i = 0; i < bases.size(); i++)
			row[i] = gp.randomExponent();
		exps[r] = row;
	}

	vector<ZZ> resA(ROUNDS), resB(ROUNDS);

	startTimer();
	for (size_t r=0; r<ROUNDS; r++)
		resA[r] = MultiExp(bases, exps[r], mod);
	timers[timer++] = printTimer(timer, "MultiExps");

	startTimer();
	for (size_t r=0; r<ROUNDS; r++)
		resB[r] = MulMod( MulMod( PowerMod(bases[0], exps[r][0], mod),
								  PowerMod(bases[1], exps[r][1], mod), mod),
						  PowerMod(bases[2], exps[r][2], mod), mod);
	timers[timer++] = printTimer(timer, "regular PowerMod/MulMod");

    for (size_t i=0; i<ROUNDS; i++) {
		if (resA[i] != resB[i]) {
			cout << "ERROR: result " << i << " doesn't match" << endl;
			cout << "resultA: " << resA[i] << endl;
			cout << "resultB: " << resB[i] << endl;
			break;
		}
    }

	return timers;
}
