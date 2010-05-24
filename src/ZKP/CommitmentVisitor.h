
#ifndef _COMMITMENTVISITOR_H_
#define _COMMITMENTVISITOR_H_

#include "ASTNode.h"
#include "ASTTVisitor.h"

/*!
 * \brief This class is used for describing the form of commitments and for
 * associating the names of the commitments with the names of the values
 * inside
 */

class CommitmentVisitor : public ASTTVisitor {

	public:
		/*! this visitor is used before the user gives actual values */
		CommitmentVisitor(Environment &e) : env(e) {}		
		
		/*! associate values inside of a commitment to the commitment name;
		 * also associate commitment name to its DLR */
		void applyASTCommitment(ASTCommitmentPtr n);

		/*! describes all commitments */
		void apply(ASTNodePtr n);

	private:
		Environment &env;

};

#endif /*_COMMITMENTVISITOR_H_*/


