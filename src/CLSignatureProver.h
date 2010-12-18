#ifndef CLSIGNATUREPROVER_H_
#define CLSIGNATUREPROVER_H_

#include "GroupRSA.h"
#include "ProgramMaker.h"
#include "ZKP/InterpreterProver.h"

class CLSignatureProver {
	public:
		/*! this constructor assumes all operations take place in the same
		 * group */
		CLSignatureProver(Ptr<const GroupRSA> publicKey, Ptr<const Group> comGroup,
						  int lx, const vector<ZZ> &coms, int numPrivates, 
						  int numPublics);

		CLSignatureProver(Ptr<const GroupRSA> pk, int lx, int numPrivates,
						  int numPublics, const gen_group_map &grps,
						  const vector<CommitmentInfo> &coms);
		
		/*! returns SigmaProof of valid signature */
		Ptr<ProofMessage>  getProof(const vector<ZZ>& sig, 
                                    const vector<SecretValue>& privates, 
                                    const vector<ZZ>& publics, 
                                    const hashalg_t &hashAlg);

	private:
		group_map g;
		input_map inputs;
		int numPrivates;
		int numPublics;
		variable_map v;	
		InterpreterProver prover;
};

#endif
