
#ifndef _VECIPHERTEXT_H_
#define _VECIPHERTEXT_H_

#include "SigmaProof.h"
#include "ZKP/Environment.h"

/*! \brief This class represents a verifiable escrow */

class VECiphertext {
	public:
		/*! text represents the ciphertext itself, com represents the
		 * commitment that was the basis of the proof, p represents the
		 * proof of correctness, rangeCs represents the commitments
		 * to each of the secret x_is contained in com */
		VECiphertext(const vector<ZZ> &text, const ZZ &com, 
					 const variable_map &publics, const SigmaProof &p)
			: ciphertext(text), commitment(com),
	   		  publicVariables(publics), proof(p) {}

		/*! copy constructor */
		VECiphertext(const VECiphertext &o)
			: ciphertext(o.ciphertext), commitment(o.commitment),
			  publicVariables(o.publicVariables), proof(o.proof) {}

		VECiphertext(const string& s) { loadString(*this, s); }

		VECiphertext() 
			: ciphertext(), commitment(0), publicVariables(), proof() {}

		// getters
		vector<ZZ> getCiphertext() const { return ciphertext; }
		ZZ getCommitment() const { return commitment; }
		SigmaProof getProof() const { return proof; }
		variable_map getPublics() const { return publicVariables; }

	private:
		vector<ZZ> ciphertext;
		ZZ commitment;
		variable_map publicVariables;
		SigmaProof proof;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(ciphertext)
				& auto_nvp(commitment)
				& make_nvp("publicVars", publicVariables)
				& make_nvp("proof", proof)
				;
		}
};

#endif /*_VECIPHERTEXT_H_*/
