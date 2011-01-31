
#include "ProgramMaker.h"
#include <assert.h>
#include <fstream>
#include "ZKP/ASTNode.h"
#include "CommonFunctions.h"

string ProgramMaker::makeCLObtain(const gen_group_map &grps, 
								  const vector<CommitmentInfo> &coms) {
	return makeCLObtain(nameGroups(grps), nameComs(coms).first, 
						nameComs(coms).second);
}

string ProgramMaker::makeCLObtain(const string &grpPart, const string &comPart,
								  const string &comRelPart) {
	string program = "computation: "
						"given: " + grpPart + " "
							"integers: x[1:l], stat, modSize "
						"compute: "
							"random integer in [0,2^(modSize+stat)): vprime "
							"C := h^vprime * for(i, 1:l, *, g_i^x_i) "
					"proof: "
						"given:" + grpPart + " "
							"element in pkGroup: C " + comPart + " "
							"integer: l_x "
						"prove knowledge of: "
							"integers: x[1:l], r[1:l] "
							"exponent in pkGroup: vprime "
						"such that: "
							"for(i, 1:l, range: (-(2^l_x-1)) <= x_i < 2^l_x) "
							"C = h^vprime * for(i, 1:l, *, g_i^x_i) ";
	program += comRelPart;
	// now write this to file and return the file name
	ofstream writer;
	string fname = CommonFunctions::getZKPDir()+"/cl-obtain-temp.txt";
	writer.open(fname.c_str(), ios::out);
	if (!writer)
		throw CashException(CashException::CE_PARSE_ERROR,
			"[ProgramMaker::makeCLObtain]: Could not write program to file");

	writer << program;
	writer.close();
	return fname;
}

string ProgramMaker::makeCLProve(const gen_group_map &grps, 
								 const vector<CommitmentInfo> &coms) {
	return makeCLProve(nameGroups(grps), nameComs(coms).first,
					   nameComs(coms).second);
}

string ProgramMaker::makeCLProve(const string &grpPart, const string &comPart,
								 const string &comRelPart) {
	string program = "computation: "
						"given:" + grpPart + " "
							"element in pkGroup: A "
							"exponents in pkGroup: e, v "
							"integers: x[1:l], modSize, stat "
						"compute: "
							"random integers in [0,2^(modSize+stat)): r, r_C "
							"vprime := v + r*e "
							"Aprime := A * h^r "
							"C := h^r_C * for(i, 1:l, *, g_i^x_i) "
							"D := for(i, l+1:l+k, *, g_i^x_i) "
					"proof: "
						"given:" + grpPart + " "
							"elements in pkGroup: C, D, Aprime " + comPart + " "
							"integers: l_x, x[l+1:l+k] "
						"prove knowledge of: "
							"integers: x[1:l], r[1:l] "
							"exponents in pkGroup: e, vprime, r_C "
						"such that: "
							"for(i, 1:l, range: (-(2^l_x-1)) <= x_i < 2^l_x) "
							"C = h^r_C * for(i, 1:l, *, g_i^x_i) "
							"f = C^(-1) * D^(-1) * (Aprime^e) * h^(r_C-vprime)";
	program += " " + comRelPart;
	// again, write to file and return filename
	ofstream writer;
	string fname = CommonFunctions::getZKPDir()+"/cl-prove-temp.txt";
	writer.open(fname.c_str(), ios::out);
	if (!writer)
		throw CashException(CashException::CE_PARSE_ERROR,
			"[ProgramMaker::makeCLProve]: Could not write program to file");
	writer << program;
	writer.close();
	return fname;
}

string ProgramMaker::makeGeneratorList(const vector<string> &genNames) {
	string gens = "<";
	for (unsigned i = 0; i < genNames.size() - 1; i++) {
		gens += genNames[i] + ",";
	}
	gens += genNames[genNames.size()-1] + ">";
	return gens;
}

string ProgramMaker::makeCommitmentForm(const string &comName,
										const vector<string> &baseNames,
										const vector<string> &expNames) {
	string form = comName + " = ";
	assert(baseNames.size() == expNames.size());
	for (unsigned i = 0; i < baseNames.size() - 1; i++) {
		form += baseNames[i] + "^" + expNames[i] + "*";
	}
	form += baseNames[baseNames.size()-1] + "^" + expNames[expNames.size()-1] 
			+ " ";
	return form;
}

string ProgramMaker::nameGroups(const gen_group_map &grps) {
	string groups = "";
	for (gen_group_map::const_iterator it = grps.begin();
									   it != grps.end(); ++it) {
		string grpName = it->first;
		string thisGroup = "group: ";
		string genNames = makeGeneratorList(it->second.second);
		thisGroup += grpName + " = " + genNames + " ";
		groups += thisGroup;
	}
	return groups;
}

pair<string, string> ProgramMaker::nameComs(const vector<CommitmentInfo> &c) {
	string comElmts = "";
	string comRels = "";
	for (unsigned i = 0; i < c.size(); i++) {
		string index = lexical_cast<string>(i+1);
		string comName = "c_"+index;
		string thisComElmt = "element in " + c[i].group + ": " + 
							 comName + " ";
		vector<string> expNames;
		expNames.push_back("x_"+index);
		expNames.push_back("r_"+index);
		string form = makeCommitmentForm(comName, c[i].baseNames, expNames);
		thisComElmt += "commitment to x_"+index + ": " + form + " ";
		comElmts += thisComElmt;
		comRels += form;
	}
	return make_pair(comElmts, comRels);
}
