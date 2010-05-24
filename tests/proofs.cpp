#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE zkp
#include <boost/test/unit_test.hpp>

#include "ZKP/Environment.h"
#include "ZKP/InterpreterProver.h"
#include "ZKP/InterpreterVerifier.h"
#include "GroupPrime.h"
#include "GroupRSA.h"
#include "Timer.h"

#define ZKP_DIR "ZKP/examples/"

//
// unit tests for proving relations with Interpreter{Prover,Verifier} 
//
// names & objs from each "fixture" struct are usable & re-init'd
// for each of their associated test cases below

struct interpreter_env {
	Environment env;
	InterpreterProver prover;
	InterpreterVerifier verifier;

	bool prove(const string& fname) {
		BOOST_TEST_MESSAGE( "## Running proof test for " << fname );

		startTimer();
		prover.check(ZKP_DIR + fname); // throws compilation errs
		printTimer("prover.check");

		startTimer();
		prover.compute(env.groups, env.variables);
		printTimer("prover.compute");

		startTimer();
		SigmaProof proof = prover.computeProof();
		printTimer("prover.computeProof");
		
		verifier.check(ZKP_DIR + fname);
		// need to get the right inputs for verifier
		variable_map publics = prover.getPublicVariables();

		startTimer();
		verifier.compute(env.groups, publics);
		printTimer("verifier.compute");

		startTimer();
		bool ok = verifier.verifyProof(proof);
		printTimer("verifier.verifyProof");

		return ok;
	}
};

struct prime_2g_env : public interpreter_env {
	// fixture for prime-order group w/ two generators
	prime_2g_env() {
		int stat = 80;
		GroupPrime* G = new GroupPrime("bank", 1024, 2*stat, stat); // g
		G->addNewGenerator(); // h
		env.groups["G"] = G;	
	}
};

// simple tests using prime-order group G: <g, h> 
BOOST_FIXTURE_TEST_SUITE(interpreter, prime_2g_env)

BOOST_AUTO_TEST_CASE(simpleDLR) { BOOST_CHECK(prove("simpleDLR.txt")); }

BOOST_AUTO_TEST_CASE(dlr) { BOOST_CHECK(prove("dlr.txt")); }

BOOST_AUTO_TEST_CASE(badDLR) { 
	env.variables["J"] = to_ZZ(51);
	BOOST_CHECK(prove("badDLR.txt")); 
}

BOOST_AUTO_TEST_CASE(multiplication) {
	env.variables["a"] = to_ZZ(3);
	env.variables["b"] = to_ZZ(2);
	BOOST_CHECK(prove("multiplication.txt"));
}

BOOST_AUTO_TEST_SUITE_END()

// environment & test suite for e-cash

struct ecash_env : public interpreter_env {
	ecash_env() {
		int stat = 80;
		GroupPrime* cashG = new GroupPrime("bank", 1024, stat*2, stat); // init gen is f
		cashG->addNewGenerator(); // this is g
		cashG->addNewGenerator(); // this is h
		cashG->addNewGenerator(); // this is h1
		cashG->addNewGenerator(); // this is h2	
		
		GroupRSA* rangeG = new GroupRSA("first", 1024, stat); // first gen is g1
		rangeG->addNewGenerator(); // second gen is g2
		env.groups["rangeGroup"] = rangeG;
		env.groups["cashGroup"] = cashG;

		vector<ZZ> secrets;
		for (int i=3; i; --i) secrets.push_back(cashG->randomExponent());

		env.variables["s"] = secrets[0];
		env.variables["t"] = secrets[1];
		env.variables["sk_u"] = secrets[2];
		env.variables["J"] = to_ZZ(51);
		env.variables["W"] = to_ZZ(power(2, 6));
		env.variables["zero"] = to_ZZ(0);
	}
};

BOOST_FIXTURE_TEST_SUITE(ecash, ecash_env)

BOOST_AUTO_TEST_CASE(ecash) { 
	BOOST_CHECK(prove("ecash.txt")); 
}

BOOST_AUTO_TEST_SUITE_END()
