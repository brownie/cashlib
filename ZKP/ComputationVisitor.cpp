
#include "ComputationVisitor.h"
#include "DescribeRelations.h"

void ComputationVisitor::applyASTDeclRandExponents(ASTDeclRandExponentsPtr n) {
	ASTListDeclPtr exps = n->getExponents();
	ASTIdentifierLitPtr grpname = n->getGroup();
	const Group* grp = env.groups.at(grpname->getName());
	
	ASTDeclIdentifierSubPtr exp;
	ASTDeclIDRangePtr expRange;
	for (int i = 0; i < exps->size(); i++) {
		ASTNodePtr ith = exps->get(i);
		if ((exp = dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0) {
			// compute random exponent and add into bag of variables
			env.variables[exp->getName()] = grp->randomExponent();
		} else if ((expRange=dynamic_pointer_cast<ASTDeclIDRange>(ith))!=0){
			for(int j = expRange->getLBound(); j <= expRange->getUBound(); j++){
				env.variables[expRange->getName(j)] = grp->randomExponent();
			}
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
						"Variable %d in list was not a valid exponent name", i);
		}
	}
}

void ComputationVisitor::applyASTRandomBnd(ASTRandomBndPtr n){
	ASTListDeclPtr l = n->getIds();
	ZZ lbound = n->getLBound()->eval(env);
	ZZ ubound = n->getUBound()->eval(env);
	
	ASTDeclIdentifierSubPtr rand;
	ASTDeclIDRangePtr randRange;
	for(int i = 0; i < l->size(); i++){
		ASTNodePtr ith = l->get(i);
		if ((rand = dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0) {
			// compute random integer and add into bag of variables
			env.variables[rand->getName()] = RandomBnd(ubound-lbound) + lbound;
		} else if ((randRange=dynamic_pointer_cast<ASTDeclIDRange>(ith)) != 0){
			for(int j = randRange->getLBound(); j <= randRange->getUBound(); j++){
				env.variables[randRange->getName(j)] = RandomBnd(ubound-lbound) 
														+ lbound;
			}
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
							"Identifier given for a random integer was not valid");
		}
	}
}

void ComputationVisitor::applyASTRandomPrime(ASTRandomPrimePtr n){
	ASTListDeclPtr l = n->getIds();
	ZZ length = n->getLength()->eval(env);
	
	ASTDeclIdentifierSubPtr rand;
	ASTDeclIDRangePtr randRange;
	for(int i = 0; i < l->size(); i++){
		ASTNodePtr ith = l->get(i);
		if ((rand = dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0) {
			// compute random integer and add into bag of variables
			env.variables[rand->getName()] = RandomPrime_ZZ(to_long(length));
		} else if ((randRange=dynamic_pointer_cast<ASTDeclIDRange>(ith)) != 0){
			for(int j = randRange->getLBound(); j <= randRange->getUBound(); j++){
				env.variables[randRange->getName(j)] = RandomPrime_ZZ(to_long(length));
			}
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
							"Identifier given for a random prime was not valid");
		}
	}
}

void ComputationVisitor::applyASTDeclEqual(ASTDeclEqualPtr n){

	ASTDeclIdentifierSubPtr lhs = n->getId();
	ASTExprPtr rhs = n->getExpr();
	// if the RHS is a multiplication, need to make this an ASTEqual so 
	// we can call splitExpr and then use multi-exp
	if (dynamic_pointer_cast<ASTMul>(rhs) != 0) {
		ASTIdentifierLitPtr base = lhs->getBase();
		string sub = lhs->getSub();
		ASTIdentifierSubPtr newLHS = new_ptr<ASTIdentifierSub>(base, sub);
		ASTEqualPtr left = new_ptr<ASTEqual>(newLHS, rhs);
		// want to use multiexp for efficiency
		DLRepresentation dlr = DescribeRelations::splitExpr(left, env);
		env.variables[lhs->getName()] = dlr.computeValue(env);
	} else {
		// exception will get thrown here if groups don't match up
		env.variables[lhs->getName()] = rhs->eval(env);
	}
}

void ComputationVisitor::apply(ASTNodePtr n) {
	n->visit(*this);
}
