
#ifndef _PROGRAMMAKER_H_
#define _PROGRAMMAKER_H_

#include "ZKP/DLRepresentation.h"
#include "Group.h"

// the following are useful for CLBlindRecipient and CLSignatureProver
// for groups, need access to group object and names of generators
typedef pair<Ptr<const Group>, vector<string> > group_pair;
typedef boost::unordered_map<string, group_pair> gen_group_map;

// for commitments, we'll assume that value being committed to is called
// x and randomness used is called r; also assume commitment name is
// just c
struct CommitmentInfo {
	CommitmentInfo(const ZZ &val, const vector<string> &bNames,
				   const string &grp)
		: comValue(val), baseNames(bNames), group(grp) {}

	ZZ comValue;
	vector<string> baseNames;
	string group;
};

// this is useful for pairing a value with the randomness used to form
// its corresponding commitment
typedef pair<ZZ,ZZ> SecretValue;

class ProgramMaker {

	public:
		ProgramMaker() {}

		/*! given the various groups and commitments, creates a CL
		 * program for obtaining a signature */
		static string makeCLObtain(const string &grpPart,
								   const string &comPart,
								   const string &comRelPart);

		static string makeCLObtain(const gen_group_map &grps, 
								   const vector<CommitmentInfo> &coms);

		static string makeCLProve(const string &grpPart, const string &comPart,
								  const string &comRelPart);

		static string makeCLProve(const gen_group_map &grps,
								  const vector<CommitmentInfo> &coms);

		static string makeGeneratorList(const vector<string> &genNames);

		static string makeCommitmentForm(const string &cName, 
										 const vector<string> &bNames,
										 const vector<string> &eNames);
		
		static string nameGroups(const gen_group_map &grps);

		static pair<string,string> nameComs(const vector<CommitmentInfo> &c);

};

#endif /*_PROGRAMMAKER_H_*/
