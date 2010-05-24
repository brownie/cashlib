
#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_

#include "Environment.h"
#include "ASTNode.h"

class Translator {

	public:
		Translator(Environment &e) : env(e), counter(0) {}
		virtual ~Translator() {}

		/*! add the description to output list */
		void describeDLR(DLRepresentation &c);

		/*! reduce commitments to form needed to prove x = a * b */
		void describeMultiplication(DLRepresentation &product,
									DLRepresentation &factor1,
									DLRepresentation &factor2);

		/*! reduce commitments to form needed to prove y = x^2 */
		void describeSquare(DLRepresentation &product, DLRepresentation &factor);

		void describeNonNegative(DLRepresentation &c);

		/*! for proving low <= val < high using the specified group */
		void describeRange(ASTExprPtr val, ASTIdentifierLitPtr group, 
						   ASTExprPtr low, ASTExprPtr high);

		dlr_map getDescriptions() { return output; }

	protected:	
		/*! helper for creating new nodes in the ASTPtr */
		ASTExprIdentifierPtr nameNode(const string &name);
		
		Environment &env;
		dlr_map output;
		int counter;

};

#endif /*_TRANSLATOR_H_*/


