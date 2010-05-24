
#ifndef _BINDGROUPVALUES_H_
#define _BINDGROUPVALUES_H_

#include "ASTNode.h"
#include "ASTTVisitor.h"

/*!
 * \brief Used to associate generators with their actual values
 */

class BindGroupValues : public ASTTVisitor {

	public:
		/*! this visitor is used only after user has given actual values */
		BindGroupValues(Environment &e) : env(e) {}

		/*! map each generator to its numeric value */
		void applyASTDeclGroup(ASTDeclGroupPtr n);

		/*! map all generators to their values in the groups */
		void apply(ASTNodePtr n);

	private:
		Environment &env;
};

#endif /*_BINDGROUPVALUES_H_*/
