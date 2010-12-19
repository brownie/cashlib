
#ifndef _FEMESSAGE_H_
#define _FEMESSAGE_H_

#include "FEContract.h"

/*! \brief This class is a wrapper for the message sent in Step 4 of the 
 * 	barter protocol */ 

class FEMessage {
	public:
		/*! constructor: takes in escrow (encryption of the symmetric
		 * key), signature on this escrow, and contract (label for the
		 * escrow) */
		FEMessage(const vector<ZZ> &escrow, 
                  const string &sig, Ptr<FEContract> contract)
			: escrow(escrow), signature(sig), contract(contract) {}
		
		FEMessage(const string &sig, Ptr<FEContract> contract)
			: signature(sig), contract(contract) {}

		/*! copy constructor */
		FEMessage(const FEMessage &o)
			: escrow(o.escrow), signature(o.signature), contract(o.contract) {}
		
		FEMessage(const string& s) { loadString(*this, s); };

		// getters
		const vector<ZZ>& getEscrow() const { return escrow; }
		const string& getSignature() const { return signature; }
		Ptr<FEContract> getContract() const { return contract; }
	
	private:
		vector<ZZ> escrow;
		string signature;
		Ptr<FEContract> contract;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(escrow)
				& auto_nvp(signature)
				& auto_nvp(contract);
		}
};

#endif /*BARTERMESSAGE_H_*/
