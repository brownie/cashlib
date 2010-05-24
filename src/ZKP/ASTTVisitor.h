
#ifndef ASTTVISITOR_H_
#define ASTTVISITOR_H_

#include "ASTHVisitor.h"

class ASTTVisitor : public ASTHVisitor {
	public: 
		void applyASTNode(ASTNodePtr n){
			n->visitChildren(*this);
		}
};

#endif /*ASTTVISITOR_H_*/
