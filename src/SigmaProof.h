/**
 * A generic sigma proof. A sigma proof roughly corresponds to the prover's
 * end of a non-interactive zero-knowledge proof. In practice, it acts a lot
 * like a SigmaProver.
 *
 * A sigma proof holds 3 messages:
 * - Round 1 messages, which we call randomized proofs
 * - Round 2 messages, which we call challenges
 * - Round 3 messages, which we call responses
 */

#ifndef SIGMAPROOF_H_
#define SIGMAPROOF_H_

#include "CommonFunctions.h"
#include "Debug.h"
#include "Hash.h"

#ifndef USE_STD_MAP
#define USE_STD_MAP 1
#endif

#ifdef USE_STD_MAP
 #include <map>
 typedef map<string, ZZ> var_map;
#else
 #include <boost/unordered_map.hpp>
 typedef boost::unordered_map<string, ZZ> var_map;
#endif

class SigmaProof {
	public:
		/*! constructs a 3-round sigma proof */
		SigmaProof(const var_map &rproofs, const var_map &coms, 
				   const hashalg_t &ha)
			: randomizedProofs(rproofs), hashAlg(ha), commitments(coms) {}

		/*! copy constructor */
		SigmaProof(const SigmaProof &o);

		/*! dummy constructor for initializing empty class members */
		SigmaProof() {}

		/*! computes the challenge (second message) of the proof */
		ZZ computeChallenge() const;

		/*! sets our responses */
		void setResponse(var_map &rs) { responses = rs; }

		var_map getCommitments() const { return commitments; }

		/*! gets a vector of possible first-round messages */
		var_map getRandomizedProofs() const { return randomizedProofs; }

		/*! gets our third-round messages */
		var_map getResponses() const { return responses; }

		bool operator==(const SigmaProof& other) const {
		    return (randomizedProofs == other.randomizedProofs &&
			    	responses == other.responses &&
			    	hashAlg == other.hashAlg &&
                    commitments == other.commitments);
		}

		void dump() const;

		var_map randomizedProofs;
		var_map responses;
		hashalg_t hashAlg;
		var_map commitments;

		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(randomizedProofs)
				& auto_nvp(responses)
				& auto_nvp(hashAlg)
				& auto_nvp(commitments);
		}
};

#endif /*SIGMAPROOF_H_*/
