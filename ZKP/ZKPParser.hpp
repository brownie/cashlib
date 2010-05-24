#ifndef INC_ZKPParser_hpp_
#define INC_ZKPParser_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20090708): "zkp.g" -> "ZKPParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "ZKPParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 3 "zkp.g"

//#include "../NTL/ZZ.h"
//namespace="brownie" XXX use namespaces someday
#include "ASTNode.h"
#include "new_ptr.hpp"

#line 19 "ZKPParser.hpp"
class CUSTOM_API ZKPParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public ZKPParserTokenTypes
{
#line 1 "zkp.g"
#line 23 "ZKPParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	ZKPParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	ZKPParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	ZKPParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	ZKPParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	ZKPParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return ZKPParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return ZKPParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return ZKPParser::tokenNames;
	}
	public: ASTSpecPtr  spec();
	public: ASTComputationPtr  computation();
	public: ASTProofPtr  proof();
	public: ASTListGivenPtr  givenList();
	public: ASTListRandomsPtr  computeRandomList();
	public: ASTListRelationPtr  computeEquationList();
	public: ASTListGivenPtr  knowledgeList();
	public: ASTListRelationPtr  suchThatList();
	public: ASTRelationPtr  suchThatRel();
	public: ASTEqualPtr  equalRelation();
	public: ASTRangePtr  rangeRelation();
	public: ASTRelationPtr  forRelation();
	public: ASTDeclExponentsPtr  exponentDecl();
	public: ASTDeclIntegersPtr  integerDecl();
	public: ASTDeclGroupPtr  groupDecl();
	public: ASTDeclElementsPtr  elementsDecl();
	public: ASTRandomBndPtr  randomBndDecl();
	public: ASTExprPtr  expr();
	public: ASTListDeclPtr  idGeneralDeclList();
	public: ASTRandomPrimePtr  randomPrimeDecl();
	public: ASTDeclRandExponentsPtr  randExponentDecl();
	public: ASTDeclIdentifierLitPtr  identifierDecl();
	public: ASTListDeclPtr  setDecl();
	public: ASTDeclIdentifierSubPtr  subscriptIdentifierDecl();
	public: ASTIdentifierLitPtr  identifier();
	public: ASTListRelationPtr  elementsEquationList();
	public: ASTRelationPtr  computeEquation();
	public: ASTDeclEqualPtr  equalDeclRelation();
	public: ASTCommitmentPtr  comRelation();
	public: ASTRelationPtr  genEqual();
	public: ASTRelationPtr  forCom();
	public: ASTRelationPtr  elementsEquation();
	public: ASTListIdentifierSubPtr  idSubList();
	public: ASTIdentifierSubPtr  subscriptIdentifier();
	public: ASTExprPtr  prodExpr();
	public: ASTExprPtr  forExpr();
	public: ASTExprPtr  powExpr();
	public: ASTExprPtr  unaryExpr();
	public: ASTExprPtr  baseExpr();
	public: ASTListDeclIdentifierSubPtr  idSubDeclList();
	public: ASTDeclGeneralPtr  idDeclGeneral();
	public: ASTListDeclIdentifierLitPtr  idDeclList();
	public: ASTListIdentifierLitPtr  idList();
	public: ASTDeclIDRangePtr  idDeclRange();
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 56;
#else
	enum {
		NUM_TOKENS = 56
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
};

#endif /*INC_ZKPParser_hpp_*/
