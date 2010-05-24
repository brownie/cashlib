
#ifndef _DESCRIBERELATIONS_H_
#define _DESCRIBERELATIONS_H_

#include "ASTTVisitor.h"
#include "Translator.h"
#include "DLRepresentation.h"

class DescribeRelations : public ASTTVisitor {

	public:
		DescribeRelations(Environment &e) : env(e), translator(e) {}

		/*! an ASTEqual can represent three of our relations:
		 * multiplication, square, or DLR.  we check types and forms of
		 * the DLR to infer which one it is, and then call the appropriate
		 * translation function to get down to base flexible equality form */
		void applyASTEqual(ASTEqualPtr n);

		/*! find appropriate commitment descriptions for value in a range 
		 * and use them to create flexible equality DLRs */
		void applyASTRange(ASTRangePtr n);

		/*! this will split an expression of the form g^x * h^r_x into
		 * its representation as vectors of bases and exponents */
		static DLRepresentation splitExpr(ASTEqualPtr n, Environment &env);

		/*! breaks all the relations in the 'such that' block down 
		 * into flexible equality form */
		void apply(ASTNodePtr n);

	private:
		static void splitExprHelper(vector<ASTExprPtr>& bases, 
									vector<ASTExprPtr>& exponents, ASTMulPtr mult);
		Environment &env; 
		Translator translator;
};

#endif /*_DESCRIBERELATIONS_H_*/
