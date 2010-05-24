#ifndef _ASTVISITOR_H_
#define _ASTVISITOR_H_

#include <boost/shared_ptr.hpp>

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
		virtual void applyASTNode(boost::shared_ptr<ASTNode> n) = 0;

		//Lists
		virtual void applyASTList(boost::shared_ptr<ASTList> n) = 0;
		virtual void applyASTListIdentifierLit(boost::shared_ptr<ASTListIdentifierLit> n) = 0;
		virtual void applyASTListIdentifierSub(boost::shared_ptr<ASTListIdentifierSub> n) = 0;
		virtual void applyASTListDeclIdentifierLit(boost::shared_ptr<ASTListDeclIdentifierLit> n) = 0;
		virtual void applyASTListDeclIdentifierSub(boost::shared_ptr<ASTListDeclIdentifierSub> n) = 0;
		virtual void applyASTListRelation(boost::shared_ptr<ASTListRelation> n) = 0;
		virtual void applyASTListGiven(boost::shared_ptr<ASTListGiven> n) = 0;
		virtual void applyASTListRandoms(boost::shared_ptr<ASTListRandoms> n) = 0;
		virtual void applyASTListDecl(boost::shared_ptr<ASTListDecl> n) = 0;
		virtual void applyASTDeclIDRange(boost::shared_ptr<ASTDeclIDRange> n) = 0;

		//Expressions
		virtual void applyASTExpr(boost::shared_ptr<ASTExpr> n) = 0;	
		virtual void applyASTExprInt(boost::shared_ptr<ASTExprInt> n) = 0;
		virtual void applyASTUnaryOp(boost::shared_ptr<ASTUnaryOp> n) = 0;
		virtual void applyASTBinaryOp(boost::shared_ptr<ASTBinaryOp> n) = 0;
		virtual void applyASTNegative(boost::shared_ptr<ASTNegative> n) = 0;
		virtual void applyASTExprIdentifier(boost::shared_ptr<ASTExprIdentifier> n) = 0;
		virtual void applyASTPow(boost::shared_ptr<ASTPow> n) = 0;
		virtual void applyASTMul(boost::shared_ptr<ASTMul> n) = 0;
		virtual void applyASTDiv(boost::shared_ptr<ASTDiv> n) = 0;
		virtual void applyASTAdd(boost::shared_ptr<ASTAdd> n) = 0;
		virtual void applyASTSub(boost::shared_ptr<ASTSub> n) = 0;
		
		virtual void applyASTForExpr(boost::shared_ptr<ASTForExpr> n) = 0;
		virtual void applyASTForRel(boost::shared_ptr<ASTForRel> n) = 0;
	
		//Relations
		virtual void applyASTRelation(boost::shared_ptr<ASTRelation> n) = 0;
		virtual void applyASTEqual(boost::shared_ptr<ASTEqual> n) = 0;
		virtual void applyASTCommitment(boost::shared_ptr<ASTCommitment> n) = 0;
		virtual void applyASTRange(boost::shared_ptr<ASTRange> n) = 0;
		virtual void applyASTDeclEqual(boost::shared_ptr<ASTDeclEqual> n) = 0;
		virtual void applyASTRandomBnd(boost::shared_ptr<ASTRandomBnd> n) = 0;
		virtual void applyASTRandomPrime(boost::shared_ptr<ASTRandomPrime> n) = 0;
	
		//Declarations
		virtual void applyASTDeclIdentifierLit(boost::shared_ptr<ASTDeclIdentifierLit> n) = 0;
		virtual void applyASTDeclIdentifierSub(boost::shared_ptr<ASTDeclIdentifierSub> n) = 0;
	
		//"Given" Constructions
		virtual void applyASTGiven(boost::shared_ptr<ASTGiven> n) = 0;
		virtual void applyASTDeclElements(boost::shared_ptr<ASTDeclElements> n) = 0;
		virtual void applyASTDeclExponents(boost::shared_ptr<ASTDeclExponents> n) = 0;
		virtual void applyASTDeclRandExponents(boost::shared_ptr<ASTDeclRandExponents> n) = 0;
		virtual void applyASTDeclGroup(boost::shared_ptr<ASTDeclGroup> n) = 0;
		virtual void applyASTDeclIntegers(boost::shared_ptr<ASTDeclIntegers> n) = 0;
	
		//The Core Constructions
		virtual void applyASTProof(boost::shared_ptr<ASTProof> n) = 0;
		virtual void applyASTComputation(boost::shared_ptr<ASTComputation> n) = 0;
		virtual void applyASTSpec(boost::shared_ptr<ASTSpec> n) = 0;
	
		//Other
		virtual void applyASTIdentifierLit(boost::shared_ptr<ASTIdentifierLit> n) = 0;
		virtual void applyASTIdentifierSub(boost::shared_ptr<ASTIdentifierSub> n) = 0;
};

#endif /*_ASTVISITOR_H_*/

