
#include "Interpreter.h"
#include <fstream>
#include "ZKPLexer.hpp"
#include "ZKPParser.hpp"
#include "UndefinedVariables.h"
#include "UnusedVariables.h"
#include "ExponentSub.h"
#include "TypeIdentifier.h"
#include "TypeChecker.h"
#include "CommitmentVisitor.h"
#include "DescribeRelations.h"
#include "ConstantSub.h"
#include "ConstantProp.h"
#include "ForExpander.h"
#include <boost/shared_ptr.hpp>
#include "Printer.h"
#include <assert.h>
#include "InterpreterCache.h"
#include "Timer.h"
#include "BindGroupValues.h"

#define UNUSED 0

void Interpreter::check(const string &programName, input_map inputs,
						group_map groups) {
	// first check if program has already been compiled, with the same groups
	// and inputs used
	cache_key_pair key = hashForCache(programName,inputs,groups);
	if (InterpreterCache::contains(key)) {
		CacheValue& val = InterpreterCache::get(key);
		// if it has, just store the values and we're done
		env = val.env;
		tree = val.tree;
		env.clearPrivates();
	} else {
		// need to start out with a fresh environment for each program
		env.clear();
		tree.reset();
	
		ifstream stream;
		stream.open(programName.c_str());
		if (!stream) {
			throw CashException(CashException::CE_PARSE_ERROR,
								"Cannot open the specified file");
		}
	
		ASTSpecPtr n;
		try {
			Ptr<ZKPLexer> lexer(new ZKPLexer(stream));
			Ptr<ZKPParser> parser(new ZKPParser(*lexer));
			n = parser->spec();
			tree = n;
		} catch(antlr::ANTLRException& e) {
			cout << "Compile error: " << type_to_str(typeid(e)) << ": " 
				 << e.toString() << endl;
			throw CashException(CashException::CE_PARSE_ERROR,
								"Cannot compile %s", programName.c_str());
		}
	
		if (n) {
			ConstantSub sub(inputs);
			ConstantProp prop(inputs);
			while(prop.subAgain()||sub.anotherPass()){
				sub.reset();
				n->visit(sub);
				prop.reset();
				n->visit(prop);
			}
	
			Environment e;
			ForExpander f(e);
			n->visit(f);
	
			// first find any undefined variables
			UndefinedVariables undef;
			undef.apply(n);
#if UNUSED
			if (n->getProof() != 0) { 
				// now check to see if any are unused
				UnusedVariables unused;
				unused.apply(n);
			}
#endif			
			// sub in exponents of '1' for any lone bases
			ExponentSub expSub(env);
			expSub.apply(n);
			
			// set the groups and privacy settings for all declared variables
			TypeIdentifier typer(env);
			typer.apply(n);
				
			// now need to check that groups match for all DLR stuff, and
			// assign types for variables that are being declared while being 
			// assigned (if the types match up)
			TypeChecker checker(env);
			checker.apply(n);
	
			// now map variables in commitments to their commmitment names
			// and commitment names to commitment forms
			CommitmentVisitor com(env);
			// replace environment with resulting one from this visitor
			com.apply(n);
	
			// finally, need to describe all relations!
			DescribeRelations describer(env);
			describer.apply(n);

			// if groups are there, first bind generator values and then cache 
			// powers for bases that are used multiple times
			if (!groups.empty()) {
				env.groups = groups;
				env.groups[Environment::NO_GROUP] = Ptr<Group>();
				BindGroupValues binder(env);
				binder.apply(n);
				//cachePowers();
			}
			// now want to store output in cache so we can load it up
			// again later if necessary
			InterpreterCache::store(key, tree, env);
		}
	}
}

// XXX let's automatically pick a sane value somehow (just using group
// order length for 160-bit prime order groups wasn't long enough)
#define POWERCACHE_MAX_EXPLEN 2048

void Interpreter::cachePowers() {
	env.cache = new_ptr<PowerCache>();
	env.multiCache = new_ptr<MultiExpCache>();

	for (dlr_map::iterator it = env.descriptions.begin();
						   it != env.descriptions.end(); ++it) {
		vector<string> baseNames;
		vector<ZZ> baseVals;
		DLRepresentation rep = it->second;
		Ptr<const Group> g = env.groups.at(rep.group);
		for (unsigned i = 0; i < rep.bases.size(); i++) {
			string name = rep.bases[i]->toString();
			baseNames.push_back(name);
			if (env.variables.count(name) > 0) 
				baseVals.push_back(env.variables.at(name));
		}
		if (baseNames.size() == baseVals.size()) {
			if (baseNames.size() == 1)
				env.cache->store(baseNames[0], baseVals[0], g->getModulus(),
								 POWERCACHE_MAX_EXPLEN, 5);
			else {
				unsigned width=0;
				switch (baseNames.size()) {
					case 2:	width = 4; break;
					case 3: width = 3; break;
					case 4: width = 2; break;
				}
				if (width)
					env.multiCache->store(baseNames, baseVals, g->getModulus(),
									  	  width, POWERCACHE_MAX_EXPLEN);
			}
		}
	}
}

cache_key_pair Interpreter::hashForCache(const string &fname, input_map i, 
										 group_map g) {
	
	vector<string> gNames;
	for (group_map::const_iterator it = g.begin(); it != g.end(); ++it) {
		gNames.push_back(it->first);
	}
	vector<string> iNames;
	for (input_map::const_iterator it = i.begin(); it != i.end(); ++it) {
		iNames.push_back(it->first);
	}
	sort(gNames.begin(),gNames.end());
	sort(iNames.begin(),iNames.end());
	vector<ZZ> hashInput;
	for (unsigned j = 0; j < gNames.size(); j++) {
		if (gNames[j] == Environment::NO_GROUP) {
			hashInput.push_back(to_ZZ(0));
		} else {
			hashInput.push_back(g[gNames[j]]->getModulus());
		}
	}
	for (unsigned j = 0; j < iNames.size(); j++) {
		hashInput.push_back(to_ZZ(i[iNames[j]]));
	}

	ZZ h = Hash::hash(hashInput,Hash::SHA1);
	return make_pair(fname,h);
}
