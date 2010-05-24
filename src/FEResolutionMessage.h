
#ifndef _FERESOLUTIONMESSAGE_H_
#define _FERESOLUTIONMESSAGE_H_

#include "FEMessage.h"
#include "FESetupMessage.h"

/*! \brief This class is a wrapper around messages sent to the arbiter in 
 * Stage 1 of the responder's resolution protocol */

class FEResolutionMessage {
	
	public:
		FEResolutionMessage(FEMessage* m, FESetupMessage* s, 
							const vector<string> &k)
		   : message(m), setupMessage(s), keys(k) {}

		FEResolutionMessage() : message(0), setupMessage(0) {}

		// getters
		FEMessage* getMessage() const { return message; }
		FESetupMessage* getSetupMessage() const { return setupMessage; }
		vector<string> getKeys() const { return keys; }

	private:
		FEMessage* message;
		FESetupMessage* setupMessage;
		vector<string> keys;

		friend class boost::serialization::access;
			template <class Archive>
			void serialize(Archive& ar, const unsigned int ver) {
				ar	& auto_nvp(message)
					& auto_nvp(setupMessage)
					& auto_nvp(keys);
			}
};

#endif /*_FERESOLUTIONMESSAGE_H_*/
