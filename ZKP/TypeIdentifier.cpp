
#include "TypeIdentifier.h"
#include "Environment.h"

// -------------------------------
// PRIVACY

void TypeIdentifier::applyASTProof(ASTProofPtr n) {
	// everything in the given block is public...
	ASTListGivenPtr publics = n->getGiven();
	isPrivate = false;
	publics->visitChildren(*this);
	// ... and everything in the 'prove knowledge of' block is private!
	ASTListGivenPtr privates = n->getKnowledge();
	isPrivate = true;
	privates->visitChildren(*this);
}

// ---------------------------------
// GROUP INFORMATION

// reserve a name to mean "this var has no bound group"
const string Environment::NO_GROUP = "__NO_GROUP__";

void TypeIdentifier::applyASTDeclElements(ASTDeclElementsPtr n) {
	
	string grpname = n->getGroup()->getName();
	ASTListDeclPtr elmts = n->getElements();
			
	ASTDeclIdentifierSubPtr idSub;
	ASTDeclIDRangePtr idRange;
	string name;
	VarInfo info(grpname, VarInfo::ELEMENT);
	for (int i = 0; i < elmts->size(); i++) {
		ASTNodePtr ith = elmts->get(i);
		// if the declaration is just a name...
		if((idSub=dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0) {
			name = idSub->getName();
			// associate element with group
			env.varTypes[name] = info;
			// also associate element with privacy setting
			env.privates[name] = isPrivate;
		} else if((idRange=dynamic_pointer_cast<ASTDeclIDRange>(ith))!=0){
			for(int j = idRange->getLBound(); j <= idRange->getUBound(); j++){
				name = idRange->getName(j);
				env.varTypes[name] = info;
				env.privates[name] = isPrivate;
			}
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
				"Variable %d was not a valid declaration", i);
		}
	}
}

void TypeIdentifier::applyASTDeclExponents(ASTDeclExponentsPtr n) {
	
	string grpname = n->getGroup()->getName();
	ASTListDeclPtr exps = n->getExponents();
		
	ASTDeclIdentifierSubPtr idSub;
	ASTDeclIDRangePtr idRange;
	string name;
	VarInfo info(grpname, VarInfo::EXPONENT);
	for (int i = 0; i < exps->size(); i++) {
		ASTNodePtr ith = exps->get(i);
		if((idSub=dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0){
			// associate exponent with group
			name = idSub->getName();
			env.varTypes[name] = info;
			env.privates[name] = isPrivate;
		} else if((idRange=dynamic_pointer_cast<ASTDeclIDRange>(ith))!=0){
			for(int j = idRange->getLBound(); j <= idRange->getUBound(); j++){
				name = idRange->getName(j);
				env.varTypes[name] = info;
				env.privates[name] = isPrivate;
			}
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
				"Variable %d was not a valid declaration", i);
		}
	}
}

void TypeIdentifier::applyASTDeclGroup(ASTDeclGroupPtr n) {

	string grpname = n->getGroup()->getName();
	ASTListDeclPtr gens = n->getItems();
	ASTDeclIdentifierSubPtr mod = n->getModulus();

	// associate modulus (if specified)
	if (mod) { 
		env.varTypes[mod->getName()] = VarInfo(grpname, VarInfo::MODULUS);
	}

	// associate generators (if specified)			
	if (gens) {	
		ASTDeclIdentifierSubPtr idSub;
		ASTDeclIDRangePtr idRange;
		string name;
		for (int i = 0; i < gens->size(); i++) {
			ASTNodePtr ith = gens->get(i);
			VarInfo info(grpname, VarInfo::ELEMENT);		
			if((idSub=dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0){
				name = idSub->getName();
				// XXX: just doing this temporarily until I figure something
				// better out
				env.generators[name] = to_ZZ(0);
				// associate generators with group and privacy setting
				env.varTypes[name] = info;
				env.privates[name] = isPrivate;
			} else if((idRange=dynamic_pointer_cast<ASTDeclIDRange>(ith))!=0){
				for(int j = idRange->getLBound(); j <= idRange->getUBound(); j++){
					name = idRange->getName(j);
					// XXX: same problem here
					env.generators[name] = to_ZZ(0);
					env.varTypes[name] = info;
					env.privates[name] = isPrivate;
				}
			} else {
				throw CashException(CashException::CE_PARSE_ERROR,
					"Variable %d was not a valid declaration", i);
			}
		}
	}
}

void TypeIdentifier::applyASTRandomBnd(ASTRandomBndPtr n){
	ASTListDeclPtr l = n->getIds();
			
	ASTDeclIdentifierSubPtr idSub;
	ASTDeclIDRangePtr range;			
	string name;
	VarInfo info(Environment::NO_GROUP, VarInfo::INTEGER);
	for(int i = 0; i < l->size(); i++){
		ASTNodePtr ith = l->get(i);
		if((idSub=dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0){
			// assign no group and privacy setting
			name = idSub->getName();
			env.varTypes[name] = info;
			env.privates[name] = isPrivate;
		} else if ((range=dynamic_pointer_cast<ASTDeclIDRange>(ith)) != 0){
			for(int j = range->getLBound(); j <= range->getUBound(); j++){
				name = range->getName(j);
				env.varTypes[name] = info;
				env.privates[name] = isPrivate;
			}
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
					"Identifier given for a random integer was not valid");
		}
	}
}

void TypeIdentifier::applyASTRandomPrime(ASTRandomPrimePtr n){
	ASTListDeclPtr l = n->getIds();
	VarInfo info(Environment::NO_GROUP, VarInfo::INTEGER);
	ASTDeclIdentifierSubPtr idSub;
	ASTDeclIDRangePtr range;
	string name;	
	for(int i = 0; i < l->size(); i++){
		ASTNodePtr ith = l->get(i);
		if((idSub=dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0){
			name = idSub->getName();
			// assign random prime as integer with privacy setting
			env.varTypes[name] = info;
			env.privates[name] = isPrivate;
		} else if ((range=dynamic_pointer_cast<ASTDeclIDRange>(ith)) != 0){
			for(int j = range->getLBound(); j <= range->getUBound(); j++){
				name = range->getName(j);
				env.varTypes[name] = info;
				env.privates[name] = isPrivate;
			}
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
					"Identifier given for a random prime was not valid");
		}
	}
}

void TypeIdentifier::applyASTDeclIntegers(ASTDeclIntegersPtr n) {

	ASTListDeclPtr ints = n->getInts();
			
	ASTDeclIdentifierSubPtr idSub;
	ASTDeclIDRangePtr idRange;	
	string name;	
	VarInfo info(Environment::NO_GROUP, VarInfo::INTEGER);
	for (int i = 0; i < ints->size(); i++) {
		ASTNodePtr ith = ints->get(i);
		if((idSub=dynamic_pointer_cast<ASTDeclIdentifierSub>(ith)) != 0){
			string name = idSub->getName();
			// integer are associated with no group and some privacy setting
			env.varTypes[name] = info;
			env.privates[name] = isPrivate;
		} else if((idRange=dynamic_pointer_cast<ASTDeclIDRange>(ith)) != 0){
			for(int j = idRange->getLBound(); j <= idRange->getUBound(); j++){
				name = idRange->getName(j);
				env.varTypes[name] = info;
				env.privates[name] = isPrivate;
			}
		} else {
			throw CashException(CashException::CE_PARSE_ERROR,
				"Variable %d was not a valid declaration", i);
		}
	}
}

void TypeIdentifier::apply(ASTNodePtr n) {
	n->visit(*this);
}
