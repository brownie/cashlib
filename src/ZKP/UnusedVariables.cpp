
#include "UnusedVariables.h"		

void UnusedVariables::applyASTDeclIdentifierLit(ASTDeclIdentifierLitPtr n){
   	definedVars[n->getName()] = false;
}

void UnusedVariables::applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n){
   	definedVars[n->getName()] = false;
}

void UnusedVariables::applyASTDeclIDRange(ASTDeclIDRangePtr n){
	for(int i = n->getLBound(); i <= n->getUBound(); i++){
		definedVars[n->getName(i)] = false;
	}	
}

void UnusedVariables::applyASTIdentifierLit(ASTIdentifierLitPtr n){
   	definedVars[n->getName()] = true;
}

void UnusedVariables::applyASTIdentifierSub(ASTIdentifierSubPtr n){
   	definedVars[n->getName()] = true;
}

void UnusedVariables::apply(ASTNodePtr n) {
	// need to do two passes over tree
	n->visit(*this);
	n->visit(*this);
	for (def_variable_map::iterator it = definedVars.begin(); 
									it != definedVars.end(); ++it) {
		if (!it->second)
			cout << "Warning: variable " << it->first << " is never used" << endl;
	}
}
