
#ifndef _TYPECHECKER_H_
#define _TYPECHECKER_H_

#include "ASTNode.h"
#include "ASTTVisitor.h"

class TypeChecker : public ASTTVisitor {

	public:
		/*! run before we have actual values (and after GroupIdentifier) */
		TypeChecker(Environment &e) : env(e) {}
		
		/*! associate these as exponents in the given group */
		void applyASTDeclRandExponents(ASTDeclRandExponentsPtr n);		
		
		/*! check to make sure RHS is valid; if it is, associate left-hand side
		 * with appropriate group and type
		 */
		void applyASTDeclEqual(ASTDeclEqualPtr n);
		
		/*! check validity of RHS and assign LHS to correct group/type */
		void applyASTEqual(ASTEqualPtr n);

		/*! check the whole tree */
		void apply(ASTNodePtr n);

	private:
		Environment &env;

};

#endif /*_TYPECHECKER_H_*/
