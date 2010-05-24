
#include "TypeChecker.h"
#include "Environment.h"

void TypeChecker::applyASTDeclRandExponents(ASTDeclRandExponentsPtr n) {
	
	ASTListDeclPtr exps = n->getExponents();
	string grpname = n->getGroup()->getName();

	ASTDeclIdentifierSubPtr exp;
	ASTDeclIDRangePtr expRange;
	VarInfo info(grpname, VarInfo::EXPONENT);
	for (int i = 0; i < exps->size(); i++) {
		ASTNodePtr ith = exps->get(i);
		if ((exp = dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0){
			// associate exponent with its group
			env.varTypes[exp->getName()] = info;
		} else if((expRange=dynamic_pointer_cast<ASTDeclIDRange>(ith))!=0){
			for(int j = expRange->getLBound(); j <= expRange->getUBound(); j++){
				env.varTypes[expRange->getName(j)] = info;
			}
		} else { 
			throw CashException(CashException::CE_PARSE_ERROR,
					"Variable %d in list was not a valid exponent name", i);
		}
	}
}

void TypeChecker::applyASTDeclEqual(ASTDeclEqualPtr n) {

	ASTDeclIdentifierSubPtr lhs = n->getId();
	ASTExprPtr rhs = n->getExpr();
	// recursively work to find expression type and assign it to the 
	// left-hand side
	// if any errors are found (for example, trying to raise an exponent to
	// an element) an exception will be thrown
	env.varTypes[lhs->getName()] = rhs->getExprType(env);
}

void TypeChecker::applyASTEqual(ASTEqualPtr n) {
	// same as with the decl equal
	ASTIdentifierSubPtr lhs = n->getId();
	ASTExprPtr rhs = n->getExpr();
	env.varTypes[lhs->getName()] = rhs->getExprType(env);
}

void TypeChecker::apply(ASTNodePtr n) {
	n->visit(*this);
}
