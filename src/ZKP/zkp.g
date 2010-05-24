// Parser
// --------------------------------------------------------------------------------
header {
//#include "../NTL/ZZ.h"
//namespace="brownie" XXX use namespaces someday
#include "ASTNode.h"
#include "new_ptr.hpp"
}

options {
   language = "Cpp";
}

class ZKPParser extends Parser;

options { 
   k = 5;
   defaultErrorHandler = false;
}

//c need to be inited with NULL becouse it's optional
spec returns [ASTSpecPtr n] {ASTComputationPtr c; ASTProofPtr p;}
	: (c = computation)? (p = proof)? { n = new_ptr<ASTSpec>(c, p);}
;

computation returns [ASTComputationPtr n] {ASTListGivenPtr g; ASTListRandomsPtr c; ASTListRelationPtr r;}
	: "computation" COLON 
		"given" COLON g=givenList
		"compute" COLON
			c=computeRandomList
			r=computeEquationList
		{n = new_ptr<ASTComputation>(g,c,r);}
;

proof returns [ASTProofPtr n] {ASTListGivenPtr g; ASTListGivenPtr k; ASTListRelationPtr st;}
	: "proof" COLON 
		"given" COLON g=givenList
		"prove" "knowledge" "of" COLON k=knowledgeList
		"such" "that" COLON st=suchThatList
		{n = new_ptr<ASTProof>(g,k,st);}
;

suchThatList returns [ASTListRelationPtr n] {n = new_ptr<ASTListRelation>(); ASTRelationPtr i;}
	: i=suchThatRel {n->add(i);} (i=suchThatRel {n->add(i);})*
;

suchThatRel returns [ASTRelationPtr n] 
	: n = equalRelation
	| n = rangeRelation
	| n = forRelation
;

// XXX: can we ever have private elements?
knowledgeList returns [ASTListGivenPtr n] {n = new_ptr<ASTListGiven>(); ASTGivenPtr i;}
	: (i=exponentDecl {n->add(i);} | i=integerDecl {n->add(i);})+
;

givenList returns [ASTListGivenPtr n] {n = new_ptr<ASTListGiven>(); ASTGivenPtr i;}
	: (i=groupDecl {n->add(i);} 
	| i=elementsDecl {n->add(i);} 
	| i=exponentDecl {n->add(i);}
	| i=integerDecl {n->add(i);})+
;

randomBndDecl returns [ASTRandomBndPtr n] {ASTExprPtr lbound, ubound; ASTListDeclPtr l;}
	: ("integer"|"integers") "in" LBRACKET lbound = expr COMMA ubound = expr RPAREN COLON l = idGeneralDeclList
		{n = new_ptr<ASTRandomBnd>(lbound, ubound, l);}
;

randomPrimeDecl returns [ASTRandomPrimePtr n] {ASTExprPtr length; ASTListDeclPtr l;}
	: ("prime"|"primes") "of" "length" length = expr COLON l=idGeneralDeclList
		{n = new_ptr<ASTRandomPrime>(length, l);}
;

computeRandomList returns [ASTListRandomsPtr n] {n = new_ptr<ASTListRandoms>(); ASTGivenPtr i;}
	: ("random" (i=randExponentDecl 
	| i = randomPrimeDecl 
	| i = randomBndDecl) {n->add(i);})*
;	

// modulus name is optional, so are generators
groupDecl returns [ASTDeclGroupPtr n] {ASTDeclIdentifierLitPtr id; 
									 ASTListDeclPtr ids;
									 ASTDeclIdentifierSubPtr mod;}
	: "group" COLON id=identifierDecl (EQUAL ids=setDecl)? ("modulus" COLON mod=subscriptIdentifierDecl)? {n = new_ptr<ASTDeclGroup>(id,ids,mod);}
;
	
randExponentDecl returns [ASTDeclRandExponentsPtr n] {ASTIdentifierLitPtr id; ASTListDeclPtr ids;}
	: ("exponent"|"exponents") "in" id=identifier COLON ids=idGeneralDeclList {n = new_ptr<ASTDeclRandExponents>(id,ids);}
;
	
exponentDecl returns [ASTDeclExponentsPtr n] {ASTIdentifierLitPtr id; ASTListDeclPtr ids;}
	: ("exponent"|"exponents") "in" id=identifier COLON ids=idGeneralDeclList {n = new_ptr<ASTDeclExponents>(id,ids);}
;

//init of ids is needed becouse the idDeclList is optional
// XXX: why is the idDeclList optional?? should just be the specification
// of commitment forms that is optional...
elementsDecl returns [ASTDeclElementsPtr n] {ASTIdentifierLitPtr id; ASTListDeclPtr ids = new_ptr<ASTListDecl>(); ASTListRelationPtr el;}
	: ("element"|"elements") "in" id=identifier COLON (ids=idGeneralDeclList)? (el=elementsEquationList)? {n = new_ptr<ASTDeclElements>(id,ids,el);}
;

integerDecl returns [ASTDeclIntegersPtr n] {ASTListDeclPtr ids;}
	: ("integer"|"integers") COLON ids=idGeneralDeclList {n = new_ptr<ASTDeclIntegers>(ids);}
;

computeEquationList returns [ASTListRelationPtr n] {n = new_ptr<ASTListRelation>(); ASTRelationPtr i;}
	: i=computeEquation {n->add(i);} (i=computeEquation {n->add(i);})*
;

computeEquation returns [ASTRelationPtr n]
	: n=equalDeclRelation
	| n=comRelation
	| n=forRelation
;

forRelation returns [ASTRelationPtr n]
	{ASTRelationPtr r; ASTIdentifierLitPtr index; ASTExprPtr lbound; ASTExprPtr ubound; string op;}
	: "for" LPAREN index=identifier COMMA lbound = expr COLON ubound = expr COMMA 
				((r = rangeRelation) | (r = genEqual)) RPAREN { n = new_ptr<ASTForRel>(index, lbound, ubound, r); }
;

forCom returns [ASTRelationPtr n]
	{ASTRelationPtr r; ASTIdentifierLitPtr index; ASTExprPtr lbound; ASTExprPtr ubound; string op;}
	: "for" LPAREN index=identifier COMMA lbound = expr COLON ubound = expr COMMA 
					r = comRelation RPAREN { n = new_ptr<ASTForRel>(index, lbound, ubound, r); }
;

elementsEquationList returns [ASTListRelationPtr n] {n = new_ptr<ASTListRelation>(); ASTRelationPtr i;}
	: i=elementsEquation {n->add(i);} (i=elementsEquation {n->add(i);})*
;

elementsEquation returns [ASTRelationPtr n]
	: n=comRelation
	| n=forCom
;

comRelation returns [ASTCommitmentPtr n] {ASTListIdentifierSubPtr coms; ASTIdentifierSubPtr id; ASTExprPtr e;}
	: "commitment" "to" coms=idSubList COLON id=subscriptIdentifier EQUAL e=expr {n = new_ptr<ASTCommitment>(coms, id, e);}
;

genEqual returns [ASTRelationPtr n] {ASTIdentifierLitPtr base; string sub; ASTExprPtr e;}
	: base=identifier (SUBSCRIPT (t:ID {sub = t->getText();} | i:INTLIT { sub = i->getText();})) ? 
		(EQUAL e=expr {n = new_ptr<ASTEqual>(new_ptr<ASTIdentifierSub>(base,sub), e);}
		| CEQUAL e=expr {n = new_ptr<ASTDeclEqual>(new_ptr<ASTDeclIdentifierSub>(base,sub), e);})
;

equalRelation returns [ASTEqualPtr n] {ASTIdentifierSubPtr id; ASTExprPtr e;}
	: id=subscriptIdentifier EQUAL e=expr {n = new_ptr<ASTEqual>(id,e);}
;

equalDeclRelation returns [ASTDeclEqualPtr n] {ASTDeclIdentifierSubPtr id; ASTExprPtr e;}
	: id=subscriptIdentifierDecl CEQUAL e=expr {n = new_ptr<ASTDeclEqual>(id,e);}
;

rangeRelation returns [ASTRangePtr n] {ASTIdentifierLitPtr g; ASTExprPtr l,c,r; 
									   bool lStrict = true; bool rStrict = true;}
	: "range" "in" g=identifier COLON l=expr ((LTHAN|LEQ {lStrict=false;})
c=expr (LTHAN|LEQ {rStrict=false;}) r=expr {n = new_ptr<ASTRange>(g,l,lStrict,c,rStrict,r);}
	| (GTHAN|GEQ {lStrict=false;}) c=expr (GTHAN|GEQ {rStrict=false;}) r=expr
{n = new_ptr<ASTRange>(g,r,rStrict,c,lStrict,l);})
;

expr returns [ASTExprPtr n] {ASTExprPtr e;}
	: n=prodExpr (ADD e=prodExpr {n = new_ptr<ASTAdd>(n,e);} | SUB e=prodExpr {n = new_ptr<ASTSub>(n,e);})* 
;

forExpr returns [ASTExprPtr n] {ASTExprPtr e; ASTIdentifierLitPtr index; ASTExprPtr lbound; ASTExprPtr ubound; string op;}
	: "for" LPAREN index=identifier COMMA  lbound = expr COLON 
										   ubound = expr COMMA
				((ADD {op="+";})|(MUL {op="*";})) COMMA
				e = expr RPAREN { n = new_ptr<ASTForExpr>(index, lbound, ubound, op, e); }
;

prodExpr returns [ASTExprPtr n] {ASTExprPtr e;}
	: n=powExpr (MUL e=powExpr {n = new_ptr<ASTMul>(n,e);}| DIV e=powExpr {n = new_ptr<ASTDiv>(n,e);})*
;
powExpr returns [ASTExprPtr n] {ASTExprPtr e;}
	: n=unaryExpr (POW e=unaryExpr {n = new_ptr<ASTPow>(n,e);})?
;
unaryExpr returns [ASTExprPtr n] {}
	:(SUB n=baseExpr {n  = new_ptr<ASTNegative>(n);}) | n=baseExpr
;
baseExpr returns [ASTExprPtr n] {ASTIdentifierSubPtr id;}
	: i:INTLIT { n = new_ptr<ASTExprInt>(i->getText());}
	| id=subscriptIdentifier {n = new_ptr<ASTExprIdentifier>(id);}
	| LPAREN n=expr RPAREN 
	| n=forExpr
;

setDecl returns [ASTListDeclPtr n]
	: LTHAN n=idGeneralDeclList GTHAN
;

idSubDeclList returns [ASTListDeclIdentifierSubPtr n] {n = new_ptr<ASTListDeclIdentifierSub>(); ASTDeclIdentifierSubPtr i;}
	: i=subscriptIdentifierDecl {n->add(i);} (COMMA i=subscriptIdentifierDecl {n->add(i);})*
;

idGeneralDeclList returns [ASTListDeclPtr n] {n = new_ptr<ASTListDecl>(); ASTDeclGeneralPtr i;}
	: i=idDeclGeneral {n->add(i);} (COMMA i=idDeclGeneral {n->add(i);})*
;


idDeclList returns [ASTListDeclIdentifierLitPtr n] {n = new_ptr<ASTListDeclIdentifierLit>(); ASTDeclIdentifierLitPtr i;}
	: i=identifierDecl {n->add(i);} (COMMA i=identifierDecl {n->add(i);})*
;

idSubList returns [ASTListIdentifierSubPtr n] {n = new_ptr<ASTListIdentifierSub>(); ASTIdentifierSubPtr i;}
	: i=subscriptIdentifier {n->add(i);} (COMMA i=subscriptIdentifier {n->add(i);})*
;

idList returns [ASTListIdentifierLitPtr n] {n = new_ptr<ASTListIdentifierLit>(); ASTIdentifierLitPtr i;}
	: i=identifier {n->add(i);} (COMMA i=identifier {n->add(i);})*
;

idDeclGeneral returns [ASTDeclGeneralPtr n] {}
	: n=idDeclRange
	| n=subscriptIdentifierDecl
; 

// XXX: dont know if this is right
// this is the syntactic sugar stuff
idDeclRange returns [ASTDeclIDRangePtr n] {ASTDeclIdentifierLitPtr base; ASTExprPtr lbound; ASTExprPtr ubound;}
	: base=identifierDecl LBRACKET lbound = expr COLON ubound = expr RBRACKET {n = new_ptr<ASTDeclIDRange>(base, lbound, ubound);}
;

subscriptIdentifierDecl returns [ASTDeclIdentifierSubPtr n] {ASTIdentifierLitPtr base; string sub;}
	: base=identifier (SUBSCRIPT (t:ID {sub = t->getText();} | i:INTLIT { sub = i->getText();})) ? {n = new_ptr<ASTDeclIdentifierSub>(base,sub); }
;
	
identifierDecl returns [ASTDeclIdentifierLitPtr n]
	: t:ID {n = new_ptr<ASTDeclIdentifierLit>(t->getText(), t->getLine(), t->getColumn());}
;

subscriptIdentifier returns [ASTIdentifierSubPtr n] {ASTIdentifierLitPtr base; string sub;}
	: base=identifier (SUBSCRIPT (t:ID {sub = t->getText();} | i:INTLIT { sub = i->getText();} )) ? {n = new_ptr<ASTIdentifierSub>(base,sub);}
;
	
identifier returns [ASTIdentifierLitPtr n]
	: t:ID {n = new_ptr<ASTIdentifierLit>(t->getText(), t->getLine(), t->getColumn());}
;

// Lexical Analyzer
// --------------------------------------------------------------------------------

class ZKPLexer extends Lexer;

options {
   k=2;
}

LPAREN  :   '(';
RPAREN  :   ')';
LBRACKET :   '[';
RBRACKET :   ']';
LCURLY   :   '{';
RCURLY   :   '}';

COMMA    :   ',';
SCOLON   :   ';';
COLON    :   ':';

CEQUAL   :  ':''=';
EQUAL    :   '=';
LTHAN    :   '<';
GTHAN    :   '>';
LEQ      :  '<''=';
GEQ      :  '>''=';
//IMPLY    :  "=>";
//EQUIV    : "<=>";

ADD : '+';
SUB : '-';
MUL : '*';
DIV : '/';
POW : '^';

AMP : '&';


SUBSCRIPT : '_';


ID
	: ('a'..'z'|'A'..'Z')( 'a'..'z'|'A'..'Z'|'0'..'9')* 
; 

INTLIT 
	: ('0' | ('1'..'9')('0'..'9')*)
;
	
// whitespace
WS
	: ( ' ' 
    	| '\t'
    	| ( '\r''\n'| '\r' | '\n') {newline();}
    ) {$setType(ANTLR_USE_NAMESPACE(antlr)Token::SKIP);}
;

// \uFFFF is to account for EOF. A comment on the last line of the file (without a \r\n) _does_ happen.
CMT
	: '/''/' (~('\n'|'\r'|'\uFFFF'))* ('\r''\n' |'\r'| '\n' | '\uFFFF') {newline(); $setType(ANTLR_USE_NAMESPACE(antlr)Token::SKIP);}
;
