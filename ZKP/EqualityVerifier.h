#ifndef _EQUALITYVERIFIER_H_
#define _EQUALITYVERIFIER_H_

#include "../SigmaVerifier.h"
#include "Environment.h"

class EqualityVerifier : public SigmaVerifier {
	public:
		EqualityVerifier(const variable_map &rProofsArg, Environment &e, 
						 int stat)
			: SigmaVerifier(rProofsArg, stat), env(e) {}

		/*! copy constructor */
		EqualityVerifier(const EqualityVerifier &o) 
			: SigmaVerifier(o), env(o.env) {}

		/*! destructor */
		virtual ~EqualityVerifier() {}

		/*! returns true iff the verifier has the appropriate
		 * commitments, group definitions, and randomized proofs */
		virtual bool canGenerateNewChallenge() const;

		/*! uses stored randomized proof and response */
		virtual bool verify(variable_map &response);

		/*! used only if canGenerateNewChallenge is true */
		virtual void setChallenge(const ZZ &c) { challenge = c; }

	private:
		const Environment &env;
};

#endif
