
#ifndef _INTERPRETERVERIFIER_H_
#define _INTERPRETERVERIFIER_H_

#include "Interpreter.h"
#include "../SigmaProof.h"

class InterpreterVerifier : public Interpreter {

	public:
		InterpreterVerifier() {}

		InterpreterVerifier(const InterpreterVerifier &o)
			: Interpreter(o) {}

		InterpreterVerifier(pair<ASTNodePtr, Environment> &p)
			: Interpreter(p) {}

		/*! takes in groups and variables; first merges its own
		 * variables with input from the prover and then creates
		 * the proper environment */
		void compute(variable_map &v, const variable_map &p, 
					 const variable_map &p2, group_map g);
		void compute(variable_map &v, const variable_map &p)
						{ compute(v, p, variable_map(), group_map()); }
		void compute(variable_map &v, const variable_map &p, group_map g)
						{ compute(v, p, variable_map(), g); }
		void compute(variable_map &v, const variable_map &p, 
					 const variable_map &p2) { compute(v, p, p2, group_map()); }

		/*! given a proof, calls on the equality verifier to check if
		 * the proof is valid or not */
		bool verify(const SigmaProof &proof, int stat);

	private:
		/*! this computes all the commitment values in the rangeComs map */
		void computeIntermediateValues();

		// indicates if commitments were formed correctly (for non-negative)
		bool badComs;
};


#endif /*_INTERPRETERVERIFIER_H_*/
