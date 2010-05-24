
#ifndef _PRINTER_H_
#define _PRINTER_H_

#include "ASTTVisitor.h"
#include "ASTNode.h"

class Printer : public ASTTVisitor {

	public:
		Printer();

		string unparse(ASTNodePtr n);
		void clear() { unparsed.clear(); }

		void newLine();
		void newLinePlus();
		void newLineMinus();

		void applyASTList(ASTListPtr n);
		void applyASTListIdentifierLit(ASTListIdentifierLitPtr n);
		void applyASTListIdentifierSub(ASTListIdentifierSubPtr n);
	 	void applyASTListDeclIdentifierLit(ASTListDeclIdentifierLitPtr n);
	 	void applyASTListDeclIdentifierSub(ASTListDeclIdentifierSubPtr n);

		// expressions
	 	void applyASTPow(ASTPowPtr n);
	 	void applyASTMul(ASTMulPtr n);
	 	void applyASTDiv(ASTDivPtr n);
	 	void applyASTAdd(ASTAddPtr n);
	 	void applyASTSub(ASTSubPtr n);

		void applyASTExprInt(ASTExprIntPtr n);
		void applyASTExprIdentifier(ASTExprIdentifierPtr n);
		void applyASTNegative(ASTNegativePtr n);

		// relations	
		void applyASTEqual(ASTEqualPtr n);
		void applyASTDeclEqual(ASTDeclEqualPtr n);
		void applyASTCommitment(ASTCommitmentPtr n);
		void applyASTRange(ASTRangePtr n);

		// given
		void applyASTDeclElements(ASTDeclElementsPtr n);
		void applyASTDeclExponents(ASTDeclExponentsPtr n);
		void applyASTDeclRandExponents(ASTDeclRandExponentsPtr n);
		void applyASTDeclGroup(ASTDeclGroupPtr n);
		void applyASTDeclIntegers(ASTDeclIntegersPtr n);
		void applyASTProof(ASTProofPtr n);
		void applyASTComputation(ASTComputationPtr n);

		// declarations	
		void applyASTDeclIdentifierLit(ASTDeclIdentifierLitPtr n);
		void applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n);
		void applyASTDeclIDRange(ASTDeclIDRangePtr n);
	
		// other
		void applyASTIdentifierLit(ASTIdentifierLitPtr n);
		void applyASTIdentifierSub(ASTIdentifierSubPtr n);

		void applyASTForExpr(ASTForExprPtr n);
		void applyASTForRel(ASTForRelPtr n);
	
	protected:
		int depth;
		int tabSize;
		string tab;

		string unparsed;

	private:
		void visitThis(ASTNodePtr n);

		void printList(ASTListPtr list, const string &sep);
		void printListLineBreak(ASTListPtr list);
		
		string printSubscript(ASTDeclIdentifierSubPtr n);
		string printSubscript(ASTIdentifierSubPtr n);
		string printSubscript(ASTIdentifierLitPtr n, const string &sub);
		
		void printUnaryOp(ASTUnaryOpPtr n, const string &op);
		void printBinaryOp(ASTBinaryOpPtr n, const string &op);
};

#include <cxxabi.h>
inline string type_to_str(const type_info& v) {
	int status;
	char* n = abi::__cxa_demangle(v.name(), 0, 0, &status);
	string ret(n);
	free(n);
	return ret;
}

inline string node_to_str(ASTNodePtr n) {
	string ret;
	if (dynamic_pointer_cast<ASTList>(n)) { 
		ret += string("size ")
			+= lexical_cast<string>(dynamic_pointer_cast<ASTList>(n)->size()); 
	} else if (dynamic_pointer_cast<ASTIdentifierLit>(n)) {
		ret += dynamic_pointer_cast<ASTIdentifierLit>(n)->getName();
	} else if (dynamic_pointer_cast<ASTDeclIdentifierLit>(n)) { 
		ret += dynamic_pointer_cast<ASTDeclIdentifierLit>(n)->getName();
	} else if (dynamic_pointer_cast<ASTIdentifierSub>(n)) {
		ret += dynamic_pointer_cast<ASTIdentifierSub>(n)->getSub();
	} else if (dynamic_pointer_cast<ASTDeclIdentifierSub>(n)) { 
		ret += dynamic_pointer_cast<ASTDeclIdentifierSub>(n)->getSub();
	}
	return ret; 
}

class TreePrinter : public ASTTVisitor {
public:
	TreePrinter() : depth(0) {}
	void applyASTNode(ASTNodePtr n) {
		cout << depth << "\t";
		for (int i=0; i<depth; i++) 
			cout << ((i == depth-1 && dynamic_pointer_cast<ASTList>(n)) ? "+ " : "  ");
		cout << type_to_str(typeid(*n)) << " [" << node_to_str(n) << "]" << endl;
		depth++;
		n->visitChildren(*this);
		depth--;
	}

private:
	int depth;
	string prefix;
};

#endif /*_PRINTER_H_*/
