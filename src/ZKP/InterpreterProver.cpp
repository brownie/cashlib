
#include "InterpreterProver.h"
#include "EqualityProver.h"
#include "BindGroupValues.h"
#include "ComputationVisitor.h"
#include "../CommonFunctions.h"
#include "../Timer.h"

#define DUMP_VARS 0

variable_map InterpreterProver::getPublicVariables() {
	variable_map publics;
	for (variable_map::const_iterator it = env.variables.begin(); 
								it != env.variables.end(); ++it) {
		// put anything that is public in the map, except generators
		if (env.privates.count(it->first) &&
			env.privates.at(it->first) == false && 
			env.generators.count(it->first) == 0) {
			publics[it->first] = it->second;
		}
	}
	return publics;
}

void InterpreterProver::compute(variable_map &vars, group_map grps) {
	if (!grps.empty()) {
		// add group information if this wasn't done at compile time
		env.groups = grps;
		env.variables = vars;
		env.groups[Environment::NO_GROUP] = 0;
		BindGroupValues groupBinder(env);
		groupBinder.apply(tree);
	}
	// if groups and generators are there, add the rest
	// XXX: should be more efficient way to do this
	for (variable_map::iterator it = vars.begin(); it != vars.end(); ++it){
		env.variables[it->first] = it->second;
	}
	// compute all values in compute block
	ComputationVisitor computer(env);
	// replace environment with the one from this visitor
	computer.apply(tree);
	// compute intermediate expressions and commitments
	computeIntermediateValues();
#if DUMP_VARS
	for (variable_map::iterator it = env.variables.begin();
								it != env.variables.end(); ++it) {
		cout << it->first << " = " << it->second << endl;
	}
	for (group_map::iterator it = env.groups.begin();
							 it != env.groups.end(); ++it) {
		if (it->first != Environment::NO_GROUP) {
			cout << "modulus for " << it->first << 
					" is " << it->second->getModulus() << endl;
			cout << "order for " << it->first << 
					" is " << it->second->getOrder() << endl;
		}
	}
#endif
}

void InterpreterProver::decompose() {
	for (decomp_map::iterator it = env.decompositions.begin();
							  it != env.decompositions.end(); ++it) {
		// get value of exponent, but it's possible that value isn't there 
		// yet (intermediate expressions haven't been evaluated)
		if (env.variables.count(it->first) == 0) {
			ASTExprPtr expr = env.expressions.at(it->first);
			env.variables[it->first] = expr->eval(env);
		}
		ZZ val = env.variables.at(it->first);
		vector<DecompNames> names = it->second;
		// now decompose exponent, or look it up in map
		if (!env.decompCache.count(val))
			env.decompCache[val] = CommonFunctions::decompose(val);
		const vector<ZZ>& fourSquares = env.decompCache.at(val);

		assert(names.size() == fourSquares.size() && names.size() == 4);
		for (int i = 0; i < 4; i++) {
			// map all names and such to the appropriate values
			env.variables[names[i].decomp] = fourSquares[i];
			env.variables[names[i].decompSquare] = power(fourSquares[i], 2);
		}
	}
}

void InterpreterProver::formRandomExponents() {
	for (unsigned i = 0; i < env.randoms.size(); i++) {
		const Group* grp = env.getGroup(env.randoms[i]);
		ZZ val = grp->randomExponent();
		env.variables[env.randoms[i]] = val;
	}
}

void InterpreterProver::computeIntermediateValues() {
	// first just do random variables (independent of others)
	formRandomExponents();
	// now decompose anything that needs it
	decompose();
	// next, compute intermediate expressions (that haven't been computed
	// in decompose)
	for (expr_map::iterator it = env.expressions.begin();
							it != env.expressions.end(); ++it) {
		if (env.variables.count(it->first) == 0)
			env.variables[it->first] = it->second->eval(env);
	}
	// important that intermediate expressions are done before commitments,
	// as some will probably be used in the commitments
	for (dlr_map::iterator it = env.comsToCompute.begin();
						   it != env.comsToCompute.end(); ++it) {
		env.variables[it->first] = it->second.computeValue(env);
	}
	// clear all the maps 
	env.randoms.clear();
	env.expressions.clear();
	env.decompositions.clear();
	env.comsToCompute.clear();
}

// ------------
// proving stuff

variable_map InterpreterProver::makeRandomizedExponents() {
	variable_map ret;
	// find a group which is type RSA, if none found just use first group
	const Group* groupForRandomness = env.groups.begin()->second;
	for (group_map::iterator it = env.groups.begin();
							 it != env.groups.end(); ++it) {
		// remember that 0 is in the map to indicate 'no group'
		if (it->first != Environment::NO_GROUP) {
			if (it->second->getType() == Group::TYPE_RSA) {
				groupForRandomness = it->second;
				break;
			}
		}
	}
	// need to make sure group is not 0 (i.e. no group)
	if (!groupForRandomness) {
			group_map::iterator it = env.groups.begin();
			it++;
			groupForRandomness = it->second;
	}
	assert(groupForRandomness);

	// now go through every exponent that will be used and make a
	// corresponding random exponent
	for (dlr_map::iterator it = env.descriptions.begin();
						   it != env.descriptions.end(); ++it) {
		DLRepresentation dlr = it->second;
		for (unsigned i = 0; i < dlr.exps.size(); i++) {
			ret[dlr.exps[i]->toString()] = groupForRandomness->randomExponent();
		}		
	}
	return ret;
}

SigmaProof InterpreterProver::computeProof(const hashalg_t &hashAlg) {
	variable_map randExps = makeRandomizedExponents();
	EqualityProver eq(env, randExps);
	return eq.getSigmaProof(hashAlg);
}
