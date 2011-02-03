#ifndef _ASTVISITOR_H_
#define _ASTVISITOR_H_

#include <boost/shared_ptr.hpp>
#include "new_ptr.hpp"

class ASTNode;
class ASTIdentifierLit;
class ASTIdentifierSub;
class ASTDeclIdentifierLit;
class ASTDeclIdentifierSub;
class ASTExpr;
class ASTExprIdentifier;
class ASTUnaryOp;
class ASTNegative;
class ASTBinaryOp;
class ASTAdd;
class ASTSub;
class ASTMul;
class ASTDiv;
class ASTPow;
class ASTList;
class ASTListIdentifierLit;
class ASTListIdentifierSub;
class ASTListDeclIdentifierLit;
class ASTListDeclIdentifierSub;
class ASTListDecl;
class ASTRelation;
class ASTEqual;
class ASTExprInt;
class ASTCommitment;
class ASTGiven;
class ASTDeclGroup;
class ASTDeclIntegers;
class ASTDeclElements;
class ASTDeclEqual;
class ASTDeclExponents;
class ASTDeclRandExponents;
class ASTDeclIDRange;
class ASTListGiven;
class ASTListRandoms;
class ASTListRelation;
class ASTRange;
class ASTComputation;
class ASTProof;
class ASTSpec;
class ASTForExpr;
class ASTForRel;
class ASTRandomBnd;
class ASTRandomPrime;

class ASTVisitor {

	public:
		virtual ~ASTVisitor() {}
		virtual void applyASTNode(Ptr<ASTNode> n) = 0;

		//Lists
		virtual void applyASTList(Ptr<ASTList> n) = 0;
		virtual void applyASTListIdentifierLit(Ptr<ASTListIdentifierLit> n) = 0;
		virtual void applyASTListIdentifierSub(Ptr<ASTListIdentifierSub> n) = 0;
		virtual void applyASTListDeclIdentifierLit(Ptr<ASTListDeclIdentifierLit> n) = 0;
		virtual void applyASTListDeclIdentifierSub(Ptr<ASTListDeclIdentifierSub> n) = 0;
		virtual void applyASTListRelation(Ptr<ASTListRelation> n) = 0;
		virtual void applyASTListGiven(Ptr<ASTListGiven> n) = 0;
		virtual void applyASTListRandoms(Ptr<ASTListRandoms> n) = 0;
		virtual void applyASTListDecl(Ptr<ASTListDecl> n) = 0;
		virtual void applyASTDeclIDRange(Ptr<ASTDeclIDRange> n) = 0;

		//Expressions
		virtual void applyASTExpr(Ptr<ASTExpr> n) = 0;	
		virtual void applyASTExprInt(Ptr<ASTExprInt> n) = 0;
		virtual void applyASTUnaryOp(Ptr<ASTUnaryOp> n) = 0;
		virtual void applyASTBinaryOp(Ptr<ASTBinaryOp> n) = 0;
		virtual void applyASTNegative(Ptr<ASTNegative> n) = 0;
		virtual void applyASTExprIdentifier(Ptr<ASTExprIdentifier> n) = 0;
		virtual void applyASTPow(Ptr<ASTPow> n) = 0;
		virtual void applyASTMul(Ptr<ASTMul> n) = 0;
		virtual void applyASTDiv(Ptr<ASTDiv> n) = 0;
		virtual void applyASTAdd(Ptr<ASTAdd> n) = 0;
		virtual void applyASTSub(Ptr<ASTSub> n) = 0;
		
		virtual void applyASTForExpr(Ptr<ASTForExpr> n) = 0;
		virtual void applyASTForRel(Ptr<ASTForRel> n) = 0;
	
		//Relations
		virtual void applyASTRelation(Ptr<ASTRelation> n) = 0;
		virtual void applyASTEqual(Ptr<ASTEqual> n) = 0;
		virtual void applyASTCommitment(Ptr<ASTCommitment> n) = 0;
		virtual void applyASTRange(Ptr<ASTRange> n) = 0;
		virtual void applyASTDeclEqual(Ptr<ASTDeclEqual> n) = 0;
		virtual void applyASTRandomBnd(Ptr<ASTRandomBnd> n) = 0;
		virtual void applyASTRandomPrime(Ptr<ASTRandomPrime> n) = 0;
	
		//Declarations
		virtual void applyASTDeclIdentifierLit(Ptr<ASTDeclIdentifierLit> n) = 0;
		virtual void applyASTDeclIdentifierSub(Ptr<ASTDeclIdentifierSub> n) = 0;
	
		//"Given" Constructions
		virtual void applyASTGiven(Ptr<ASTGiven> n) = 0;
		virtual void applyASTDeclElements(Ptr<ASTDeclElements> n) = 0;
		virtual void applyASTDeclExponents(Ptr<ASTDeclExponents> n) = 0;
		virtual void applyASTDeclRandExponents(Ptr<ASTDeclRandExponents> n) = 0;
		virtual void applyASTDeclGroup(Ptr<ASTDeclGroup> n) = 0;
		virtual void applyASTDeclIntegers(Ptr<ASTDeclIntegers> n) = 0;
	
		//The Core Constructions
		virtual void applyASTProof(Ptr<ASTProof> n) = 0;
		virtual void applyASTComputation(Ptr<ASTComputation> n) = 0;
		virtual void applyASTSpec(Ptr<ASTSpec> n) = 0;
	
		//Other
		virtual void applyASTIdentifierLit(Ptr<ASTIdentifierLit> n) = 0;
		virtual void applyASTIdentifierSub(Ptr<ASTIdentifierSub> n) = 0;
};

#endif /*_ASTVISITOR_H_*/

