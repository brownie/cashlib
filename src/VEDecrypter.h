
/**
 * this is a class to represent a decrypter for verifiable encryption
 * it will also be used as the arbiter for fair exchange
 *
 * because the decrypter is the TTP (and needs to know the secret key) the
 * setup (i.e. keygen) is also done in this class
 */

#ifndef _VEDECRYPTER_H_
#define _VEDECRYPTER_H_

#include "VEPublicKey.h"
#include "VESecretKey.h"

class VEDecrypter {
	public:
		/*! takes in some parameters and performs the key generator for
		 * verifiable encryption  - if no group is specified then an
		 * appropriate RSA group will be created in the course of the setup */
		VEDecrypter(const int m, const int modLength, const int stat);
		VEDecrypter(const int m, const int modLength, const int stat, 
					Ptr<GroupRSA> auxGroup);
		
		/*! also allow the setup to be done elsewhere and the decrypter to
		 * just be handed the public/secret keys */
		VEDecrypter(Ptr<VEPublicKey> p, Ptr<VESecretKey> s) : pk(p), sk(s) {}

		/*! copy constructor */
		VEDecrypter(const VEDecrypter& o) : pk(o.pk), sk(o.sk) {}

		// destructor
		~VEDecrypter() {}

		/*! given a ciphertext and the encryption label, returns the
		 * plaintext value if the ciphertext was formed correctly */
		vector<ZZ> decrypt(const vector<ZZ> &ciphertext, 
						   const string &label, const hashalg_t &hashAlg) const;

		// getters
		Ptr<VEPublicKey> getPK() { return pk; }
		Ptr<VESecretKey> getSK() { return sk; }
		
	private:
		/*! does the key generation for verifiable encryption (so sets the
		 * public and secret keys) */
		void setup(const int m, const int modLength, const int stat, 
				   Ptr<GroupRSA> auxGroup);
		
		/*! creates a group with the appropriate number of generators and
		 * modLength */
		Ptr<GroupRSA> createSecondGroup(const int m, const int modLength, 
                                        const int stat);
		Ptr<VEPublicKey> pk;
		Ptr<VESecretKey> sk;
};

#endif /*VEDECRYPTER_H_*/
