#ifndef _CLSIGNATUREVERIFIER_H_
#define _CLSIGNATUREVERIFIER_H_

#include "GroupRSA.h"
#include "ProgramMaker.h"
#include "ZKP/InterpreterVerifier.h"

class CLSignatureVerifier {
	public:
		CLSignatureVerifier(const GroupRSA *publicKey, 
							const Group* comGroup, int lx, const vector<ZZ> &c,
							int numPrivates, int numPublics);

		CLSignatureVerifier(const GroupRSA* pk, int lx, int numPrivates,
							int numPublics, const gen_group_map &groups,
							const vector<CommitmentInfo> &coms);
		
		/*! checks to see if a signature composed of A, e, and v is valid */
		bool verify(const ProofMessage* pm, int stat);

	private:
		group_map g;
		variable_map v;
		input_map inputs;
		int numPrivates;
		int numPublics;
		InterpreterVerifier verifier;
};

#endif /*_CLSIGNATUREVERIFIER_H_*/
