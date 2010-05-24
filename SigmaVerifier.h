/*
 * Verifier class for a sigma proof. This is a parent class for specific proofs.
 */

#ifndef SIGMAVERIFIER_H_
#define SIGMAVERIFIER_H_

#include "SigmaProof.h"
#include "Group.h"

class SigmaVerifier {
	public:
		/*! randomized Proof will only be set via constructor */
		SigmaVerifier(const var_map &rp, int st) : rProof(rp), stat(st) {}

		/*! copy constructor */
		SigmaVerifier(const SigmaVerifier &o)
			: challenge(o.challenge), rProof(o.rProof), stat(o.stat) {}

		/*! destructor */
		virtual ~SigmaVerifier() {}

		/*! will return a new challenge if canGenerateNewChallenge
		 * is true */
		ZZ getChallenge() const { return challenge; }

		virtual void setChallenge(const ZZ &challengeArg) = 0;
		ZZ getNewChallenge();

		virtual bool canGenerateNewChallenge() const = 0;

		/*! the response of SigmaProof will not be stored
		 * but used in the computation of the verification equation */
		virtual bool verify(var_map &response) = 0;

		/*! will get first-round message of the sigma proof */
		var_map getRandomizedProofs() const { return rProof; }

	protected:
		/*! to be used by getChallenge if canGenerateNewChallenge
		 * returns true */
		ZZ generateChallenge();
		ZZ challenge;

	private:
		var_map rProof;
		int stat;
};


#endif /* SIGMAVERIFIER_H_ */
