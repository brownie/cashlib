#ifndef INC_ZKPLexer_hpp_
#define INC_ZKPLexer_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20090830): "zkp.g" -> "ZKPLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "ZKPParserTokenTypes.hpp"
#include <antlr/CharScanner.hpp>
#line 3 "zkp.g"

//#include "../NTL/ZZ.h"
//namespace="brownie" XXX use namespaces someday
#include "ASTNode.h"
#include "new_ptr.hpp"

#line 19 "ZKPLexer.hpp"
class CUSTOM_API ZKPLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public ZKPParserTokenTypes
{
#line 1 "zkp.g"
#line 23 "ZKPLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return true;
	}
public:
	ZKPLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	ZKPLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	ZKPLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	public: void mLPAREN(bool _createToken);
	public: void mRPAREN(bool _createToken);
	public: void mLBRACKET(bool _createToken);
	public: void mRBRACKET(bool _createToken);
	public: void mLCURLY(bool _createToken);
	public: void mRCURLY(bool _createToken);
	public: void mCOMMA(bool _createToken);
	public: void mSCOLON(bool _createToken);
	public: void mCOLON(bool _createToken);
	public: void mCEQUAL(bool _createToken);
	public: void mEQUAL(bool _createToken);
	public: void mLTHAN(bool _createToken);
	public: void mGTHAN(bool _createToken);
	public: void mLEQ(bool _createToken);
	public: void mGEQ(bool _createToken);
	public: void mADD(bool _createToken);
	public: void mSUB(bool _createToken);
	public: void mMUL(bool _createToken);
	public: void mDIV(bool _createToken);
	public: void mPOW(bool _createToken);
	public: void mAMP(bool _createToken);
	public: void mSUBSCRIPT(bool _createToken);
	public: void mID(bool _createToken);
	public: void mINTLIT(bool _createToken);
	public: void mWS(bool _createToken);
	public: void mCMT(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
};

#endif /*INC_ZKPLexer_hpp_*/
