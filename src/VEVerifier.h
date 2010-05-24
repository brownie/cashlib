
#ifndef _VEVERIFIER_H_
#define _VEVERIFIER_H_

#include "VEPublicKey.h"
#include "VECiphertext.h"

class VEVerifier {
	public:
		/*! constructor takes in the decrypter's public key */
		VEVerifier(const VEPublicKey *pk) : pk(pk) {}

		/*! copy constructor */
		VEVerifier(const VEVerifier &original) : pk(original.pk) {}

		/*! destructor */
		~VEVerifier() {}

		/*! given the output of a VEProver, checks to see that it is correct
		 * (i.e. accepts or rejects the proof that the value contained in the
		 * ciphertext corresponds to the opening of commitment x) */
		bool verify(const VECiphertext &text, const ZZ &x, const Group* grp, 
                    const string &label, const hashalg_t &hashAlg, int stat);

	private:
		const VEPublicKey* pk;
};

#endif /*_VEVERIFIER_H_*/
