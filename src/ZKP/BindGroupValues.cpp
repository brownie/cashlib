
#include "BindGroupValues.h"
#include "../Group.h"
#include "Environment.h"

void BindGroupValues::applyASTDeclGroup(ASTDeclGroupPtr n) {

	string grpname = n->getGroup()->getName();	
	Ptr<const Group> grp = env.groups.at(grpname);
	ASTListDeclPtr gens = n->getItems();
	
	// first bind modulus if it has been specified
	ASTDeclIdentifierSubPtr modName = n->getModulus();
	if (modName != 0) {
		ZZ mod = grp->getModulus();
		env.variables[modName->getName()] = mod;
	}

	// only need to do generators if they were specified
	int offset = 0;
	if (gens) {			
		ASTDeclIdentifierSubPtr id;
		ASTDeclIDRangePtr idRange;
		for (int i = 0; i < gens->size(); i++) {		
			ASTNodePtr ith = gens->get(i);
			if ((id=dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0) {
				// associate generator with its value
				string name = id->getName();
				env.variables[name] = grp->getGenerator(i+offset);
				// XXX: for now just using a dummy value since I don't
				// care, I just care if it's in the map or not
				env.generators[name] = 1;
			} else if((idRange=dynamic_pointer_cast<ASTDeclIDRange>(ith))!=0){
				for(int x = idRange->getLBound(); x<=idRange->getUBound(); x++){
					// associate generator with its value
					string name = idRange->getName(x);
					env.variables[name] = grp->getGenerator(i+offset);
					env.generators[name] = 1;
					offset++;
				}
				// in the last iteration, we don't want to increase offset
				// because i will be increased, so we need to compensate
				offset--;
			} else{
				throw CashException(CashException::CE_PARSE_ERROR,
					"Variable %d was not a valid declaration", i);
			}
		}
	}
}

void BindGroupValues::apply(ASTNodePtr n) {
	n->visit(*this);
}


		
