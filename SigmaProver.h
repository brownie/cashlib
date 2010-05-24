
#ifndef SIGMAPROVER_H_
#define SIGMAPROVER_H_

#include "SigmaProof.h"
#include "Group.h"

class SigmaProver {
	public:
		SigmaProver(){}
		SigmaProver(const SigmaProver &original){}
		
		virtual ~SigmaProver(){}

		virtual var_map getCommitments() = 0;
		virtual var_map randomizedProofs() = 0;

		/*! computes and returns response(s) according to challenge */
		virtual var_map respond(const ZZ &challenge) = 0;

		/*! calls randomizedProof and respond methods */
		SigmaProof getSigmaProof(const hashalg_t &hashAlg);
};

#endif /* SIGMAPROVER_H_ */
