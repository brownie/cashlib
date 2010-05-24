
#include "Printer.h"
#include <boost/lexical_cast.hpp>

Printer::Printer() {
	depth = 0;
	tabSize = 3;
		
	tab = "";
	for(int k=0; k < tabSize; k++){
		tab += " ";
	}
}
string Printer::unparse(ASTNodePtr n) {
	
	//assert(n != null):"ASTNode is null";
	n->visit(*this);
	return unparsed;
}

void Printer::newLine(){
	unparsed.append("\n");
	for(int k=0; k<depth; k++)
		unparsed.append(tab);
}

void Printer::newLinePlus(){	
	depth++;
	newLine();
}

void Printer::newLineMinus(){		
	
	//assert(_depth > 0):"depth should never be negative";
	depth--;
	newLine();
}
	
void Printer::visitThis(ASTNodePtr n){
	if (n)
		n->visit(*this);
}

void Printer::printList(ASTListPtr list, const string &sep){
	
	bool first = true;
	for(int i = 0; i < list->size(); i++) {
		ASTNodePtr n = list->get(i);
		if (first){
			first = false;
		} else {
			unparsed.append(sep);
		}
		visitThis(n);
	}
}
		
void Printer::printListLineBreak(ASTListPtr list){
	
	for(int i = 0; i < list->size(); i++){
		visitThis(list->get(i));
		newLine();
	}
}
		
string Printer::printSubscript(ASTDeclIdentifierSubPtr n){
	return printSubscript(n->getBase(),n->getSub());
}
		
string Printer::printSubscript(ASTIdentifierSubPtr n){
	return printSubscript(n->getBase(),n->getSub());
}
		
string Printer::printSubscript(ASTIdentifierLitPtr n, const string &sub){ 
	if(sub.empty())
		return n->getName();//+"_{}";
	else
		return n->getName()+"_"+sub;
}
		
void Printer::printUnaryOp(ASTUnaryOpPtr n, const string &op){
	unparsed.append("(");
	unparsed.append(op);
	visitThis(n->getExpr());
	unparsed.append(")");
}
		
void Printer::printBinaryOp(ASTBinaryOpPtr n, const string &op){
	unparsed.append("(");
	visitThis(n->getLHS());
	unparsed.append(" "+op+" ");
	visitThis(n->getRHS());
	unparsed.append(")");
}

void Printer::applyASTList(ASTListPtr n){
	printListLineBreak(n);
}

void Printer::applyASTListIdentifierLit(ASTListIdentifierLitPtr n){
	printList(n, ", ");
}

void Printer::applyASTListIdentifierSub(ASTListIdentifierSubPtr n){
	printList(n, ", ");
}

void Printer::applyASTListDeclIdentifierLit(ASTListDeclIdentifierLitPtr n){
	printList(n, ", ");
}
	
void Printer::applyASTListDeclIdentifierSub(ASTListDeclIdentifierSubPtr n){
	printList(n, ", ");
}

void Printer::applyASTPow(ASTPowPtr n){
	printBinaryOp(n,"^");
}

void Printer::applyASTMul(ASTMulPtr n){
	printBinaryOp(n,"*");
}

void Printer::applyASTDiv(ASTDivPtr n){
	printBinaryOp(n,"/");
}

void Printer::applyASTAdd(ASTAddPtr n){
	printBinaryOp(n,"+");
}

void Printer::applyASTSub(ASTSubPtr n){
	printBinaryOp(n,"-");
}

void Printer::applyASTExprInt(ASTExprIntPtr n){	
	unparsed.append(lexical_cast<string>(n->getVal()));
}

void Printer::applyASTExprIdentifier(ASTExprIdentifierPtr n){	
	unparsed.append(printSubscript(n->getId()));
}

void Printer::applyASTNegative(ASTNegativePtr n){
	printUnaryOp(n,"-");
}

void Printer::applyASTEqual(ASTEqualPtr n){		
	visitThis(n->getId());
	unparsed.append(" = ");
	visitThis(n->getExpr());
}

void Printer::applyASTDeclEqual(ASTDeclEqualPtr n){
	visitThis(n->getId());
	unparsed.append(" := ");
	visitThis(n->getExpr());
}

void Printer::applyASTCommitment(ASTCommitmentPtr n){
	unparsed.append("Com(");
	visitThis(n->getId());
	unparsed.append(") = ");
	visitThis(n->getExpr());
}

void Printer::applyASTRange(ASTRangePtr n) {
	visitThis(n->getLower());
	unparsed.append(n->isLowerStrict() ? " < " : " <= ");
	visitThis(n->getCenter());
	unparsed.append(n->isUpperStrict() ? " < " : " <= ");
	visitThis(n->getUpper());
}

void Printer::applyASTDeclElements(ASTDeclElementsPtr n){		
	unparsed.append("elements in ");
	visitThis(n->getGroup());
	unparsed.append(": ");
	visitThis(n->getElements());
	newLinePlus();
	visitThis(n->getRelations());
	depth--;
}

void Printer::applyASTDeclExponents(ASTDeclExponentsPtr n){		
	unparsed.append("exponents in ");
	visitThis(n->getGroup());
	unparsed.append(": ");
	visitThis(n->getExponents());
}

void Printer::applyASTDeclRandExponents(ASTDeclRandExponentsPtr n){		
	unparsed.append("random exponents in ");
	visitThis(n->getGroup());
	unparsed.append(": ");
	visitThis(n->getExponents());
}

void Printer::applyASTDeclGroup(ASTDeclGroupPtr n){		
	unparsed.append("group: ");
	visitThis(n->getGroup());
	unparsed.append(" = <");
	visitThis(n->getItems());
	unparsed.append(">");
}

void Printer::applyASTDeclIntegers(ASTDeclIntegersPtr n){		
	unparsed.append("integers: ");
	visitThis(n->getInts());
}

void Printer::applyASTProof(ASTProofPtr n){		
	
	unparsed.append("proof: ");
	newLinePlus();
		
	unparsed.append("given: ");
	newLinePlus();
	visitThis(n->getGiven());
	newLineMinus();
			
	unparsed.append("prove knowledge of: ");
	newLinePlus();
	visitThis(n->getKnowledge());
	newLineMinus();
			
	unparsed.append("such that: ");
	newLinePlus();
	visitThis(n->getSuchThat());
	depth--;
		
	newLineMinus();
}

void Printer::applyASTComputation(ASTComputationPtr n){		
	
	unparsed.append("computation: ");
	newLinePlus();
		
	unparsed.append("given: ");
	newLinePlus();
	visitThis(n->getGiven());
	newLineMinus();
			
	unparsed.append("compute: ");
	newLinePlus();
	visitThis(n->getRandExponents());
	newLine();
	visitThis(n->getRelations());
	depth--;
		
	newLineMinus();
}

void Printer::applyASTDeclIdentifierLit(ASTDeclIdentifierLitPtr n){
	unparsed.append(n->getName());
}

void Printer::applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n){	
	unparsed.append(printSubscript(n));
}

void Printer::applyASTIdentifierLit(ASTIdentifierLitPtr n){	
	unparsed.append(n->getName());
}

void Printer::applyASTIdentifierSub(ASTIdentifierSubPtr n){
	unparsed.append(printSubscript(n));
}

void Printer::applyASTForExpr(ASTForExprPtr n) {
	unparsed.append("for(");
   	visitThis(n->getIndex());
	unparsed.append(", ");
   	visitThis(n->getLBound());
	unparsed.append(", ");
   	visitThis(n->getUBound());
	unparsed.append(", ");
	unparsed.append(n->getOp() + ", ");
   	visitThis(n->getExpr());
	unparsed.append(")");
}

void Printer::applyASTForRel(ASTForRelPtr n) {
	unparsed.append("for(");
   	visitThis(n->getIndex());
	unparsed.append(", ");
   	visitThis(n->getLBound());
	unparsed.append(", ");
   	visitThis(n->getUBound());
	unparsed.append(", ");
   	visitThis(n->getRelation());
	unparsed.append(")");
}

void Printer::applyASTDeclIDRange(ASTDeclIDRangePtr n){
	visitThis(n->getBase());
	unparsed.append("[");
	visitThis(n->getLP());
	unparsed.append(",");
	visitThis(n->getUP());
	unparsed.append("]");
}
