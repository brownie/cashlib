#include "ConstantProp.h"

void ConstantProp::applyASTForExpr(ASTForExprPtr n){
	ZZ result;
	Environment e;
	ASTExprIntPtr ne;
	try{
		result = n->getLBound()->eval(e);
		ne = new_ptr<ASTExprInt>(result);
		n->setLBound(ne);
	} catch(std::out_of_range e){
		n->getLBound()->visit(*this);
	}
	try{
		result = n->getUBound()->eval(e);
		ne = new_ptr<ASTExprInt>(result);
		n->setUBound(ne);
	} catch(std::out_of_range e){
		n->getUBound()->visit(*this);
	}
	try{
		result = n->getExpr()->eval(e);
		ne = new_ptr<ASTExprInt>(result);
		n->setExpr(ne);
	} catch(std::out_of_range e){
		n->getExpr()->visit(*this);
	}
}

void ConstantProp::applyASTForRel(ASTForRelPtr n){
	ZZ result;
	Environment e;
	ASTExprIntPtr ne;
	try{
		result = n->getLBound()->eval(e);
		ne = new_ptr<ASTExprInt>(result);
		n->setLBound(ne);
	} catch(std::out_of_range e){
		n->getLBound()->visit(*this);
	}
	try{
		result = n->getUBound()->eval(e);
		ne = new_ptr<ASTExprInt>(result);
		n->setUBound(ne);
	} catch(std::out_of_range e){
		n->getUBound()->visit(*this);
	}
}

void ConstantProp::applyASTDeclIDRange(ASTDeclIDRangePtr n){
	ZZ result;
	Environment e;
	ASTExprIntPtr ne;
	try{
		result = n->getLP()->eval(e);
		ne = new_ptr<ASTExprInt>(result);
		n->setLBound(ne);
	} catch(std::out_of_range e){
		n->getLP()->visit(*this);
	}
	try{
		result = n->getUP()->eval(e);
		ne = new_ptr<ASTExprInt>(result);
		n->setUBound(ne);
	} catch(std::out_of_range e){
		n->getUP()->visit(*this);
	}
}

void ConstantProp::applyASTBinaryOp(ASTBinaryOpPtr n){
	ZZ result;
	Environment e;
	ASTExprIntPtr ne;
	try{
		result = n->getLHS()->eval(e);
		ne = new_ptr<ASTExprInt>(result);
		n->setLHS(ne);
	} catch(std::out_of_range e){
		n->getLHS()->visit(*this);
	}
	try{
		result = n->getRHS()->eval(e);
		ne = new_ptr<ASTExprInt>(result);
		n->setRHS(ne);
	} catch(std::out_of_range e){
		n->getRHS()->visit(*this);
	}
}

void ConstantProp::applyASTUnaryOp(ASTUnaryOpPtr n){
	ZZ result;
	Environment e;
	try{
		result = n->getExpr()->eval(e);
		ASTExprIntPtr ne = new_ptr<ASTExprInt>(result);
		n->setExpr(ne);
	} catch(std::out_of_range e){
		n->getExpr()->visit(*this);
	}
}

