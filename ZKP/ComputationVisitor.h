
#ifndef _COMPUTATIONVISITOR_H_
#define _COMPUTATIONVISITOR_H_

#include "ASTTVisitor.h"
#include "ASTNode.h"

/*! 
 * \brief This visitor carries out the instructions for the computation
 * block and stores the results 
 */

class ComputationVisitor : public ASTTVisitor {

	public:
		/*! this visitor is used after the user gives us real values */
		ComputationVisitor(Environment &e) : env(e) {}

		/*! compute all random values needed */
		void applyASTDeclRandExponents(ASTDeclRandExponentsPtr n);
		void applyASTRandomBnd(ASTRandomBndPtr n);
		void applyASTRandomPrime(ASTRandomPrimePtr n);

		/*! computes commitments and other DLR forms; will throw exception
		 * if unable to perform computation (i.e. b/c mismatched groups) */
		void applyASTDeclEqual(ASTDeclEqualPtr n);

		/*! performs all computations described in computation block and
		 * stores new variables in the environment */
		void apply(ASTNodePtr n);

	private:
		Environment &env;
};

#endif /*_COMPUTATIONVISITOR_H_*/
