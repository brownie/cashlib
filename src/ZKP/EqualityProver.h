
#ifndef _EQUALITYPROVER_H_
#define _EQUALITYPROVER_H_

#include "Environment.h"
#include "../SigmaProver.h"

class EqualityProver : public SigmaProver {

	public:
		/*! the constructor takes in an environment for proving, as well
		 * as the map r containing randomized exponents */
		EqualityProver(Environment &e, variable_map &r)
			: env(e), randExps(r) {}

		EqualityProver(const EqualityProver &o) 
			: env(o.env), randExps(o.randExps) {}

		~EqualityProver() {}

		variable_map getCommitments() { return computeCommitments(1); }

		variable_map randomizedProofs() { return computeCommitments(0); }

		variable_map respond(const ZZ &challenge);

		void sanityCheck();

	private:
		variable_map computeCommitments(bool indicator);

		const Environment &env;
		variable_map randExps;
};

#endif /*_EQUALITYPROVER_H_*/
