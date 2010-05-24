#ifndef __CONSTANTSUB_H__
#define __CONSTANTSUB_H__

#include "ASTNode.h"
#include "ASTTVisitor.h"
#include "Environment.h"

class ConstantSub : public ASTTVisitor{
	
	public:
		ConstantSub(input_map &i) : inputs(i), newConstant(true) {}
		
		void applyASTForExpr(ASTForExprPtr n);	
		void applyASTForRel(ASTForRelPtr n);
		void applyASTDeclIDRange(ASTDeclIDRangePtr n);
		void applyASTRange(ASTRangePtr n);
		void applyASTCommitment(ASTCommitmentPtr n);
		void applyASTEqual(ASTEqualPtr n);
		void applyASTDeclEqual(ASTDeclEqualPtr n);
		void applyASTBinaryOp(ASTBinaryOpPtr n);
		void applyASTUnaryOp(ASTUnaryOpPtr n);
		void applyASTIdentifierSub(ASTIdentifierSubPtr n);
		void applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n);
		void applyASTRandomBnd(ASTRandomBndPtr n);
		void applyASTRandomPrime(ASTRandomPrimePtr n);
		
		bool anotherPass(){ return newConstant; }
		void reset(){newConstant=false;}
	
	private:
		input_map &inputs;
		bool newConstant;
};

#endif /* _CONSTANTSUB_H_*/
