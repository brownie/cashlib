#include "UndefinedVariables.h"

void UndefinedVariables::apply(ASTNodePtr n){
	// need to do two passes over the tree because things can be declared
	// in 'prove knowledge of' block and used in 'given' block and vice
	// versa
	n->visit(*this);
	n->visit(*this);
	for (def_map::iterator it = definedVars.begin();
						   it != definedVars.end(); ++it) {
			if (!it->second)
				throw CashException(CashException::CE_PARSE_ERROR,
							"%s has not been defined", it->first.c_str());
	}
}
	
void UndefinedVariables::applyASTDeclIdentifierLit(ASTDeclIdentifierLitPtr n){
	definedVars[n->getName()] = true;
}

void UndefinedVariables::applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n){
	definedVars[n->getName()] = true;
}

void UndefinedVariables::applyASTDeclIDRange(ASTDeclIDRangePtr n){
	for(int i = n->getLBound(); i <= n->getUBound(); i++){
		definedVars[n->getName(i)] = true;
	}	
}

void UndefinedVariables::applyASTIdentifierLit(ASTIdentifierLitPtr n){
	definedVars[n->getName()] = (definedVars.count(n->getName()) != 0);
}
	
void UndefinedVariables::applyASTIdentifierSub(ASTIdentifierSubPtr n){
	definedVars[n->getName()] = (definedVars.count(n->getName()) != 0);
}
