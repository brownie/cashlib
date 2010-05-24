
#ifndef _TYPEIDENTIFIER_H_
#define _TYPEIDENTIFIER_H_

#include "ASTTVisitor.h"
#include "ASTNode.h"

/*!
 * \brief This class is used to identify the group in which a given
 * variable is contained and also the privacy setting for that variable
 */

class TypeIdentifier : public ASTTVisitor {

	public:
		/*! this visitor should be used before we have actual values */
		TypeIdentifier(Environment &e) : env(e) {}

		/*! used for privacy settings */
		void applyASTProof(ASTProofPtr n);

		/*! used to associate elements/exponents/generators with their groups */
		void applyASTDeclElements(ASTDeclElementsPtr n);
		void applyASTDeclExponents(ASTDeclExponentsPtr n);
		void applyASTDeclGroup(ASTDeclGroupPtr n);

		/*! used to associate integers with no group */
		void applyASTDeclIntegers(ASTDeclIntegersPtr n);
		void applyASTRandomBnd(ASTRandomBndPtr n);
		void applyASTRandomPrime(ASTRandomPrimePtr n);

		/*! associate all variables with groups and privacy setting */
		void apply(ASTNodePtr n);

	private:
		Environment &env;
		bool isPrivate;
};

#endif /*_GROUPIDENTIFIER_H_*/

