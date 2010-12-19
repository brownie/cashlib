
#include "InterpreterVerifier.h"
#include "EqualityVerifier.h"
#include "BindGroupValues.h"

void InterpreterVerifier::compute(variable_map &v, const variable_map &p, 
								  const variable_map &p2, group_map g) {
	// store all information in the environment
	if (!g.empty()) {
		env.groups = g;
		env.variables = v;
		env.groups[Environment::NO_GROUP] = Ptr<Group>();
		// run BindGroupValues
		BindGroupValues binder(env);
		binder.apply(tree);
	}
	// XXX: again, is there a more efficient way?
	for (variable_map::iterator it = v.begin(); it != v.end(); ++it) {
		env.variables[it->first] = it->second;
	}
	for (variable_map::const_iterator it = p.begin(); it != p.end(); ++it) {
		// want to store prover's commitments and public bases, but any
		// inputs given to the verifier take precedence
		if (env.variables.count(it->first) == 0) {
			env.variables[it->first] = it->second;
		}
	}
	// do same thing for optional additional inputs
	for (variable_map::const_iterator it = p2.begin(); it != p2.end(); ++it) {
		if (env.variables.count(it->first) == 0) {
			env.variables[it->first] = it->second;
		}
	}
	// now we just need to deal with commitments for any range proofs
	// (note: this needs to be run after we get values from prover) 
	computeIntermediateValues();
}

bool InterpreterVerifier::verify(const SigmaProof &proof, int stat) {
	// if some commitments were already wrong, no point in continuing
	if (badComs)
		return false;
	else {
		EqualityVerifier eq(proof.getRandomizedProofs(), env, stat);
		eq.setChallenge(proof.computeChallenge());
		variable_map res = proof.getResponses();
		return eq.verify(res);
	}
}

void InterpreterVerifier::computeIntermediateValues() {
	// form range commitments
	for (dlr_map::iterator it = env.rangeComs.begin();
						   it != env.rangeComs.end(); ++it) {
		env.variables[it->first] = it->second.computeValue(env);
	}
	// for any decompositions, want to check that c_x = product over c_xi2, so
	// that x = x_1^2 + x_2^2 + x_3^2 + x_4^2
	badComs = false;
	for (decomp_map::iterator it = env.decompositions.begin();
							  it != env.decompositions.end(); ++it) {
		ZZ squareProd = to_ZZ(1);
		vector<DecompNames> fourNames = it->second;
		ZZ mod = env.getGroup(fourNames[0].decompSquare)->getModulus();
		for (int i = 0; i < 4; i++) {
			string name = fourNames[i].decompSquare;
			squareProd = MulMod(squareProd, env.getCommitmentValue(name), mod);
		}
		// if this product isn't equal to the original c_x, immediately know
		// proof won't be correct because commitments are formed improperly
		if (squareProd != env.getCommitmentValue(it->first)) {
			cout << "Product of square commitments not equal to original "
				"commitment: " << endl;
			cout << squareProd << " != " << env.getCommitmentValue(it->first) 
				 << endl;
			badComs = true;
		}
	}
	env.rangeComs.clear();
	env.decompositions.clear();
}
