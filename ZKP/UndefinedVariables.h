
#ifndef _UNDEFINEDVARIABLES_H_
#define _UNDEFINEDVARIABLES_H_

#include "ASTTVisitor.h"
#include "ASTNode.h"
#include <boost/unordered_map.hpp>

/*! 
 * \brief This class is used to see if any variables are used without first
 * being defined
 */

using std::string;
typedef boost::unordered_map<string, bool> def_map;

class UndefinedVariables : public ASTTVisitor {

	public:

		/*! this visitor is used before we have actual values */
		UndefinedVariables() {}

		/*! finds all variables, if no exception is thrown then all 
		 * used variables have been defined! */
		void apply(ASTNodePtr n);

		/*! once a variable is declared, put it in the map */
		void applyASTDeclIdentifierLit(ASTDeclIdentifierLitPtr n);
		void applyASTDeclIdentifierSub(ASTDeclIdentifierSubPtr n);
		void applyASTDeclIDRange(ASTDeclIDRangePtr n);

		/*! once a variable is used, check to see that it has been defined;
		 * if not throw an exception */
		void applyASTIdentifierLit(ASTIdentifierLitPtr n);
		void applyASTIdentifierSub(ASTIdentifierSubPtr n);
	
	private:
		def_map definedVars;
};

#endif /*_UNDEFINEDVARIABLES_H_*/
