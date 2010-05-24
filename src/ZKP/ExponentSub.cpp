
#include "ExponentSub.h"
#include "Environment.h"

void ExponentSub::apply(ASTNodePtr n) {
	n->visit(*this);
}

void ExponentSub::applyASTMul(ASTMulPtr mul) {
	ASTExprPtr lhs = mul->getLHS();
	ASTExprPtr rhs = mul->getRHS();
	// only if the sides are exprIDs or exprInts do we want to add 1
	if (dynamic_pointer_cast<ASTExprIdentifier>(lhs) != 0 ||
		dynamic_pointer_cast<ASTExprInt>(lhs) != 0) {
		ASTPowPtr newLHS = new_ptr<ASTPow>(lhs, new_ptr<ASTExprInt>("1"));
		lhs = newLHS;
	}
	if (dynamic_pointer_cast<ASTExprIdentifier>(rhs) != 0 ||
		dynamic_pointer_cast<ASTExprInt>(rhs) != 0) {
		ASTPowPtr newRHS = new_ptr<ASTPow>(rhs, new_ptr<ASTExprInt>("1"));
		rhs = newRHS;
	}
}
