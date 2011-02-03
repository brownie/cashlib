
#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "../Group.h"

#ifndef USE_STD_MAP
#define USE_STD_MAP 1
#endif

#ifdef USE_STD_MAP
// XXX for some reason, BuyMessages are not working when unordered_map
// is used in ProofMessages: may have to do with map serialization
 #include <map>
 #define MAP_TYPE std::map
#else
 #include <boost/functional/hash.hpp>
 #include <boost/unordered_map.hpp>
 #define MAP_TYPE boost::unordered_map
#endif

#include "DLRepresentation.h"
#include "SigmaProof.h"
#include "ASTNode.h"
#include "PowerCache.h"
#include "MultiExpCache.h"

/*! \brief This class is just a container */

struct DecompNames { 
	string decomp;
	string decompSquare;

	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int ver) {
		ar	& auto_nvp(decomp)
			& auto_nvp(decompSquare)
			;
	}
};

// hash function for use with boost::hash and unordered_map, etc
std::size_t hash_value(const ZZ& n);

typedef MAP_TYPE<string, ZZ> variable_map;
typedef MAP_TYPE<string, Ptr<const Group> > group_map;
typedef MAP_TYPE<string, string> commitment_map;
typedef MAP_TYPE<string, DLRepresentation> dlr_map;
typedef MAP_TYPE<string, vector<DecompNames> > decomp_map;
typedef MAP_TYPE<ZZ, vector<ZZ> > decomp_val_map;
//typedef boost::unordered_map<ZZ, vector<ZZ>, boost::hash<ZZ> > decomp_val_map;
typedef MAP_TYPE<string, VarInfo> variable_type_map;
typedef MAP_TYPE<string, bool> privacy_map;
typedef MAP_TYPE<string, ASTExprPtr> expr_map;
typedef MAP_TYPE<string,ZZ> input_map;
typedef MAP_TYPE<string, int> base_map;
typedef MAP_TYPE<vector<string>, int> multi_base_map;

/*! struct stores a proof, public variables needed for verifying the 
 * proof, and some possible additional variable information */
struct ProofMessage {
	
	ProofMessage() {}

	ProofMessage(const variable_map &v, const variable_map &p,
				 const SigmaProof &pr)
		: vars(v), publics(p), proof(pr) {}

	// since vars will sometimes be empty, have constructor without it
	ProofMessage(const variable_map &p, const SigmaProof &pr)
		: publics(p), proof(pr) {}

	variable_map vars;
	variable_map publics;
	SigmaProof proof;

	// need to be able to serialize this
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int ver) {
		ar	& auto_nvp(vars)
			& auto_nvp(publics)
			& auto_nvp(proof)
			;	
	}
};

class Environment {

	public:
		Environment() {}

		static const string NO_GROUP;
		
		/*! maps variable names to their actual values */
		variable_map variables;
		// XXX: not sure what the best data structure is here...
		variable_map generators;
		/*! maps group names to the group objects */
		group_map groups;
		/*! maps variable names to their associated group and type */
		variable_type_map varTypes;
		/*! keeps track of types even for intermediate expressions; needed
		 * for caching types to use once we call eval at runtime */
		variable_type_map exprTypes;
		/*! maps variable names to their privacy setting */
		privacy_map privates;
		/*! maps a variable name to its corresponding commitment */
		commitment_map commitments;
		/*! maps variable to its DLR representation */
		dlr_map discreteLogs;
		/*! maps description name to description object */
		dlr_map descriptions;
		/*! maps values to the names of their four squares decomposition */
		decomp_map decompositions;
		decomp_val_map decompCache;
		/*! list of variables that will need to be created (randomly)
		 * at runtime */
		vector<string> randoms;
		/*! used for remembering form of intermediate expressions */
		expr_map expressions;
		/*! used for remembering intermediate commitments */
		dlr_map comsToCompute;
		/*! used for keeping track of range commitments that need to be
		 * computed by verifier at runtime */
		dlr_map rangeComs;
		/*! information for caching powers of known bases */	
		Ptr<PowerCache> cache;
		Ptr<MultiExpCache> multiCache;

		/*! clears out all the maps */
		void clear();

		void clearPrivates();

		/*! gets group object for a given variable */
		Ptr<const Group> getGroup(const string &varName) const;

		/*! gets commitment description for a given committed variable 
		 * whose value has already been computed */
		DLRepresentation getCommitment(const string &varName) const; 

		/*! gets commitment value for a given variable */
		ZZ getCommitmentValue(const string &varName) const;

		/*! adds variable to randoms vector; at runtime we will compute
		 * its value using the appropriate group->randomExponent() call */
		void addRandomVariable(const string &name, VarInfo info);

		/*! adds variable/commitment pair to commitments map, as well as
		 * adding commitment to discreteLogs and to comsToCompute */
		void addCommittedVariable(const string &name, const string &comName,
								  const DLRepresentation &dlr, VarInfo info);

		/*! adds an intermediate expression, with its group type and 
		 * desired privacy setting */
		void addExpression(const string &name, ASTExprPtr e, VarInfo i, bool p);

		/*! does exponentiation, possibly using cache values */
		ZZ modPow(const string &bName, const ZZ &b, const ZZ &e, 
				  const ZZ &mod) const;

		/*! does multiexponentiation */
		ZZ multiExp(const vector<string> &bNames, const vector<ZZ> &bs, 
					const vector<ZZ> &es, const ZZ &mod) const;

		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int ver) {
			ar	& auto_nvp(NO_GROUP)
				& auto_nvp(variables)
				& auto_nvp(generators)
				& auto_nvp(groups)
				& auto_nvp(varTypes)
				& auto_nvp(exprTypes)
				& auto_nvp(privates)
				& auto_nvp(commitments)
				& auto_nvp(discreteLogs)
				& auto_nvp(descriptions)
				& auto_nvp(decompositions)
				& auto_nvp(randoms)
				& auto_nvp(expressions)
				& auto_nvp(comsToCompute)
				& auto_nvp(rangeComs)
				;
		}
};


#endif /*_ENVIRONMENT_H_*/
