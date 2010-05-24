
#include "CommitmentVisitor.h"
#include "DescribeRelations.h"

void CommitmentVisitor::applyASTCommitment(ASTCommitmentPtr n) {

	ASTListIdentifierSubPtr vals = n->getComs();
	ASTIdentifierSubPtr com = n->getId();
	ASTExprPtr dlr = n->getExpr();
	string comName = com->getName();

	// need to get real DLR to put in map
	ASTEqualPtr eq = new_ptr<ASTEqual>(com, dlr);
	DLRepresentation rep = DescribeRelations::splitExpr(eq, env);

	// add DL form of the commitment in here
	env.discreteLogs[comName] = rep;

	ASTIdentifierSubPtr name;
	for (int i = 0; i < vals->size(); i++) {
		name = dynamic_pointer_cast<ASTIdentifierSub>(vals->get(i));
		// problem if it's not id or if it's not in the map
		if (name == 0 || env.varTypes.count(name->getName()) == 0)
			throw CashException(CashException::CE_PARSE_ERROR,
					"Variable %d was not a valid name", i);
		env.commitments[name->getName()] = comName;
	}
}

void CommitmentVisitor::apply(ASTNodePtr n) {
	n->visit(*this);
}
