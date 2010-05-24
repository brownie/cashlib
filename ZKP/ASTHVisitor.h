
#ifndef ASTHVISITOR_H_
#define ASTHVISITOR_H_

#include "ASTVisitor.h"
#include "ASTNode.h"

class ASTHVisitor : public ASTVisitor {

	public: 
		void applyASTNode(ASTNodePtr n){}
	
		//Lists
		void applyASTList(ASTListPtr n){ applyASTNode(dynamic_pointer_cast<ASTNode>(n)); }
		void applyASTListIdentifierLit(ASTListIdentifierLitPtr n){applyASTList(n);}
		void applyASTListIdentifierSub(ASTListIdentifierSubPtr n){applyASTList(n);}
		void applyASTListDeclIdentifierLit(ASTListDeclIdentifierLitPtr n){applyASTList(n);}
		void applyASTListDeclIdentifierSub(ASTListDeclIdentifierSubPtr n){applyASTList(n);}
		void applyASTListRelation(ASTListRelationPtr n){applyASTList(n);}
		void applyASTListGiven(ASTListGivenPtr n){applyASTList(n);}
		void applyASTListRandoms(ASTListRandomsPtr n){applyASTList(n);}
		void applyASTListDecl(ASTListDeclPtr n){applyASTList(n);}
		void applyASTDeclIDRange(ASTDeclIDRangePtr n){applyASTNode(n);}

		//Expressions
		void applyASTExpr(ASTExprPtr n){applyASTNode(n);}		
		void applyASTExprInt(ASTExprIntPtr n){applyASTExpr(n);}
		void applyASTExprIdentifier(ASTExprIdentifierPtr n){applyASTExpr(n);}
	  	void applyASTUnaryOp(ASTUnaryOpPtr n){applyASTExpr(n);}
	  	void applyASTBinaryOp(ASTBinaryOpPtr n){applyASTExpr(n);}
	  	
	  	void applyASTForExpr(ASTForExprPtr n){applyASTNode(n);}
	  	void applyASTForRel(ASTForRelPtr n){applyASTNode(n);}
	
	  	void applyASTNegative(ASTNegativePtr n){applyASTUnaryOp(n);}
	
		void applyASTPow(ASTPowPtr n){applyASTBinaryOp(n);}
		void applyASTMul(ASTMulPtr n){applyASTBinaryOp(n);}
	  	void applyASTDiv(ASTDivPtr n){applyASTBinaryOp(n);}
	  	void applyASTAdd(ASTAddPtr n){applyASTBinaryOp(n);}
	  	void applyASTSub(ASTSubPtr n){applyASTBinaryOp(n);}
	
		//Relations
		void applyASTRelation(ASTRelationPtr n){applyASTNode(n);}
	  	void applyASTEqual(ASTEqualPtr n){applyASTRelation(n);}
	  	void applyASTCommitment(ASTCommitmentPtr n){applyASTRelation(n);}
	 	void applyASTRange(ASTRangePtr n){applyASTRelation(n);}
	  	void applyASTDeclEqual(ASTDeclEqualPtr n) {applyASTRelation(n);}
	  	void applyASTRandomBnd(ASTRandomBndPtr n){applyASTNode(n);}
	  	void applyASTRandomPrime(ASTRandomPrimePtr n){applyASTNode(n);}
	  	
		//Declarations
	  	void applyASTDeclIdentifierLit(ASTDeclIdentifierLitPtr n){applyASTNode(n);}
	  	void applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n){applyASTNode(n);}
	
		//"Given" Constructions
	  	void applyASTGiven(ASTGivenPtr n){applyASTNode(n);}
	  	void applyASTDeclElements(ASTDeclElementsPtr n){applyASTGiven(n);}
	  	void applyASTDeclExponents(ASTDeclExponentsPtr n){applyASTGiven(n);}
	  	void applyASTDeclGroup(ASTDeclGroupPtr n){applyASTGiven(n);}
	  	void applyASTDeclIntegers(ASTDeclIntegersPtr n){applyASTGiven(n);}
	  	void applyASTDeclRandExponents(ASTDeclRandExponentsPtr n) {applyASTGiven(n);}
	
		//The Core Constructions
		void applyASTProof(ASTProofPtr n){applyASTNode(n);}
	  	void applyASTComputation(ASTComputationPtr n){applyASTNode(n);}
	  	void applyASTSpec(ASTSpecPtr n){applyASTNode(n);}
	
		//Other
	  	void applyASTIdentifierLit(ASTIdentifierLitPtr n){applyASTNode(n);}
	  	void applyASTIdentifierSub(ASTIdentifierSubPtr n){applyASTNode(n);}	
};

#endif /*ASTHVISITOR_H_*/
