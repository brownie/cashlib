/* $ANTLR 2.7.7 (20090708): "zkp.g" -> "ZKPParser.cpp"$ */
#include "ZKPParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 1 "zkp.g"
#line 8 "ZKPParser.cpp"
ZKPParser::ZKPParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

ZKPParser::ZKPParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,5)
{
}

ZKPParser::ZKPParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

ZKPParser::ZKPParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,5)
{
}

ZKPParser::ZKPParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,5)
{
}

ASTSpecPtr  ZKPParser::spec() {
#line 22 "zkp.g"
	ASTSpecPtr n;
#line 37 "ZKPParser.cpp"
#line 22 "zkp.g"
	ASTComputationPtr c; ASTProofPtr p;
#line 40 "ZKPParser.cpp"
	
	{
	switch ( LA(1)) {
	case LITERAL_computation:
	{
		c=computation();
		break;
	}
	case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
	case LITERAL_proof:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	switch ( LA(1)) {
	case LITERAL_proof:
	{
		p=proof();
		break;
	}
	case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 23 "zkp.g"
	n = new_ptr<ASTSpec>(c, p);
#line 79 "ZKPParser.cpp"
	return n;
}

ASTComputationPtr  ZKPParser::computation() {
#line 26 "zkp.g"
	ASTComputationPtr n;
#line 86 "ZKPParser.cpp"
#line 26 "zkp.g"
	ASTListGivenPtr g; ASTListRandomsPtr c; ASTListRelationPtr r;
#line 89 "ZKPParser.cpp"
	
	match(LITERAL_computation);
	match(COLON);
	match(LITERAL_given);
	match(COLON);
	g=givenList();
	match(LITERAL_compute);
	match(COLON);
	c=computeRandomList();
	r=computeEquationList();
#line 32 "zkp.g"
	n = new_ptr<ASTComputation>(g,c,r);
#line 102 "ZKPParser.cpp"
	return n;
}

ASTProofPtr  ZKPParser::proof() {
#line 35 "zkp.g"
	ASTProofPtr n;
#line 109 "ZKPParser.cpp"
#line 35 "zkp.g"
	ASTListGivenPtr g; ASTListGivenPtr k; ASTListRelationPtr st;
#line 112 "ZKPParser.cpp"
	
	match(LITERAL_proof);
	match(COLON);
	match(LITERAL_given);
	match(COLON);
	g=givenList();
	match(LITERAL_prove);
	match(LITERAL_knowledge);
	match(LITERAL_of);
	match(COLON);
	k=knowledgeList();
	match(LITERAL_such);
	match(LITERAL_that);
	match(COLON);
	st=suchThatList();
#line 40 "zkp.g"
	n = new_ptr<ASTProof>(g,k,st);
#line 130 "ZKPParser.cpp"
	return n;
}

ASTListGivenPtr  ZKPParser::givenList() {
#line 58 "zkp.g"
	ASTListGivenPtr n;
#line 137 "ZKPParser.cpp"
#line 58 "zkp.g"
	n = new_ptr<ASTListGiven>(); ASTGivenPtr i;
#line 140 "ZKPParser.cpp"
	
	{ // ( ... )+
	int _cnt15=0;
	for (;;) {
		switch ( LA(1)) {
		case LITERAL_group:
		{
			i=groupDecl();
#line 59 "zkp.g"
			n->add(i);
#line 151 "ZKPParser.cpp"
			break;
		}
		case LITERAL_element:
		case LITERAL_elements:
		{
			i=elementsDecl();
#line 60 "zkp.g"
			n->add(i);
#line 160 "ZKPParser.cpp"
			break;
		}
		case LITERAL_exponent:
		case LITERAL_exponents:
		{
			i=exponentDecl();
#line 61 "zkp.g"
			n->add(i);
#line 169 "ZKPParser.cpp"
			break;
		}
		case LITERAL_integer:
		case LITERAL_integers:
		{
			i=integerDecl();
#line 62 "zkp.g"
			n->add(i);
#line 178 "ZKPParser.cpp"
			break;
		}
		default:
		{
			if ( _cnt15>=1 ) { goto _loop15; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		}
		_cnt15++;
	}
	_loop15:;
	}  // ( ... )+
	return n;
}

ASTListRandomsPtr  ZKPParser::computeRandomList() {
#line 75 "zkp.g"
	ASTListRandomsPtr n;
#line 196 "ZKPParser.cpp"
#line 75 "zkp.g"
	n = new_ptr<ASTListRandoms>(); ASTGivenPtr i;
#line 199 "ZKPParser.cpp"
	
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LITERAL_random)) {
			match(LITERAL_random);
			{
			switch ( LA(1)) {
			case LITERAL_exponent:
			case LITERAL_exponents:
			{
				i=randExponentDecl();
				break;
			}
			case LITERAL_prime:
			case LITERAL_primes:
			{
				i=randomPrimeDecl();
				break;
			}
			case LITERAL_integer:
			case LITERAL_integers:
			{
				i=randomBndDecl();
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
#line 78 "zkp.g"
			n->add(i);
#line 233 "ZKPParser.cpp"
		}
		else {
			goto _loop23;
		}
		
	}
	_loop23:;
	} // ( ... )*
	return n;
}

ASTListRelationPtr  ZKPParser::computeEquationList() {
#line 107 "zkp.g"
	ASTListRelationPtr n;
#line 248 "ZKPParser.cpp"
#line 107 "zkp.g"
	n = new_ptr<ASTListRelation>(); ASTRelationPtr i;
#line 251 "ZKPParser.cpp"
	
	i=computeEquation();
#line 108 "zkp.g"
	n->add(i);
#line 256 "ZKPParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LITERAL_for || LA(1) == LITERAL_commitment || LA(1) == ID)) {
			i=computeEquation();
#line 108 "zkp.g"
			n->add(i);
#line 263 "ZKPParser.cpp"
		}
		else {
			goto _loop39;
		}
		
	}
	_loop39:;
	} // ( ... )*
	return n;
}

ASTListGivenPtr  ZKPParser::knowledgeList() {
#line 54 "zkp.g"
	ASTListGivenPtr n;
#line 278 "ZKPParser.cpp"
#line 54 "zkp.g"
	n = new_ptr<ASTListGiven>(); ASTGivenPtr i;
#line 281 "ZKPParser.cpp"
	
	{ // ( ... )+
	int _cnt12=0;
	for (;;) {
		switch ( LA(1)) {
		case LITERAL_exponent:
		case LITERAL_exponents:
		{
			i=exponentDecl();
#line 55 "zkp.g"
			n->add(i);
#line 293 "ZKPParser.cpp"
			break;
		}
		case LITERAL_integer:
		case LITERAL_integers:
		{
			i=integerDecl();
#line 55 "zkp.g"
			n->add(i);
#line 302 "ZKPParser.cpp"
			break;
		}
		default:
		{
			if ( _cnt12>=1 ) { goto _loop12; } else {throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());}
		}
		}
		_cnt12++;
	}
	_loop12:;
	}  // ( ... )+
	return n;
}

ASTListRelationPtr  ZKPParser::suchThatList() {
#line 43 "zkp.g"
	ASTListRelationPtr n;
#line 320 "ZKPParser.cpp"
#line 43 "zkp.g"
	n = new_ptr<ASTListRelation>(); ASTRelationPtr i;
#line 323 "ZKPParser.cpp"
	
	i=suchThatRel();
#line 44 "zkp.g"
	n->add(i);
#line 328 "ZKPParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LITERAL_for || LA(1) == ID || LA(1) == LITERAL_range)) {
			i=suchThatRel();
#line 44 "zkp.g"
			n->add(i);
#line 335 "ZKPParser.cpp"
		}
		else {
			goto _loop8;
		}
		
	}
	_loop8:;
	} // ( ... )*
	return n;
}

ASTRelationPtr  ZKPParser::suchThatRel() {
#line 47 "zkp.g"
	ASTRelationPtr n;
#line 350 "ZKPParser.cpp"
	
	switch ( LA(1)) {
	case ID:
	{
		n=equalRelation();
		break;
	}
	case LITERAL_range:
	{
		n=rangeRelation();
		break;
	}
	case LITERAL_for:
	{
		n=forRelation();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

ASTEqualPtr  ZKPParser::equalRelation() {
#line 148 "zkp.g"
	ASTEqualPtr n;
#line 379 "ZKPParser.cpp"
#line 148 "zkp.g"
	ASTIdentifierSubPtr id; ASTExprPtr e;
#line 382 "ZKPParser.cpp"
	
	id=subscriptIdentifier();
	match(EQUAL);
	e=expr();
#line 149 "zkp.g"
	n = new_ptr<ASTEqual>(id,e);
#line 389 "ZKPParser.cpp"
	return n;
}

ASTRangePtr  ZKPParser::rangeRelation() {
#line 156 "zkp.g"
	ASTRangePtr n;
#line 396 "ZKPParser.cpp"
#line 156 "zkp.g"
	ASTIdentifierLitPtr g; ASTExprPtr l,c,r; 
										   bool lStrict = true; bool rStrict = true;
#line 400 "ZKPParser.cpp"
	
	match(LITERAL_range);
	match(LITERAL_in);
	g=identifier();
	match(COLON);
	l=expr();
	{
	switch ( LA(1)) {
	case LTHAN:
	case LEQ:
	{
		{
		switch ( LA(1)) {
		case LTHAN:
		{
			match(LTHAN);
			break;
		}
		case LEQ:
		{
			match(LEQ);
#line 158 "zkp.g"
			lStrict=false;
#line 424 "ZKPParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		c=expr();
		{
		switch ( LA(1)) {
		case LTHAN:
		{
			match(LTHAN);
			break;
		}
		case LEQ:
		{
			match(LEQ);
#line 159 "zkp.g"
			rStrict=false;
#line 446 "ZKPParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		r=expr();
#line 159 "zkp.g"
		n = new_ptr<ASTRange>(g,l,lStrict,c,rStrict,r);
#line 458 "ZKPParser.cpp"
		break;
	}
	case GTHAN:
	case GEQ:
	{
		{
		switch ( LA(1)) {
		case GTHAN:
		{
			match(GTHAN);
			break;
		}
		case GEQ:
		{
			match(GEQ);
#line 160 "zkp.g"
			lStrict=false;
#line 476 "ZKPParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		c=expr();
		{
		switch ( LA(1)) {
		case GTHAN:
		{
			match(GTHAN);
			break;
		}
		case GEQ:
		{
			match(GEQ);
#line 160 "zkp.g"
			rStrict=false;
#line 498 "ZKPParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		r=expr();
#line 161 "zkp.g"
		n = new_ptr<ASTRange>(g,r,rStrict,c,lStrict,l);
#line 510 "ZKPParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	return n;
}

ASTRelationPtr  ZKPParser::forRelation() {
#line 117 "zkp.g"
	ASTRelationPtr n;
#line 525 "ZKPParser.cpp"
#line 117 "zkp.g"
	ASTRelationPtr r; ASTIdentifierLitPtr index; ASTExprPtr lbound; ASTExprPtr ubound; string op;
#line 528 "ZKPParser.cpp"
	
	match(LITERAL_for);
	match(LPAREN);
	index=identifier();
	match(COMMA);
	lbound=expr();
	match(COLON);
	ubound=expr();
	match(COMMA);
	{
	switch ( LA(1)) {
	case LITERAL_range:
	{
		{
		r=rangeRelation();
		}
		break;
	}
	case ID:
	{
		{
		r=genEqual();
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(RPAREN);
#line 120 "zkp.g"
	n = new_ptr<ASTForRel>(index, lbound, ubound, r);
#line 563 "ZKPParser.cpp"
	return n;
}

ASTDeclExponentsPtr  ZKPParser::exponentDecl() {
#line 92 "zkp.g"
	ASTDeclExponentsPtr n;
#line 570 "ZKPParser.cpp"
#line 92 "zkp.g"
	ASTIdentifierLitPtr id; ASTListDeclPtr ids;
#line 573 "ZKPParser.cpp"
	
	{
	switch ( LA(1)) {
	case LITERAL_exponent:
	{
		match(LITERAL_exponent);
		break;
	}
	case LITERAL_exponents:
	{
		match(LITERAL_exponents);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(LITERAL_in);
	id=identifier();
	match(COLON);
	ids=idGeneralDeclList();
#line 93 "zkp.g"
	n = new_ptr<ASTDeclExponents>(id,ids);
#line 599 "ZKPParser.cpp"
	return n;
}

ASTDeclIntegersPtr  ZKPParser::integerDecl() {
#line 103 "zkp.g"
	ASTDeclIntegersPtr n;
#line 606 "ZKPParser.cpp"
#line 103 "zkp.g"
	ASTListDeclPtr ids;
#line 609 "ZKPParser.cpp"
	
	{
	switch ( LA(1)) {
	case LITERAL_integer:
	{
		match(LITERAL_integer);
		break;
	}
	case LITERAL_integers:
	{
		match(LITERAL_integers);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(COLON);
	ids=idGeneralDeclList();
#line 104 "zkp.g"
	n = new_ptr<ASTDeclIntegers>(ids);
#line 633 "ZKPParser.cpp"
	return n;
}

ASTDeclGroupPtr  ZKPParser::groupDecl() {
#line 82 "zkp.g"
	ASTDeclGroupPtr n;
#line 640 "ZKPParser.cpp"
#line 82 "zkp.g"
	ASTDeclIdentifierLitPtr id; 
										 ASTListDeclPtr ids;
										 ASTDeclIdentifierSubPtr mod;
#line 645 "ZKPParser.cpp"
	
	match(LITERAL_group);
	match(COLON);
	id=identifierDecl();
	{
	switch ( LA(1)) {
	case EQUAL:
	{
		match(EQUAL);
		ids=setDecl();
		break;
	}
	case LITERAL_compute:
	case LITERAL_prove:
	case LITERAL_integer:
	case LITERAL_integers:
	case LITERAL_group:
	case LITERAL_modulus:
	case LITERAL_exponent:
	case LITERAL_exponents:
	case LITERAL_element:
	case LITERAL_elements:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	switch ( LA(1)) {
	case LITERAL_modulus:
	{
		match(LITERAL_modulus);
		match(COLON);
		mod=subscriptIdentifierDecl();
		break;
	}
	case LITERAL_compute:
	case LITERAL_prove:
	case LITERAL_integer:
	case LITERAL_integers:
	case LITERAL_group:
	case LITERAL_exponent:
	case LITERAL_exponents:
	case LITERAL_element:
	case LITERAL_elements:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 85 "zkp.g"
	n = new_ptr<ASTDeclGroup>(id,ids,mod);
#line 706 "ZKPParser.cpp"
	return n;
}

ASTDeclElementsPtr  ZKPParser::elementsDecl() {
#line 99 "zkp.g"
	ASTDeclElementsPtr n;
#line 713 "ZKPParser.cpp"
#line 99 "zkp.g"
	ASTIdentifierLitPtr id; ASTListDeclPtr ids = new_ptr<ASTListDecl>(); ASTListRelationPtr el;
#line 716 "ZKPParser.cpp"
	
	{
	switch ( LA(1)) {
	case LITERAL_element:
	{
		match(LITERAL_element);
		break;
	}
	case LITERAL_elements:
	{
		match(LITERAL_elements);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(LITERAL_in);
	id=identifier();
	match(COLON);
	{
	switch ( LA(1)) {
	case ID:
	{
		ids=idGeneralDeclList();
		break;
	}
	case LITERAL_compute:
	case LITERAL_prove:
	case LITERAL_integer:
	case LITERAL_integers:
	case LITERAL_group:
	case LITERAL_exponent:
	case LITERAL_exponents:
	case LITERAL_element:
	case LITERAL_elements:
	case LITERAL_for:
	case LITERAL_commitment:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	switch ( LA(1)) {
	case LITERAL_for:
	case LITERAL_commitment:
	{
		el=elementsEquationList();
		break;
	}
	case LITERAL_compute:
	case LITERAL_prove:
	case LITERAL_integer:
	case LITERAL_integers:
	case LITERAL_group:
	case LITERAL_exponent:
	case LITERAL_exponents:
	case LITERAL_element:
	case LITERAL_elements:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 100 "zkp.g"
	n = new_ptr<ASTDeclElements>(id,ids,el);
#line 794 "ZKPParser.cpp"
	return n;
}

ASTRandomBndPtr  ZKPParser::randomBndDecl() {
#line 65 "zkp.g"
	ASTRandomBndPtr n;
#line 801 "ZKPParser.cpp"
#line 65 "zkp.g"
	ASTExprPtr lbound, ubound; ASTListDeclPtr l;
#line 804 "ZKPParser.cpp"
	
	{
	switch ( LA(1)) {
	case LITERAL_integer:
	{
		match(LITERAL_integer);
		break;
	}
	case LITERAL_integers:
	{
		match(LITERAL_integers);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(LITERAL_in);
	match(LBRACKET);
	lbound=expr();
	match(COMMA);
	ubound=expr();
	match(RPAREN);
	match(COLON);
	l=idGeneralDeclList();
#line 67 "zkp.g"
	n = new_ptr<ASTRandomBnd>(lbound, ubound, l);
#line 834 "ZKPParser.cpp"
	return n;
}

ASTExprPtr  ZKPParser::expr() {
#line 164 "zkp.g"
	ASTExprPtr n;
#line 841 "ZKPParser.cpp"
#line 164 "zkp.g"
	ASTExprPtr e;
#line 844 "ZKPParser.cpp"
	
	n=prodExpr();
	{ // ( ... )*
	for (;;) {
		switch ( LA(1)) {
		case ADD:
		{
			match(ADD);
			e=prodExpr();
#line 165 "zkp.g"
			n = new_ptr<ASTAdd>(n,e);
#line 856 "ZKPParser.cpp"
			break;
		}
		case SUB:
		{
			match(SUB);
			e=prodExpr();
#line 165 "zkp.g"
			n = new_ptr<ASTSub>(n,e);
#line 865 "ZKPParser.cpp"
			break;
		}
		default:
		{
			goto _loop65;
		}
		}
	}
	_loop65:;
	} // ( ... )*
	return n;
}

ASTListDeclPtr  ZKPParser::idGeneralDeclList() {
#line 199 "zkp.g"
	ASTListDeclPtr n;
#line 882 "ZKPParser.cpp"
#line 199 "zkp.g"
	n = new_ptr<ASTListDecl>(); ASTDeclGeneralPtr i;
#line 885 "ZKPParser.cpp"
	
	i=idDeclGeneral();
#line 200 "zkp.g"
	n->add(i);
#line 890 "ZKPParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			i=idDeclGeneral();
#line 200 "zkp.g"
			n->add(i);
#line 898 "ZKPParser.cpp"
		}
		else {
			goto _loop84;
		}
		
	}
	_loop84:;
	} // ( ... )*
	return n;
}

ASTRandomPrimePtr  ZKPParser::randomPrimeDecl() {
#line 70 "zkp.g"
	ASTRandomPrimePtr n;
#line 913 "ZKPParser.cpp"
#line 70 "zkp.g"
	ASTExprPtr length; ASTListDeclPtr l;
#line 916 "ZKPParser.cpp"
	
	{
	switch ( LA(1)) {
	case LITERAL_prime:
	{
		match(LITERAL_prime);
		break;
	}
	case LITERAL_primes:
	{
		match(LITERAL_primes);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(LITERAL_of);
	match(LITERAL_length);
	length=expr();
	match(COLON);
	l=idGeneralDeclList();
#line 72 "zkp.g"
	n = new_ptr<ASTRandomPrime>(length, l);
#line 943 "ZKPParser.cpp"
	return n;
}

ASTDeclRandExponentsPtr  ZKPParser::randExponentDecl() {
#line 88 "zkp.g"
	ASTDeclRandExponentsPtr n;
#line 950 "ZKPParser.cpp"
#line 88 "zkp.g"
	ASTIdentifierLitPtr id; ASTListDeclPtr ids;
#line 953 "ZKPParser.cpp"
	
	{
	switch ( LA(1)) {
	case LITERAL_exponent:
	{
		match(LITERAL_exponent);
		break;
	}
	case LITERAL_exponents:
	{
		match(LITERAL_exponents);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(LITERAL_in);
	id=identifier();
	match(COLON);
	ids=idGeneralDeclList();
#line 89 "zkp.g"
	n = new_ptr<ASTDeclRandExponents>(id,ids);
#line 979 "ZKPParser.cpp"
	return n;
}

ASTDeclIdentifierLitPtr  ZKPParser::identifierDecl() {
#line 231 "zkp.g"
	ASTDeclIdentifierLitPtr n;
#line 986 "ZKPParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	t = LT(1);
	match(ID);
#line 232 "zkp.g"
	n = new_ptr<ASTDeclIdentifierLit>(t->getText(), t->getLine(), t->getColumn());
#line 993 "ZKPParser.cpp"
	return n;
}

ASTListDeclPtr  ZKPParser::setDecl() {
#line 191 "zkp.g"
	ASTListDeclPtr n;
#line 1000 "ZKPParser.cpp"
	
	match(LTHAN);
	n=idGeneralDeclList();
	match(GTHAN);
	return n;
}

ASTDeclIdentifierSubPtr  ZKPParser::subscriptIdentifierDecl() {
#line 227 "zkp.g"
	ASTDeclIdentifierSubPtr n;
#line 1011 "ZKPParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 227 "zkp.g"
	ASTIdentifierLitPtr base; string sub;
#line 1016 "ZKPParser.cpp"
	
	base=identifier();
	{
	switch ( LA(1)) {
	case SUBSCRIPT:
	{
		match(SUBSCRIPT);
		{
		switch ( LA(1)) {
		case ID:
		{
			t = LT(1);
			match(ID);
#line 228 "zkp.g"
			sub = t->getText();
#line 1032 "ZKPParser.cpp"
			break;
		}
		case INTLIT:
		{
			i = LT(1);
			match(INTLIT);
#line 228 "zkp.g"
			sub = i->getText();
#line 1041 "ZKPParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		break;
	}
	case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
	case LITERAL_compute:
	case LITERAL_prove:
	case LITERAL_such:
	case LITERAL_integer:
	case LITERAL_integers:
	case COMMA:
	case LITERAL_random:
	case LITERAL_group:
	case LITERAL_exponent:
	case LITERAL_exponents:
	case LITERAL_element:
	case LITERAL_elements:
	case LITERAL_for:
	case LITERAL_commitment:
	case ID:
	case CEQUAL:
	case GTHAN:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 228 "zkp.g"
	n = new_ptr<ASTDeclIdentifierSub>(base,sub);
#line 1081 "ZKPParser.cpp"
	return n;
}

ASTIdentifierLitPtr  ZKPParser::identifier() {
#line 239 "zkp.g"
	ASTIdentifierLitPtr n;
#line 1088 "ZKPParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	t = LT(1);
	match(ID);
#line 240 "zkp.g"
	n = new_ptr<ASTIdentifierLit>(t->getText(), t->getLine(), t->getColumn());
#line 1095 "ZKPParser.cpp"
	return n;
}

ASTListRelationPtr  ZKPParser::elementsEquationList() {
#line 129 "zkp.g"
	ASTListRelationPtr n;
#line 1102 "ZKPParser.cpp"
#line 129 "zkp.g"
	n = new_ptr<ASTListRelation>(); ASTRelationPtr i;
#line 1105 "ZKPParser.cpp"
	
	i=elementsEquation();
#line 130 "zkp.g"
	n->add(i);
#line 1110 "ZKPParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LITERAL_for || LA(1) == LITERAL_commitment)) {
			i=elementsEquation();
#line 130 "zkp.g"
			n->add(i);
#line 1117 "ZKPParser.cpp"
		}
		else {
			goto _loop48;
		}
		
	}
	_loop48:;
	} // ( ... )*
	return n;
}

ASTRelationPtr  ZKPParser::computeEquation() {
#line 111 "zkp.g"
	ASTRelationPtr n;
#line 1132 "ZKPParser.cpp"
	
	switch ( LA(1)) {
	case ID:
	{
		n=equalDeclRelation();
		break;
	}
	case LITERAL_commitment:
	{
		n=comRelation();
		break;
	}
	case LITERAL_for:
	{
		n=forRelation();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

ASTDeclEqualPtr  ZKPParser::equalDeclRelation() {
#line 152 "zkp.g"
	ASTDeclEqualPtr n;
#line 1161 "ZKPParser.cpp"
#line 152 "zkp.g"
	ASTDeclIdentifierSubPtr id; ASTExprPtr e;
#line 1164 "ZKPParser.cpp"
	
	id=subscriptIdentifierDecl();
	match(CEQUAL);
	e=expr();
#line 153 "zkp.g"
	n = new_ptr<ASTDeclEqual>(id,e);
#line 1171 "ZKPParser.cpp"
	return n;
}

ASTCommitmentPtr  ZKPParser::comRelation() {
#line 138 "zkp.g"
	ASTCommitmentPtr n;
#line 1178 "ZKPParser.cpp"
#line 138 "zkp.g"
	ASTListIdentifierSubPtr coms; ASTIdentifierSubPtr id; ASTExprPtr e;
#line 1181 "ZKPParser.cpp"
	
	match(LITERAL_commitment);
	match(LITERAL_to);
	coms=idSubList();
	match(COLON);
	id=subscriptIdentifier();
	match(EQUAL);
	e=expr();
#line 139 "zkp.g"
	n = new_ptr<ASTCommitment>(coms, id, e);
#line 1192 "ZKPParser.cpp"
	return n;
}

ASTRelationPtr  ZKPParser::genEqual() {
#line 142 "zkp.g"
	ASTRelationPtr n;
#line 1199 "ZKPParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 142 "zkp.g"
	ASTIdentifierLitPtr base; string sub; ASTExprPtr e;
#line 1204 "ZKPParser.cpp"
	
	base=identifier();
	{
	switch ( LA(1)) {
	case SUBSCRIPT:
	{
		match(SUBSCRIPT);
		{
		switch ( LA(1)) {
		case ID:
		{
			t = LT(1);
			match(ID);
#line 143 "zkp.g"
			sub = t->getText();
#line 1220 "ZKPParser.cpp"
			break;
		}
		case INTLIT:
		{
			i = LT(1);
			match(INTLIT);
#line 143 "zkp.g"
			sub = i->getText();
#line 1229 "ZKPParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		break;
	}
	case EQUAL:
	case CEQUAL:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	switch ( LA(1)) {
	case EQUAL:
	{
		match(EQUAL);
		e=expr();
#line 144 "zkp.g"
		n = new_ptr<ASTEqual>(new_ptr<ASTIdentifierSub>(base,sub), e);
#line 1259 "ZKPParser.cpp"
		break;
	}
	case CEQUAL:
	{
		match(CEQUAL);
		e=expr();
#line 145 "zkp.g"
		n = new_ptr<ASTDeclEqual>(new_ptr<ASTDeclIdentifierSub>(base,sub), e);
#line 1268 "ZKPParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	return n;
}

ASTRelationPtr  ZKPParser::forCom() {
#line 123 "zkp.g"
	ASTRelationPtr n;
#line 1283 "ZKPParser.cpp"
#line 123 "zkp.g"
	ASTRelationPtr r; ASTIdentifierLitPtr index; ASTExprPtr lbound; ASTExprPtr ubound; string op;
#line 1286 "ZKPParser.cpp"
	
	match(LITERAL_for);
	match(LPAREN);
	index=identifier();
	match(COMMA);
	lbound=expr();
	match(COLON);
	ubound=expr();
	match(COMMA);
	r=comRelation();
	match(RPAREN);
#line 126 "zkp.g"
	n = new_ptr<ASTForRel>(index, lbound, ubound, r);
#line 1300 "ZKPParser.cpp"
	return n;
}

ASTRelationPtr  ZKPParser::elementsEquation() {
#line 133 "zkp.g"
	ASTRelationPtr n;
#line 1307 "ZKPParser.cpp"
	
	switch ( LA(1)) {
	case LITERAL_commitment:
	{
		n=comRelation();
		break;
	}
	case LITERAL_for:
	{
		n=forCom();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

ASTListIdentifierSubPtr  ZKPParser::idSubList() {
#line 208 "zkp.g"
	ASTListIdentifierSubPtr n;
#line 1331 "ZKPParser.cpp"
#line 208 "zkp.g"
	n = new_ptr<ASTListIdentifierSub>(); ASTIdentifierSubPtr i;
#line 1334 "ZKPParser.cpp"
	
	i=subscriptIdentifier();
#line 209 "zkp.g"
	n->add(i);
#line 1339 "ZKPParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			i=subscriptIdentifier();
#line 209 "zkp.g"
			n->add(i);
#line 1347 "ZKPParser.cpp"
		}
		else {
			goto _loop90;
		}
		
	}
	_loop90:;
	} // ( ... )*
	return n;
}

ASTIdentifierSubPtr  ZKPParser::subscriptIdentifier() {
#line 235 "zkp.g"
	ASTIdentifierSubPtr n;
#line 1362 "ZKPParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  t = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 235 "zkp.g"
	ASTIdentifierLitPtr base; string sub;
#line 1367 "ZKPParser.cpp"
	
	base=identifier();
	{
	switch ( LA(1)) {
	case SUBSCRIPT:
	{
		match(SUBSCRIPT);
		{
		switch ( LA(1)) {
		case ID:
		{
			t = LT(1);
			match(ID);
#line 236 "zkp.g"
			sub = t->getText();
#line 1383 "ZKPParser.cpp"
			break;
		}
		case INTLIT:
		{
			i = LT(1);
			match(INTLIT);
#line 236 "zkp.g"
			sub = i->getText();
#line 1392 "ZKPParser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		break;
	}
	case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
	case COLON:
	case LITERAL_compute:
	case LITERAL_proof:
	case LITERAL_prove:
	case LITERAL_integer:
	case LITERAL_integers:
	case COMMA:
	case RPAREN:
	case LITERAL_group:
	case EQUAL:
	case LITERAL_exponent:
	case LITERAL_exponents:
	case LITERAL_element:
	case LITERAL_elements:
	case LITERAL_for:
	case LITERAL_commitment:
	case ID:
	case LITERAL_range:
	case LTHAN:
	case LEQ:
	case GTHAN:
	case GEQ:
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case POW:
	case RBRACKET:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 236 "zkp.g"
	n = new_ptr<ASTIdentifierSub>(base,sub);
#line 1443 "ZKPParser.cpp"
	return n;
}

ASTExprPtr  ZKPParser::prodExpr() {
#line 175 "zkp.g"
	ASTExprPtr n;
#line 1450 "ZKPParser.cpp"
#line 175 "zkp.g"
	ASTExprPtr e;
#line 1453 "ZKPParser.cpp"
	
	n=powExpr();
	{ // ( ... )*
	for (;;) {
		switch ( LA(1)) {
		case MUL:
		{
			match(MUL);
			e=powExpr();
#line 176 "zkp.g"
			n = new_ptr<ASTMul>(n,e);
#line 1465 "ZKPParser.cpp"
			break;
		}
		case DIV:
		{
			match(DIV);
			e=powExpr();
#line 176 "zkp.g"
			n = new_ptr<ASTDiv>(n,e);
#line 1474 "ZKPParser.cpp"
			break;
		}
		default:
		{
			goto _loop72;
		}
		}
	}
	_loop72:;
	} // ( ... )*
	return n;
}

ASTExprPtr  ZKPParser::forExpr() {
#line 168 "zkp.g"
	ASTExprPtr n;
#line 1491 "ZKPParser.cpp"
#line 168 "zkp.g"
	ASTExprPtr e; ASTIdentifierLitPtr index; ASTExprPtr lbound; ASTExprPtr ubound; string op;
#line 1494 "ZKPParser.cpp"
	
	match(LITERAL_for);
	match(LPAREN);
	index=identifier();
	match(COMMA);
	lbound=expr();
	match(COLON);
	ubound=expr();
	match(COMMA);
	{
	switch ( LA(1)) {
	case ADD:
	{
		{
		match(ADD);
#line 171 "zkp.g"
		op="+";
#line 1512 "ZKPParser.cpp"
		}
		break;
	}
	case MUL:
	{
		{
		match(MUL);
#line 171 "zkp.g"
		op="*";
#line 1522 "ZKPParser.cpp"
		}
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(COMMA);
	e=expr();
	match(RPAREN);
#line 172 "zkp.g"
	n = new_ptr<ASTForExpr>(index, lbound, ubound, op, e);
#line 1537 "ZKPParser.cpp"
	return n;
}

ASTExprPtr  ZKPParser::powExpr() {
#line 178 "zkp.g"
	ASTExprPtr n;
#line 1544 "ZKPParser.cpp"
#line 178 "zkp.g"
	ASTExprPtr e;
#line 1547 "ZKPParser.cpp"
	
	n=unaryExpr();
	{
	switch ( LA(1)) {
	case POW:
	{
		match(POW);
		e=unaryExpr();
#line 179 "zkp.g"
		n = new_ptr<ASTPow>(n,e);
#line 1558 "ZKPParser.cpp"
		break;
	}
	case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
	case COLON:
	case LITERAL_compute:
	case LITERAL_proof:
	case LITERAL_prove:
	case LITERAL_integer:
	case LITERAL_integers:
	case COMMA:
	case RPAREN:
	case LITERAL_group:
	case LITERAL_exponent:
	case LITERAL_exponents:
	case LITERAL_element:
	case LITERAL_elements:
	case LITERAL_for:
	case LITERAL_commitment:
	case ID:
	case LITERAL_range:
	case LTHAN:
	case LEQ:
	case GTHAN:
	case GEQ:
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case RBRACKET:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	return n;
}

ASTExprPtr  ZKPParser::unaryExpr() {
#line 181 "zkp.g"
	ASTExprPtr n;
#line 1603 "ZKPParser.cpp"
#line 181 "zkp.g"
#line 1605 "ZKPParser.cpp"
	
	switch ( LA(1)) {
	case SUB:
	{
		{
		match(SUB);
		n=baseExpr();
#line 182 "zkp.g"
		n  = new_ptr<ASTNegative>(n);
#line 1615 "ZKPParser.cpp"
		}
		break;
	}
	case LITERAL_for:
	case LPAREN:
	case ID:
	case INTLIT:
	{
		n=baseExpr();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

ASTExprPtr  ZKPParser::baseExpr() {
#line 184 "zkp.g"
	ASTExprPtr n;
#line 1638 "ZKPParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  i = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 184 "zkp.g"
	ASTIdentifierSubPtr id;
#line 1642 "ZKPParser.cpp"
	
	switch ( LA(1)) {
	case INTLIT:
	{
		i = LT(1);
		match(INTLIT);
#line 185 "zkp.g"
		n = new_ptr<ASTExprInt>(i->getText());
#line 1651 "ZKPParser.cpp"
		break;
	}
	case ID:
	{
		id=subscriptIdentifier();
#line 186 "zkp.g"
		n = new_ptr<ASTExprIdentifier>(id);
#line 1659 "ZKPParser.cpp"
		break;
	}
	case LPAREN:
	{
		match(LPAREN);
		n=expr();
		match(RPAREN);
		break;
	}
	case LITERAL_for:
	{
		n=forExpr();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

ASTListDeclIdentifierSubPtr  ZKPParser::idSubDeclList() {
#line 195 "zkp.g"
	ASTListDeclIdentifierSubPtr n;
#line 1685 "ZKPParser.cpp"
#line 195 "zkp.g"
	n = new_ptr<ASTListDeclIdentifierSub>(); ASTDeclIdentifierSubPtr i;
#line 1688 "ZKPParser.cpp"
	
	i=subscriptIdentifierDecl();
#line 196 "zkp.g"
	n->add(i);
#line 1693 "ZKPParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			i=subscriptIdentifierDecl();
#line 196 "zkp.g"
			n->add(i);
#line 1701 "ZKPParser.cpp"
		}
		else {
			goto _loop81;
		}
		
	}
	_loop81:;
	} // ( ... )*
	return n;
}

ASTDeclGeneralPtr  ZKPParser::idDeclGeneral() {
#line 216 "zkp.g"
	ASTDeclGeneralPtr n;
#line 1716 "ZKPParser.cpp"
#line 216 "zkp.g"
#line 1718 "ZKPParser.cpp"
	
	if ((LA(1) == ID) && (LA(2) == LBRACKET)) {
		n=idDeclRange();
	}
	else if ((LA(1) == ID) && (_tokenSet_0.member(LA(2)))) {
		n=subscriptIdentifierDecl();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return n;
}

ASTListDeclIdentifierLitPtr  ZKPParser::idDeclList() {
#line 204 "zkp.g"
	ASTListDeclIdentifierLitPtr n;
#line 1736 "ZKPParser.cpp"
#line 204 "zkp.g"
	n = new_ptr<ASTListDeclIdentifierLit>(); ASTDeclIdentifierLitPtr i;
#line 1739 "ZKPParser.cpp"
	
	i=identifierDecl();
#line 205 "zkp.g"
	n->add(i);
#line 1744 "ZKPParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			i=identifierDecl();
#line 205 "zkp.g"
			n->add(i);
#line 1752 "ZKPParser.cpp"
		}
		else {
			goto _loop87;
		}
		
	}
	_loop87:;
	} // ( ... )*
	return n;
}

ASTListIdentifierLitPtr  ZKPParser::idList() {
#line 212 "zkp.g"
	ASTListIdentifierLitPtr n;
#line 1767 "ZKPParser.cpp"
#line 212 "zkp.g"
	n = new_ptr<ASTListIdentifierLit>(); ASTIdentifierLitPtr i;
#line 1770 "ZKPParser.cpp"
	
	i=identifier();
#line 213 "zkp.g"
	n->add(i);
#line 1775 "ZKPParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			i=identifier();
#line 213 "zkp.g"
			n->add(i);
#line 1783 "ZKPParser.cpp"
		}
		else {
			goto _loop93;
		}
		
	}
	_loop93:;
	} // ( ... )*
	return n;
}

ASTDeclIDRangePtr  ZKPParser::idDeclRange() {
#line 223 "zkp.g"
	ASTDeclIDRangePtr n;
#line 1798 "ZKPParser.cpp"
#line 223 "zkp.g"
	ASTDeclIdentifierLitPtr base; ASTExprPtr lbound; ASTExprPtr ubound;
#line 1801 "ZKPParser.cpp"
	
	base=identifierDecl();
	match(LBRACKET);
	lbound=expr();
	match(COLON);
	ubound=expr();
	match(RBRACKET);
#line 224 "zkp.g"
	n = new_ptr<ASTDeclIDRange>(base, lbound, ubound);
#line 1811 "ZKPParser.cpp"
	return n;
}

void ZKPParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* ZKPParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"\"computation\"",
	"COLON",
	"\"given\"",
	"\"compute\"",
	"\"proof\"",
	"\"prove\"",
	"\"knowledge\"",
	"\"of\"",
	"\"such\"",
	"\"that\"",
	"\"integer\"",
	"\"integers\"",
	"\"in\"",
	"LBRACKET",
	"COMMA",
	"RPAREN",
	"\"prime\"",
	"\"primes\"",
	"\"length\"",
	"\"random\"",
	"\"group\"",
	"EQUAL",
	"\"modulus\"",
	"\"exponent\"",
	"\"exponents\"",
	"\"element\"",
	"\"elements\"",
	"\"for\"",
	"LPAREN",
	"\"commitment\"",
	"\"to\"",
	"SUBSCRIPT",
	"ID",
	"INTLIT",
	"CEQUAL",
	"\"range\"",
	"LTHAN",
	"LEQ",
	"GTHAN",
	"GEQ",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"POW",
	"RBRACKET",
	"LCURLY",
	"RCURLY",
	"SCOLON",
	"AMP",
	"WS",
	"CMT",
	0
};

const unsigned long ZKPParser::_tokenSet_0_data_[] = { 4186231424UL, 1050UL, 0UL, 0UL };
// "compute" "prove" "such" "integer" "integers" COMMA "random" "group" 
// "exponent" "exponents" "element" "elements" "for" "commitment" SUBSCRIPT 
// ID GTHAN 
const ANTLR_USE_NAMESPACE(antlr)BitSet ZKPParser::_tokenSet_0(_tokenSet_0_data_,4);


