
#ifndef _INTERPRETERPROVER_H_
#define _INTERPRETERPROVER_H_

#include "Interpreter.h"
#include "../SigmaProof.h"

class InterpreterProver : public Interpreter {

	public:
		InterpreterProver() {}

		InterpreterProver(const InterpreterProver &o) 
			: Interpreter(o) {}

		/*! this constructor should be used for programs that are 
		 * pre-compiled */
		InterpreterProver(pair<ASTNodePtr, Environment> &p)
			: Interpreter(p) {}
	
		/*! this will run all the visitors that need to be run
		 * AFTER we have actual values */
		void compute(variable_map &v, group_map g = group_map());

		/*! returns all public variables (includes bases as well as
		 * commitments) */
		variable_map getPublicVariables();

		/*! to be called after running check and compute: returns a
		 * proof of the validity of the program given to check */
		SigmaProof computeProof(const hashalg_t &hashAlg);

	private:
		/*! this decomposes key values in decomposition maps into their
		 * four squares representation, then assigns values appropriately */
		void decompose();

		/*! this forms random exponents for the variables in randoms */
		void formRandomExponents();

		/*! this computes all values in the expressions and comsToCompute
		 * maps in the environment */
		void computeIntermediateValues();

		/*! for each exponent, creates a random exponent to be used by
		 * equality prover */
		variable_map makeRandomizedExponents();

		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int ver) {
			ar & base_object_nvp(Interpreter);
		}

};

#endif /*_INTERPRETERPROVER_H_*/
