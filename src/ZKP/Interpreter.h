
#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "Environment.h"
#include "ASTNode.h"
#include "../SigmaProof.h"

/*!
 * \brief This class will interpret instructions given by a program
 */

typedef pair<string,ZZ> cache_key_pair;

class Interpreter {

	public:
		Interpreter() {}

		Interpreter(const Interpreter &o) : env(o.env), tree(o.tree) {}

		/*! to load from the cache */
		Interpreter(pair<ASTNodePtr, Environment> &p) 
			: env(p.second), tree(p.first) {}

		virtual ~Interpreter() {} 

		/*! this will run all the visitors that are meant to be used 
		 * BEFORE user has given any numeric values (maybe groups though) */
		void check(const string &programName, input_map inputs, group_map grps);
		void check(const string &programName) 
					{ check(programName, input_map(), group_map()); }
		void check(const string &programName, input_map &inputs)
					{ check(programName, inputs, group_map()); }
		void check(const string &programName, group_map &groups)
					{ check(programName, input_map(), groups); }

		Environment getEnvironment() { return env; }

	protected:
		void cachePowers();

		cache_key_pair hashForCache(const string &fname, input_map i, group_map g);

		/*! used by the prover/verifier to compute any runtime values
		 * that are needed for the proof but haven't been handed in 
		 * by the user (so this will only be run AFTER the user has given
		 * in numeric values */
		virtual void computeIntermediateValues() = 0;

		Environment env;
		ASTNodePtr tree;
};

#endif /*_INTERPRETER_H_*/
