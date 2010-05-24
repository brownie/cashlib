
#ifndef _EXPONENTSUB_H_
#define _EXPONENTSUB_H_

#include "ASTNode.h"
#include "ASTTVisitor.h"

class ExponentSub : public ASTTVisitor {

	public:
		ExponentSub(Environment &e) : env(e) {}

		void applyASTMul(ASTMulPtr n);
		void apply(ASTNodePtr n);
		
	private:
		Environment &env;
};

#endif /*_EXPONENTSUB_H_*/
