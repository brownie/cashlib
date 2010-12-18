
#ifndef _CLBLINDISSUER_H_
#define _CLBLINDISSUER_H_

#include "GroupRSA.h"
#include "ZKP/InterpreterVerifier.h"
#include "ProgramMaker.h"

class CLBlindIssuer {

	public:
		CLBlindIssuer(Ptr<const GroupRSA> sk, Ptr<const Group> comGroup, int lx, 
					  const vector<ZZ> &coms, int numPrivates, int numPublics);

		CLBlindIssuer(Ptr<const GroupRSA> sk, int lx, int numPrivates,
					  int numPublics, const gen_group_map &groups,
					  const vector<CommitmentInfo> &coms);
		
		CLBlindIssuer(const CLBlindIssuer &o);

		/*! returns a variable map with the components of a partial signature 
		 * A, e, and v'' as well as a proof that it was correctly formed */
		Ptr<ProofMessage> getPartialSignature(const ZZ &C, const vector<ZZ>& pubs, 
                                              const ProofMessage &pm, int stat,
                                              const hashalg_t &hashAlg);

	private:
		input_map inputs;
		group_map g;
		int numPrivates;
		int numPublics;
		variable_map v;
		InterpreterVerifier verifier;
};

#endif /*_CLBLINDISSUER_H_*/
