
#include "DLRepresentation.h"
#include "MultiExp.h"
#include <assert.h>
#include "Environment.h"
#include "Printer.h"

DLRepresentation::DLRepresentation(const DLRepresentation &o)
	: left(o.left), group(o.group), bases(o.bases), exps(o.exps)
{
}

string DLRepresentation::toString() const {
	string toReturn = "";
	toReturn += left->toString() + " = ";
	
	assert(bases.size() == exps.size());
	for (unsigned i = 0; i < bases.size(); i++) {
		toReturn += "(" + bases[i]->toString() + ")^(" + 
					exps[i]->toString() + ")";
	}
	toReturn += " in " + group;
	return toReturn;
}

vector<string> DLRepresentation::getBaseNames() const {
	vector<string> names;
	for (unsigned i = 0; i < bases.size(); i++) {
		names.push_back(bases[i]->toString());
	}
	return names;
}

int DLRepresentation::randIndex(Environment &env) const { 
	assert(exps.size() == 2);
	return (env.commitments.count(exps[0]->toString()) == 0) ? 0 : 1; 
}

ZZ DLRepresentation::computeValue(Environment &env) const {
	assert(bases.size() == exps.size());
	ZZ mod = env.groups.at(group)->getModulus();
	vector<ZZ> bs, es;
	vector<string> baseNames;
	for (unsigned i = 0; i < bases.size(); i++) {
		ZZ b = bases[i]->eval(env), e = exps[i]->eval(env);
		bs.push_back(b); 
		es.push_back(e);
		baseNames.push_back(bases[i]->toString());
	}
	if (bases.size() == 1)
		return env.modPow(baseNames[0], bs[0], es[0], mod);
	else
		return env.multiExp(baseNames, bs, es, mod);
}
