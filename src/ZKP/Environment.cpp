
#include "Environment.h"
#include "../CommonFunctions.h"
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH
//#define EXP_DEBUG 1

std::size_t hash_value(const ZZ& n) {
	std::ostringstream o; o << n;
	return boost::hash<std::string>()(o.str());
}

void Environment::clear() {
	variables.clear(); 
	generators.clear();
	groups.clear();
	varTypes.clear();
	exprTypes.clear();
	privates.clear();
	commitments.clear();
	discreteLogs.clear();
	descriptions.clear();
	decompositions.clear();
	decompCache.clear();
	randoms.clear();
	expressions.clear();
	comsToCompute.clear();
	rangeComs.clear();
	cache.reset();
	multiCache.reset();
}

void Environment::clearPrivates() {
	foreach(privacy_map::value_type p, privates)
		if (p.second) // private variable
			variables.erase(p.first);
}

const Group* Environment::getGroup(const string &varName) const { 
	try {
		return groups.at(varTypes.at(varName).group); 
	} catch (std::out_of_range& e) {
		throw CashException(CashException::CE_SIZE_ERROR,
							"No group for %s", varName.c_str());
	}
}		

DLRepresentation Environment::getCommitment(const string &varName) const { 
	try {
		return discreteLogs.at(commitments.at(varName)); 
	} catch (std::out_of_range& e) {
		throw CashException(CashException::CE_SIZE_ERROR,
							"No commitment for %s", varName.c_str());
	}
}

ZZ Environment::getCommitmentValue(const string &varName) const {
	try {
		return variables.at(commitments.at(varName));
	} catch (std::out_of_range& e) {
		throw CashException(CashException::CE_SIZE_ERROR,
							"No commitmentValue for %s", varName.c_str());
	}
}

void Environment::addRandomVariable(const string &name, VarInfo info) {
	varTypes[name] = info;
	randoms.push_back(name);
	// assume all random variables are private
	privates[name] = 1;
}

void Environment::addCommittedVariable(const string &name, 
									   const string &comName,
									   const DLRepresentation &dlr, 
									   VarInfo info) {
	varTypes[name] = info;
	// want to make committed variable private, commitment public
	privates[name] = 1;
	privates[comName] = 0;
	commitments[name] = comName;
	discreteLogs[comName] = dlr;
	// also want to give commitment information
	varTypes[comName] = VarInfo(info.group, VarInfo::ELEMENT);
	comsToCompute[comName] = dlr;
}

void Environment::addExpression(const string &name, ASTExprPtr expr,
								VarInfo info, bool isPrivate) {
	expressions[name] = expr;
	varTypes[name] = info;
	privates[name] = isPrivate;
}

ZZ Environment::modPow(const string &baseName, const ZZ &base, const ZZ &exp, 
					   const ZZ &mod) const {
#ifdef EXP_DEBUG
	cout << "Environment::modPow called on " << baseName << endl;
#endif
	if (cache && cache->contains(baseName)) {		
		if (exp >= 0)
			return cache->modPow(baseName, exp, mod);
		else
			return PowerMod(base, exp, mod);
	} else {
		return PowerMod(base, exp, mod);
	}
}

ZZ Environment::multiExp(const vector<string> &baseNames, const vector<ZZ> &bs, 
						 const vector<ZZ> &es, const ZZ &mod) const {
#ifdef EXP_DEBUG
	cout << "Environment::multiExp called on " << baseNames.size() << " bases" << endl;
#endif
	if (multiCache && multiCache->contains(baseNames)) {
		bool negative = false;
		for (unsigned i = 0; i < es.size(); i++) {
			if (es[i] < 0)
				negative = true;
		}
		if (!negative)
			return multiCache->modPow(baseNames, bs, es, mod);
		else
			return MultiExp(bs, es, mod);
	} else {
		return MultiExp(bs, es, mod);
	}
}
