#include "ForExpander.h"
#include <deque>
#include "Environment.h"

int ForExpander::lookupBound(ASTExprPtr bnode) {
	ASTExprIdentifierPtr nIdent;
	ASTExprIntPtr nInt;
	ASTExprPtr expr;
	if ((nIdent = dynamic_pointer_cast<ASTExprIdentifier>(bnode))) {
		// scope: bound identifier from current for() context takes precedence 
		// over environment
		string name = nIdent->toString();
		if (indexToCur.count(name))
			return lexical_cast<int>(indexToCur.at(name));
		else 
			return to_int(e.variables.at(name));
	} else if ((nInt = dynamic_pointer_cast<ASTExprInt>(bnode))) {
		return to_int(nInt->getVal());
	} else if ((expr = dynamic_pointer_cast<ASTBinaryOp>(bnode)) !=0 ){
		Environment e;
		ZZ result = expr->eval(e);
		return to_int(result);
	} else {
		throw CashException(CashException::CE_PARSE_ERROR,
				"Bound on ASTForExpr is not an int or bound identifier: %s", 
				bnode->toString().c_str());
	}
}

ASTExprPtr ForExpander::replaceFor(ASTForExprPtr forNode) { 
	int lb = lookupBound(forNode->getLBound());
	int ub = lookupBound(forNode->getUBound());		
	deque<ASTExprPtr> toTree(ub-lb+1);
	ASTForExprPtr subFor;
	for(int x = lb; x<=ub; x++){
		toTree[x-lb] = forNode->getExpr()->clone();
		indexToCur[forNode->getIndex()->getName()] = lexical_cast<string>(x);
		if((subFor=dynamic_pointer_cast<ASTForExpr>(toTree[x-lb]))){
			toTree[x-lb]=replaceFor(subFor);
		} else {
			toTree[x-lb]->visit(*this);
		}
	}

	while(toTree.size() != 1){
		ASTExprPtr first = toTree.front();
		toTree.pop_front();
		ASTExprPtr second = toTree.front();
		toTree.pop_front();
		assert(first); assert(second);
		if(forNode->getOp() == "*"){
			toTree.push_back(new_ptr<ASTMul>(first, second));
		} else if(forNode->getOp() == "+") {
			toTree.push_back(new_ptr<ASTAdd>(first, second));
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
					"For instantiated with an invalid operator: %s", 
					forNode->getOp().c_str());
		}
	}
	return toTree[0];
}

vector<ASTRelationPtr> ForExpander::replaceFor(ASTForRelPtr forNode) { 
	int lb = lookupBound(forNode->getLBound());
	int ub = lookupBound(forNode->getUBound());
	vector<ASTRelationPtr> relations(ub-lb+1);
	for(int x = lb; x<=ub; x++){
		relations[x-lb] = forNode->getRelation()->clone();
		indexToCur[forNode->getIndex()->getName()] = lexical_cast<string>(x);
		relations[x-lb]->visit(*this);
	}
	return relations;
}

void ForExpander::applyASTNode(ASTNodePtr n) {	
	ASTForExprPtr isFor = dynamic_pointer_cast<ASTForExpr>(n);
	// if this is an ASTForExpr node, then replace with a subtree of 
	// expanded ASTExprs
	if (isFor) {
		ASTExprPtr nn = replaceFor(isFor); // new subtree of ASTExprs
		ASTEqualPtr equal;
		ASTDeclEqualPtr dequal;
		ASTBinaryOpPtr binop;
		ASTUnaryOpPtr unop;
		ASTForExprPtr fo;
		// set parent's pointer to the expanded exprs
		ASTNodePtr par = parent.top();
		if ((equal = dynamic_pointer_cast<ASTEqual>(par))) {
			equal->setExpr(nn);
		} else if((dequal = dynamic_pointer_cast<ASTDeclEqual>(par))){
			dequal->setExpr(nn);
		} else if((binop = dynamic_pointer_cast<ASTBinaryOp>(par))){
			if ((fo = dynamic_pointer_cast<ASTForExpr>(binop->getLHS()))){
				binop->setLHS(nn);
			} else {
				binop->setRHS(nn);
			}
		} else if((unop = dynamic_pointer_cast<ASTUnaryOp>(par))){
			unop->setExpr(nn);
		} else if((fo = dynamic_pointer_cast<ASTForExpr>(par))){
			fo->setExpr(nn);
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
								"Bad ASTForExpr parent");
		}
		// throw exception if not ASTEqual, DeclEqual, BinaryOp, 
		// UnaryOp, ASTForExpr
	} else {
		// store pointer to parent node locally, so we can replace the 
		// ASTFor node later
		parent.push(n);
		n->visitChildren(*this);
		parent.pop();
	}
}

void ForExpander::applyASTListRelation(ASTListRelationPtr n){
	parent.push(n);
	vector<vector<ASTRelationPtr> > toAdd;
	vector<ASTNodePtr> newList;
	ASTForRelPtr forRel;
	for(int x = 0; x < n->size(); x++){
		if((forRel = dynamic_pointer_cast<ASTForRel>(n->get(x)))){
			toAdd.push_back(replaceFor(forRel));
		} else {
			n->get(x)->visit(*this);
			newList.push_back(n->get(x));
		}
	}
	
	for(unsigned x = 0; x<toAdd.size(); x++) {
		for(unsigned y = 0; y<toAdd[x].size(); y++) {
			newList.push_back(toAdd[x][y]);
		}
	}
	n->setList(newList);
	parent.pop();
}

void ForExpander::applyASTIdentifierSub(ASTIdentifierSubPtr n){
	if(indexToCur.count(n->getSub())>0){
		n->setSub(indexToCur.at(n->getSub()));
	}
}

void ForExpander::applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n){
	if(indexToCur.count(n->getSub())>0){
		n->setSub(indexToCur.at(n->getSub()));
	}
}
	
