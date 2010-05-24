#ifndef __CONSTANTPROP_H__
#define __CONSTANTPROP_H__

#include "ASTNode.h"
#include "ASTTVisitor.h"
#include <stack>
#include "Environment.h"

class ConstantProp : public ASTTVisitor{

	public:
		ConstantProp(input_map &i) : inputs(i), newConstant(true) {}

		void applyASTForExpr(ASTForExprPtr n);	
		void applyASTForRel(ASTForRelPtr n);
		void applyASTDeclIDRange(ASTDeclIDRangePtr n);
		void applyASTRange(ASTRangePtr n) {}
		void applyASTCommitment(ASTCommitmentPtr n) {}
		void applyASTEqual(ASTEqualPtr n) {}
		void applyASTDeclEqual(ASTDeclEqualPtr n) {}
		void applyASTBinaryOp(ASTBinaryOpPtr n);
		void applyASTUnaryOp(ASTUnaryOpPtr n);
		void applyASTRandomBnd(ASTRandomBndPtr n) {}
		void applyASTRandomPrime(ASTRandomPrimePtr n) {}

		bool subAgain(){return newConstant;}
		void reset(){newConstant=false;}
	
	private:
		input_map &inputs;
		bool newConstant;
};

#endif /* __CONSTANTPROP_H__*/
