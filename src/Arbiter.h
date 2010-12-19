#ifndef _ARBITER_H_
#define _ARBITER_H_

#include "MerkleProof.h"
#include "VEDecrypter.h"
#include "BuyMessage.h"
#include "MerkleVerifier.h"
#include "FEResolutionMessage.h"

/*! \brief This class is for resolving any disputes that may arise in the 
 * course of a fair exchange protocol */

typedef pair<vector<string>, Ptr<BuyMessage> > ResolutionPair;

class Arbiter {
	public:	
		Arbiter(Ptr<const VEDecrypter> vD, Ptr<const VEDecrypter> rD,
				const hashalg_t &h, int t) 
			: verifiableDecrypter(vD), regularDecrypter(rD), hashAlg(h), 
			  timeoutTolerance(t) {}

		/*! resolutions for buyer/initiator are the same: retrieve the
		 * seller key(s) from the database if the id r exists as an entry */
		vector<string> buyerResolve(const ZZ &r);
		vector<string> initiatorResolve(const ZZ &r);

		// the following are resolutions for the seller
		/*! first check the validity of the message, then output a set of 
		 * challenges */
		vector<unsigned> sellerResolveI(const ResolutionPair &keyMessagePair);

		/*! if the proof verifies, output the buyer's endorsement and
		 * store the seller's keys in the database (for the buyer to 
		 * retrieve at some later date) */
		vector<ZZ> sellerResolveII(Ptr<const MerkleProof> proof);

		// the following are resolutions for the responder		
		/*! Stage I: the responder sends a request, and the arbiter
		 * checks the validity of the two messages and stores the keys */
		vector<unsigned> responderResolveI(Ptr<const FEResolutionMessage> request);
		vector<unsigned> responderResolveI(const vector<string> &keys, 
										   Ptr<const FEMessage> message, 
										   Ptr<const FESetupMessage> setupMessage);
	
		/*! Stage II: if the proof verifies, return the initiator's keys */
		vector<string> responderResolveII(Ptr<const MerkleProof> proof);
	
		/*! Stage III: if the initiator's keys were incorrect, the responder
		 * sends a proof of this.  if this proof is valid the arbiter will
		 * return the endorsement */
		vector<ZZ> responderResolveIII(Ptr<const MerkleProof> prooof);
	
		//used to test stuff
		void setKeys(const vector<string> &ks){ keys = ks; }

		boost::function<void(ZZ, vector<string>)> updateDB;
		boost::function<vector<string>(ZZ)> getDB;	
	
	private:	
		void updateDatabase(const ZZ &sessionID, const string &key);

		/*! takes in a proof and verifies it using the stored keys */
		bool verifyKeys(Ptr<const MerkleProof> proof);
		
		/*! much like verifyKeys except it ignores plaintext proofs; this
		 * is a helper for responderResolveIII */
		bool verifyDecryption(Ptr<const MerkleProof> proof);
		
		Ptr<const VEDecrypter> verifiableDecrypter;//, regularDecrypter;
		Ptr<const VEDecrypter> regularDecrypter;
		hashalg_t hashAlg;
		int timeoutTolerance;
		Ptr<MerkleVerifier> ptVerifier, ctVerifier;
		vector<ZZ> endorsement;	
		vector<string> keys;
		Ptr<FEContract> contract;
		Ptr<const FEMessage> message;
};

#endif
