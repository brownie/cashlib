
#ifndef _CLBLINDRECIPIENT_H_
#define _CLBLINDRECIPIENT_H_

#include "GroupRSA.h"
#include "ZKP/InterpreterProver.h"
#include "ProgramMaker.h"

class CLBlindRecipient {

	public:
		/*! this constructor assumes commitments are formed in e-cash group
		 * and is passed in the number of private and public variables and 
		 * a message length parameter, as well as the private commitments */ 
		CLBlindRecipient(const GroupRSA* pk, const Group* comGroup, int lx, 
						 const vector<ZZ> &coms, int numPrivates, 
						 int numPublics);

		/*! this constructor allows for commitments that have been formed in
		 * groups other than the PK group */
		CLBlindRecipient(const GroupRSA* pk, int lx, int numPrivates, 
						 int numPublics, const gen_group_map &groups,
						 const vector<CommitmentInfo> &coms);

		// XXX: right now we're assuming that each private message is 
		// committed to individually... we shouldn't be doing this, because
		// it is also possible to use one that is a commitment to all at once
		/*! returns the product C = h^v' * g_1^x_1 * ... * g_l^x_l, as 
		 * well as a proof that it was formed correctly */
		ProofMessage* getC(const vector<SecretValue> &privates,
						   const hashalg_t &hashAlg);
	
		/*! given a partial signature, check that it was formed
		 * correctly (i.e. that bank's PoK of 1/e verifies) */
		bool verifySig(const ProofMessage &pm, int stat);	

		/*! given a partial signature, creates the full signature (A, e, v) */
		vector<ZZ> createSig(const vector<ZZ> &partialSig);

	private:
		group_map g;
		variable_map v;
		input_map inputs;
		InterpreterProver prover;
		int numPrivates; 
		int numPublics;
};

#endif /*_CLBLINDRECIPIENT_H_*/
