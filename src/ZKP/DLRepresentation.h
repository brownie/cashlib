
#ifndef _DLREPRESENTATION_H_
#define _DLREPRESENTATION_H_

#include "ASTNode.h"

class Environment;

class DLRepresentation {

	public:
		DLRepresentation() {}

		DLRepresentation(const DLRepresentation &o);

		int randIndex(Environment &env) const;
		ASTExprPtr commitExp(Environment &env) const
			{ return exps[(randIndex(env) + 1) % 2]; }
		ASTExprPtr randExp(Environment &env) const
			{ return exps[randIndex(env)]; }
		ASTExprPtr base(Environment &env) const
			{ return bases[(randIndex(env) + 1) % 2]; }
		ASTExprPtr randBase(Environment &env) const
			{ return bases[randIndex(env)]; }

		ZZ computeValue(Environment &env) const;
		string toString() const;
		vector<string> getBaseNames() const;

		ASTExprIdentifierPtr left;
		string group;
		vector<ASTExprPtr> bases;
		vector<ASTExprPtr> exps;

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int ver) {
			ar	& auto_nvp(left)
				& auto_nvp(group)
				& auto_nvp(bases)
				& auto_nvp(exps)
				;
		}
};

#endif /*_DLREPRESENTATION_H_*/
