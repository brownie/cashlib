
#include "DescribeRelations.h"

void DescribeRelations::apply(ASTNodePtr n) {
	n->visit(*this);
	env.descriptions = translator.getDescriptions();
}

void DescribeRelations::applyASTEqual(ASTEqualPtr n) {

	ASTIdentifierSubPtr lhs = n->getId();
	ASTExprPtr rhs = n->getExpr();

	/** expression must either be a multiplication, square, or DLR proof
	  * multiplication proof will be x = a * b, where all variables are
	  * exponents.  square proof will be x = y^2, where again all variables
	  * are exponents.  DLR proof will be anything else; bases should be
	  * elements and exponents exponents but this will have been guaranteed
	  * by TypeChecker */
	ASTMulPtr mult = dynamic_pointer_cast<ASTMul>(rhs);
	if (mult != 0) {
		// if they are exponents, this is a multiplication proof
		VarInfo info = mult->getExprType(env);
		if (info.type == VarInfo::EXPONENT) {
			// need to get all identifiers and look up commitments
			DLRepresentation productDes = env.getCommitment(lhs->getName());
			ASTExprIdentifierPtr l = dynamic_pointer_cast<ASTExprIdentifier>(mult->getLHS());
			ASTExprIdentifierPtr r = dynamic_pointer_cast<ASTExprIdentifier>(mult->getRHS());
			if (l == 0 || r == 0)
				throw CashException(CashException::CE_PARSE_ERROR,
							"That is not a valid relation");
			DLRepresentation lDes = env.getCommitment(l->toString());
			DLRepresentation rDes = env.getCommitment(r->toString());
			translator.describeMultiplication(productDes, lDes, rDes);
		}
		// if they are elements, this is DLR proof
		else if (info.type == VarInfo::ELEMENT) {
			// need to split RHS and use as DL for LHS
			DLRepresentation dlDes = splitExpr(n, env);
			translator.describeDLR(dlDes);
		}
		else {
			throw CashException(CashException::CE_PARSE_ERROR,
							"That is not a valid relation");
		}
	}
	else {
		ASTPowPtr pow = dynamic_pointer_cast<ASTPow>(rhs);
		if (pow != 0) {
			// if values are exponents, it is a square proof
			VarInfo info = pow->getExprType(env);
			if (info.type == VarInfo::EXPONENT) {
				// need to split this description up (into DL description)
				DLRepresentation squareDes = env.getCommitment(lhs->getName());
				// need to get identifier from RHS and look up its commitment
				// description in the environment 
				ASTExprIdentifierPtr id = dynamic_pointer_cast<ASTExprIdentifier>(pow->getLHS());
				if (id == 0) 
					throw CashException(CashException::CE_PARSE_ERROR,
							"That is not a valid relation");
				DLRepresentation rootDes = env.getCommitment(id->toString());
				translator.describeSquare(squareDes, rootDes);
			}
			// if they are elements, this is a DLR proof
			else if (info.type == VarInfo::ELEMENT) {
				// need to split up the whole RHS and use that as DL
				DLRepresentation dlDes = splitExpr(n, env);
				translator.describeDLR(dlDes);
			}
			// using integers doesn't make any sense
			else {
				throw CashException(CashException::CE_PARSE_ERROR,
						"That is not a valid relation");
			}
		}
		else {
			throw CashException(CashException::CE_PARSE_ERROR,
					"That is not a valid relation");
		}
	}
}

void DescribeRelations::applyASTRange(ASTRangePtr n) {
	// XXX: can we be more flexible than this?
	// first check that lower bound is not strict and upper is 
	if (n->isLowerStrict() || !n->isUpperStrict()) 
		throw CashException(CashException::CE_PARSE_ERROR,
				"Those are not valid bounds for a range proof");

	// also check that we are using an RSA group (if we already have groups)
	ASTIdentifierLitPtr grp = n->getGroup();
	if (env.groups.count(grp->getName()) > 0) {
		if (env.groups.at(grp->getName())->getType() != Group::TYPE_RSA)
			throw CashException(CashException::CE_PARSE_ERROR,
					"Cannot use a non-RSA group for doing range proofs");
	}
				
	ASTExprPtr center = n->getCenter();
	translator.describeRange(n->getCenter(), grp, n->getLower(), n->getUpper());
}

DLRepresentation DescribeRelations::splitExpr(ASTEqualPtr n, Environment &e) {

	ASTExprPtr rhs = n->getExpr();
	DLRepresentation dlr;
	// get left-hand side
	ASTExprIdentifierPtr lhs = new_ptr<ASTExprIdentifier>(n->getId());
	dlr.left = lhs;
	// need to get group as well
	string leftName = n->getId()->getName();
	dlr.group = e.varTypes.at(leftName).group;

	vector<ASTExprPtr> bs;
	vector<ASTExprPtr> es;
	// now get bases and exponents
	// easiest case is if there is only one base/exp pair; i.e. g^x
	ASTPowPtr one = dynamic_pointer_cast<ASTPow>(rhs);
	if (one != 0) {
		bs.push_back(one->getLHS());
		es.push_back(one->getRHS());
		dlr.bases = bs;
		dlr.exps = es;
		return dlr;
	}
	else {
		// go down tree looking for a bunch of mults in a row
		// until the chain ends, want to look at pows along the way
		// for each power, split the expression up into base and exponent
		ASTMulPtr mult = dynamic_pointer_cast<ASTMul>(rhs);
		splitExprHelper(bs, es, mult);
		dlr.bases = bs;
		dlr.exps = es;		
		return dlr;
	}
}

void DescribeRelations::splitExprHelper(vector<ASTExprPtr>& bases, 
										vector<ASTExprPtr>& exponents, 
										ASTMulPtr mult){
	ASTMulPtr next;
	ASTPowPtr pow;
	if ((pow = dynamic_pointer_cast<ASTPow>(mult->getRHS())) != 0) {
		bases.push_back(pow->getLHS());
		exponents.push_back(pow->getRHS());
	} else if ((next = dynamic_pointer_cast<ASTMul>(mult->getRHS())) != 0){
		splitExprHelper(bases, exponents, next);
	} else {
		bases.push_back(mult->getRHS());
		exponents.push_back(new_ptr<ASTExprInt>(to_ZZ(1)));
	}
	if ((pow = dynamic_pointer_cast<ASTPow>(mult->getLHS())) != 0) {
		bases.push_back(pow->getLHS());
		exponents.push_back(pow->getRHS());
	} else if ((next = dynamic_pointer_cast<ASTMul>(mult->getLHS())) != 0){
		splitExprHelper(bases, exponents, next);
	} else {
		bases.push_back(mult->getLHS());
		exponents.push_back(new_ptr<ASTExprInt>(to_ZZ(1)));
	}
}
