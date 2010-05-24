
#include "EqualityProver.h"
#include <assert.h>
#include "MultiExp.h"

variable_map EqualityProver::computeCommitments(bool indicator) {

	variable_map commitmentValues;
	// XXX: do we want to do this for all discrete logs, or just for 
	// ones that are actually commitments?
	for (dlr_map::const_iterator it = env.descriptions.begin();
								 it != env.descriptions.end(); ++it) {
		DLRepresentation cd = it->second;
		ZZ mod = env.groups.at(cd.group)->getModulus();

		vector<ZZ> bases;
		vector<string> baseNames;
		vector<ZZ> exps;
		for(unsigned j = 0; j < cd.bases.size(); j++) {
			// want to add bases and exponents to vectors, then use multi-exp
			string baseName = cd.bases[j]->toString();
			string expName = cd.exps[j]->toString();

			baseNames.push_back(baseName);
			bases.push_back(env.variables.at(baseName));
			if(indicator) {
				// use exponents from commitment opening
				exps.push_back(env.variables.at(expName));
			}
			else {	
				// use exponents from randomized proof opening
				exps.push_back(randExps.at(expName));
			}
		}
		//ZZ result = MultiExp(basesVector, exponentsVector, mod);
		ZZ result = env.multiExp(baseNames, bases, exps, mod);
		commitmentValues[cd.toString()] = result;
	}
	return commitmentValues;
}

variable_map EqualityProver::respond(const ZZ &challenge) {
	variable_map response;
	for (variable_map::iterator it = randExps.begin();
								it != randExps.end(); ++it) {
		ZZ temp = it->second + challenge * env.variables.at(it->first);
		response[it->first] = temp;
	}
	return response;
}
