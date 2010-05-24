#ifndef __FOREXPANDER_H__
#define __FOREXPANDER_H__

#include "ASTNode.h"
#include "ASTTVisitor.h"
#include <stack>

/*! Applies & expands a tree-parsing "syntactic sugar" for() rule on
 * ASTExprs, replacing ASTFor nodes with subtrees of expanded ASTExprs
 * joined by the appropriate operator (either ASTMul or ASTAdd).
 * 
 * If variables are used in the upper/lower bound arguments of for(), they
 * must be set in the environment before ForExpander is called on the tree.
 *
 * Supports nesting, e.g.:
 *	 for(i, 1:5, *, for(j, 1:j, +, g_i^x_j)) 
 *	 for(i, 1:5, *, g_i^x_i * for(j, 1:j, +, h_j^y_j)) 
 *	   
 * This class mutates the tree, so that ASTFor doesn't have to
 * implement its own getExprType() and eval() logic.  Should be run
 * after the grammar, before the visitors.
 */

class ForExpander : public ASTTVisitor {
	public:
		ForExpander(Environment &env) : e(env) {}
		
		/*! look up lower and upper bounds of [LB:UB] */
		int lookupBound(ASTExprPtr bnode);
		
		/*! returns a subtree of expanded ASTExprs, 
		 * joined with ASTMul/ASTAdd nodes */
		ASTExprPtr replaceFor(ASTForExprPtr forNode);
	
		/*! if this is an ASTFor node, then replace with a subtree of 
		 * expanded ASTExprsPtr */
		void applyASTNode(ASTNodePtr n);
		
		/*! replace x_i with value of i if this IdentifierSub is inside a 
		 * for context */
		void applyASTIdentifierSub(ASTIdentifierSubPtr n);
		
		void applyASTListRelation(ASTListRelationPtr n);
		
		void applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n);
		
		vector<ASTRelationPtr> replaceFor(ASTForRelPtr forNode);
	
	private:
		Environment& e;
		boost::unordered_map<string, string> indexToCur;
		stack<ASTNodePtr> parent; // track the parent of each ASTNode
};

#endif
