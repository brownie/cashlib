#include "ZKPParser.hpp"
#include "ZKPLexer.hpp"
#include "ASTNode.h"
#include "Printer.h"
#include "UnusedVariables.h"
#include "UndefinedVariables.h"
#include "ComputationVisitor.h"
#include "BindGroupValues.h"
#include "Interpreter.h"
#include "InterpreterProver.h"
#include "InterpreterVerifier.h"
#include "ForExpander.h"
#include "ConstantSub.h"
#include "ConstantProp.h"


#include <typeinfo>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <antlr/ANTLRException.hpp>
#include <boost/program_options.hpp>

#include "GroupPrime.h"
#include "GroupRSA.h"

using namespace std;

typedef boost::unordered_map<string, bool> def_map;

void setupEnvironment(Environment &e, const string& fname) {
	int stat = 80;	

	// hack for known examples: set up environment
	if (fname.find("ecash.txt") != string::npos) {

		GroupPrime* cashG = new GroupPrime("bank", 1024, stat*2, stat); // init gen is f
		cashG->addNewGenerator(); // this is g
		cashG->addNewGenerator(); // this is h
		cashG->addNewGenerator(); // this is h1
		cashG->addNewGenerator(); // this is h2	
		
		GroupRSA* rangeG = new GroupRSA("first", 1024, stat); // first gen is g1
		rangeG->addNewGenerator(); // second gen is g2
		e.groups["rangeGroup"] = rangeG;
		e.groups["cashGroup"] = cashG;
		vector<ZZ> secrets;
		for (int i = 0; i < 3; i++) {
			secrets.push_back(cashG->randomExponent());
		}
		e.variables["s"] = secrets[0];
		e.variables["t"] = secrets[1];
		e.variables["sk_u"] = secrets[2];
		e.variables["J"] = to_ZZ(51);

	} else if (fname.find("multiplication.txt") != string::npos) {

		GroupPrime* cashG = new GroupPrime("bank", 1024, 2*stat, stat); // this is g
		cashG->addNewGenerator(); // this is h
		e.groups["G"] = cashG;	
		// let's go ahead and give in a and b
		ZZ a = to_ZZ(3);
		ZZ b = to_ZZ(2);
		variable_map v;
		//e.variables["a"] = a;
		//e.variables["b"] = b;
	} else if (fname.find("twoDL.txt") != string::npos 
			|| fname.find("dlr.txt") != string::npos
			|| fname.find("badDLR.txt") != string::npos
			|| fname.find("simpleDLR.txt") != string::npos
			|| fname.find("declRange.txt") != string::npos
			|| fname.find("compute.txt") != string::npos 
			|| fname.find("modulus.txt") != string::npos) {
		GroupPrime* cashG = new GroupPrime("bank", 1024, stat*2, stat);
		cashG->addNewGenerator(); // this is h
		e.groups["G"] = cashG;	
		if (fname.find("badDLR.txt") != string::npos)
			e.variables["J"] = to_ZZ(51);
	} else if (fname.find("range.txt") != string::npos
			   || fname.find("complicatedRange.txt") != string::npos) {
		GroupRSA* cashG = new GroupRSA("bank", 1024, stat); // this is g
		//GroupPrime* cashG = new GroupPrime("bank", 1024, stat*2, stat);
		cashG->addNewGenerator(); // this is h
		e.groups["G"] = cashG;	
		e.variables["J"] = to_ZZ(51);
		e.variables["W"] = to_ZZ(100);
	}
}

int main(int argc, char** argv) {

	namespace po = boost::program_options;
	po::options_description desc("supported options");
	desc.add_options()
		("help,h", "display this help message")
		("printer,p", "pretty-print the program")
		("tree,t", "print the AST")
		("undefined,u", "find undefined variables")
		("unused,U", "find unused variables")
		("expand,e", "expand for-loops")
		("compute,c", "compute := lines")
		("interpret,i", "run the Interpreter on this program")
		("input-file,f", po::value<string>(), "ZKP file to test")
		;
	po::positional_options_description p;
	p.add("input-file", -1);
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
			  options(desc).positional(p).run(), vm);
	po::notify(vm);    

	if (vm.count("input-file") == 0) {
		cout << desc;
		return 1;
	}
		
	ifstream ifs(vm["input-file"].as<string>().c_str());

	try { 
		ZKPLexer* lexer = new ZKPLexer(ifs);
		ZKPParser* parser = new ZKPParser(*lexer);
		ASTSpecPtr n = parser->spec();
		
		if (n) {
			if (vm.count("undefined")) {
				// test out some more visitors
				UndefinedVariables undef;
				undef.apply(n);
			}
			if (vm.count("unused")) {
				UnusedVariables unused;
				unused.apply(n);
			}
			if(vm.count("expand")){
				Environment e;
				ForExpander exp(e);
				exp.applyASTNode(n);
				Printer print;
				cout<<print.unparse(n) <<endl;
			}
			if (vm.count("printer")) {
				Printer print;
				cout << print.unparse(n) << endl;
			}
			if (vm.count("tree")) {
				cout << "Tree:" << endl;
				TreePrinter tp;
				tp.applyASTNode(n);
			}
			if (vm.count("compute")) {
				Environment e;
				setupEnvironment(e, vm["input-file"].as<string>());

				//GroupIdentifier groupGuy(e);
				//groupGuy.apply(n);	
				for (variable_type_map::iterator it = e.varTypes.begin();
					 it != e.varTypes.end(); ++it) {
					cout << it->first << " " << it->second.group << endl;
				}

				BindGroupValues binder(e);
				binder.apply(n);
				ComputationVisitor cv(e);
				cv.apply(n);
				for (variable_type_map::iterator it = e.varTypes.begin();
						it != e.varTypes.end(); ++it) {
					cout << it->first << " " << it->second.group << endl;
				}
				for (variable_map::iterator it = e.variables.begin();
						it != e.variables.end(); ++it) {
					cout << it->first << " " << it->second << endl;
				}
			}
			if (vm.count("interpret")) {
				string fname = vm["input-file"].as<string>();
				hashalg_t hashAlg = Hash::SHA1;
				int stat = 80;
				
				Environment e;
				setupEnvironment(e, fname);
				group_map g = e.groups;
				variable_map v = e.variables;

				InterpreterProver prover;
				prover.check(fname);
				prover.compute(v, g);				
				SigmaProof proof = prover.computeProof(hashAlg);
				// need to get the right inputs for verifier
				variable_map publics = prover.getPublicVariables();

				InterpreterVerifier verifier;
				verifier.check(fname);
				verifier.compute(v, publics, g);
				verifier.verify(proof, stat);
			}
		}
	} catch(antlr::ANTLRException& e) {
		cout << type_to_str(typeid(e)) << ": " << e.toString() << endl;
	}

	return 0;
}
