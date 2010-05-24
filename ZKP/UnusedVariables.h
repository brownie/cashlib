
#ifndef _UNUSEDVARIABLES_H_
#define _UNUSEDVARIABLES_H_

#include "ASTTVisitor.h"
#include "ASTNode.h"
#include <boost/unordered_map.hpp>

/*!
 * \brief This class will warn the user if a variable is declared but never
 * used 
 */

typedef boost::unordered_map<string, bool> def_variable_map;

class UnusedVariables : public ASTTVisitor {

	public:
		/*! this visitor is used before we have actual values */
		UnusedVariables() {}

		/*! when node is declared, put it in the map (as unused) */
		void applyASTDeclIdentifierLit(ASTDeclIdentifierLitPtr n);
		void applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n);
		void applyASTDeclIDRange(ASTDeclIDRangePtr n);
		
		/*! once a variable is used, change its entry in the map to true;
		 * because we run this after UndefinedVariables can assume all
		 * variables that are being used have been defined */
		void applyASTIdentifierLit(ASTIdentifierLitPtr n);
		void applyASTIdentifierSub(ASTIdentifierSubPtr n);

		/*! will go through all variable declarations/usages and warn the 
		 * user if a variable is being defined but not used */
		void apply(ASTNodePtr n);

	private:
		def_variable_map definedVars;

};

#endif /*_UNUSEDVARIABLES_H_*/
