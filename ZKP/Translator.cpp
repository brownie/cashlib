
#include "Translator.h"
#include <assert.h>
#include "../Debug.h"

void Translator::describeDLR(DLRepresentation &c) {
	// consider anything complicated to be reduced and add it to 
	// intermediate expressions map
	assert(c.bases.size() == c.exps.size());
	for (unsigned i = 0; i < c.bases.size(); i++) {
		// for each base or exponent, check to see if it's an id
		// if not, need to eval it later on so add it to expressions
		// XXX: what if base is just integers or exponents?
		VarInfo baseType = VarInfo(c.group, VarInfo::ELEMENT);
		VarInfo expType = VarInfo(c.group, VarInfo::EXPONENT);
		if (dynamic_pointer_cast<ASTExprIdentifier>(c.bases[i]) == 0) {
			env.addExpression(c.bases[i]->toString(), c.bases[i], baseType, 0);
		}
		if (dynamic_pointer_cast<ASTExprIdentifier>(c.exps[i]) == 0) {
			env.addExpression(c.exps[i]->toString(), c.exps[i], expType, 1);
		}
	}
	output[c.toString()] = c;
}

void Translator::describeMultiplication(DLRepresentation &product,
					DLRepresentation &factor1, DLRepresentation &factor2) {
	// let's say this is x = a * b
	// need new DLR for c_x = c_a^b * h^(r_x-b*r_a)
	DLRepresentation newDLR;
	
	// first make new exponent: r_x - b*r_a
	ASTMulPtr first = new_ptr<ASTMul>(factor2.commitExp(env), 
									  factor1.randExp(env));
	ASTSubPtr newExp = new_ptr<ASTSub>(product.randExp(env), first);

	// add this exponent to expressions that need to be evaluated
	string exprString = newExp->toString();
	// use product group, but all groups will be the same so it doesn't matter
	VarInfo info = VarInfo(product.group, VarInfo::EXPONENT);
	env.addExpression(exprString, newExp, info, 1);

	vector<ASTExprPtr> newBases;
	vector<ASTExprPtr> newExps;
	
	// c_a^b
	newBases.push_back(factor1.left);
	newExps.push_back(factor2.commitExp(env));
	
	// h^(r_x-b*r_a)
	newBases.push_back(factor1.randBase(env));
	newExps.push_back(newExp);
	
	newDLR.left = product.left;
	newDLR.group = product.group;
	newDLR.bases = newBases;
	newDLR.exps = newExps;

	// now can describe both the new DLR and the given DLR for c_b
	describeDLR(newDLR);
	describeDLR(factor2);
}

void Translator::describeSquare(DLRepresentation &product,
								DLRepresentation &factor) {
	describeMultiplication(product, factor, factor);
}

void Translator::describeNonNegative(DLRepresentation &c) {

	// need to decompose x into x = x_1^2 + x_2^2 + x_3^2 + x_4^2
	// for each x_i, need to do a square proof
	vector<DecompNames> decomps;
	// set up common stuff
	vector<ASTExprPtr> bs;
	bs.push_back(c.base(env));
	bs.push_back(c.randBase(env));		
	VarInfo groupPair = VarInfo(c.group, VarInfo::EXPONENT);
	vector<ASTExprIdentifierPtr> randSquareExps;
	for (int i = 0; i < 4; i++) {
		string ctr = lexical_cast<string>(counter);
		// need names for x_i and x_2i; also for random values and commitments
		DecompNames names;
		names.decomp = "__decomp__" + ctr;
		names.decompSquare = "__square__" + ctr;
		decomps.push_back(names);
		string rand_decomp = "__randdecomp__" + ctr;
		string rand_square = "__randsquare__" + ctr;
		string baseCom = "__decomp_com__" + ctr;
		string squareCom = "__square_com__" + ctr;
		counter++;
		// now need c_i and c_2i
		DLRepresentation baseDLR;
		DLRepresentation squareDLR;
		baseDLR.left = nameNode(baseCom);
		squareDLR.left = nameNode(squareCom);
		// group should be same as original, same with bases
		baseDLR.group = c.group;
		squareDLR.group = c.group;
		baseDLR.bases = bs;
		squareDLR.bases = bs;
		// now make exponents
		baseDLR.exps.push_back(nameNode(names.decomp));
		baseDLR.exps.push_back(nameNode(rand_decomp));
		squareDLR.exps.push_back(nameNode(names.decompSquare));
		ASTExprIdentifierPtr randSquareId = nameNode(rand_square);
		// if it's the last commitment, randomness needs to be 
		// r_x - (r_1 + r_2 + r_3)
		if (i != 3) {
			// keep random exponent used in square commitment for later
			randSquareExps.push_back(randSquareId);
			squareDLR.exps.push_back(randSquareId);
		} else {
			ASTAddPtr a = new_ptr<ASTAdd>(randSquareExps[0],randSquareExps[1]);
			ASTAddPtr a2 = new_ptr<ASTAdd>(a, randSquareExps[2]);
			ASTSubPtr newExp = new_ptr<ASTSub>(c.randExp(env), a2);
			squareDLR.exps.push_back(newExp);
		}
		// also need to associate values with proper group, and with commitment
		env.addCommittedVariable(names.decomp, baseCom, baseDLR, groupPair);
		env.addCommittedVariable(names.decompSquare, squareCom, squareDLR,
								 groupPair);
		env.addRandomVariable(rand_decomp, groupPair);
		if (i != 3)
			env.addRandomVariable(rand_square, groupPair);

		// now need to do square proof for c_2i and c_i 
		describeSquare(squareDLR, baseDLR);
	}
	// now need to associate variable with its decomposition
	env.decompositions[c.commitExp(env)->toString()] = decomps;

	// need to increment counter before we do others
	counter++;
}

void Translator::describeRange(ASTExprPtr x, ASTIdentifierLitPtr group, 
							   ASTExprPtr low, ASTExprPtr high) {
	string grpName = group->getName();
	string xName = x->toString();
	VarInfo groupPair = VarInfo(grpName, VarInfo::EXPONENT);
	string ctr = lexical_cast<string>(counter);
	// first make new exponent: (x - low)
	ASTSubPtr xLoSubExp = new_ptr<ASTSub>(x, low);
	string xLoSubExpName = xLoSubExp->toString();
	// next make new exponent: (high - x)
	ASTSubPtr hiXSubExp = new_ptr<ASTSub>(high, x);
	string hiXSubExpName = hiXSubExp->toString();
	// make new exponent for value (x -low)*(high - x)
	ASTMulPtr prodExp = new_ptr<ASTMul>(xLoSubExp, hiXSubExp);
	string prodExpName = prodExp->toString();
	// need to evaluate these later on 
	env.expressions[xLoSubExpName] = xLoSubExp;
	env.expressions[hiXSubExpName] = hiXSubExp;
	env.expressions[prodExpName] = prodExp;

	// need commitment to x in range group; if there is already one in
	// environment then use that, otherwise make a new one
	ASTExprPtr g, h;
	vector<ASTExprPtr> bs;
	DLRepresentation c;
	int randIndex = 0;
	// check that group is correct and commitment is there
	if (env.commitments.count(xName) > 0 && 
		env.getCommitment(xName).group == grpName) {
		c = env.getCommitment(xName);
		// also need to set g and h
		g = c.base(env);
		h = c.randBase(env);
		bs.push_back(g);
		bs.push_back(h);
		randIndex = c.randIndex(env);
	} else {	
		c.group = grpName;
		string cName = "__x_commitment__" + ctr;
		c.left = nameNode(cName);	
		// XXX: this could probably be optimized
		int i = 0;
		for (variable_map::iterator it = env.generators.begin();
									it != env.generators.end(); ++it) {
				// if we found a generator of the group, use it
				if (env.varTypes.at(it->first).group == grpName){
					if (i == 0) {
						g = nameNode(it->first);
						i++;
					} else if (i == 1) {
						h = nameNode(it->first);
						i++;
					} else break;
				}
		}
		// want to make sure that we actually assigned both
		assert(i == 2);
		bs.push_back(g);
		bs.push_back(h);
		c.bases = bs;
		c.exps.push_back(x);
		string rand = "__randx_com__" + ctr;
		c.exps.push_back(nameNode(rand));
		env.addRandomVariable(rand, groupPair);
		env.discreteLogs[cName] = c;
		env.varTypes[cName] = VarInfo(grpName, VarInfo::ELEMENT);
		env.comsToCompute[cName] = c;
		env.privates[cName] = 0;
		// XXX: have to hard-code this here because x isn't getting added
		// to the commitments map... not ideal!
		randIndex = 1;
	}

	// make new commitment to (x - low)
	string xLoCom = "__xLo_commitment__" + ctr;
	DLRepresentation xLoComDLR;
	xLoComDLR.left = nameNode(xLoCom);
	xLoComDLR.group = grpName;
	xLoComDLR.bases = bs;
	xLoComDLR.exps.push_back(xLoSubExp);
	xLoComDLR.exps.push_back(c.exps[randIndex]);
	
	// XXX: is there any reason why the prover shouldn't use this as well?
	// we also want DLR representation for commitment to x - low
	// that the verifier can compute on its own
	DLRepresentation verifierLoDLR;
	verifierLoDLR.group = grpName;
	verifierLoDLR.left = nameNode(xLoCom);
	// this will be c_{x - low} = c_x * g^-low
	verifierLoDLR.bases.push_back(g);
	verifierLoDLR.bases.push_back(c.left);
	verifierLoDLR.exps.push_back(new_ptr<ASTNegative>(low));
	verifierLoDLR.exps.push_back(new_ptr<ASTExprInt>("1"));
	// now add it to map to be computed later
	env.rangeComs[xLoCom] = verifierLoDLR;

	// make -r_x an exponent [for commitment to (high - x)]
	ASTNegativePtr negRX = new_ptr<ASTNegative>(c.exps[randIndex]);
	env.addExpression(negRX->toString(), negRX, groupPair, 1);

	// make new commitment to (high - x)
	string hiXCom = "__hiX_commitment__" + ctr;
	DLRepresentation hiXComDLR;
	hiXComDLR.left = nameNode(hiXCom);
	hiXComDLR.group = grpName;
	hiXComDLR.bases = bs;
	hiXComDLR.exps.push_back(hiXSubExp);
	hiXComDLR.exps.push_back(negRX);

	// we also want DLR representation for commitment to high - x
	// that the verifier can compute on its own
	DLRepresentation verifierHiDLR;
	verifierHiDLR.group = grpName;
	verifierHiDLR.left = nameNode(hiXCom);
	// this will be c_{high - x} = g^hi * c_x^{-1}
	verifierHiDLR.bases.push_back(g);
	verifierHiDLR.bases.push_back(c.left);
	verifierHiDLR.exps.push_back(high);
	verifierHiDLR.exps.push_back(new_ptr<ASTNegative>(new_ptr<ASTExprInt>("1")));
	// now add it to map to be computed later
	env.rangeComs[hiXCom] = verifierHiDLR;

	// compute new commitment to product (x - low)*(high - x)
	string r1 = "__randExp__" + ctr;
	string prodCom = "__prod_commitment__" + ctr;
	DLRepresentation prodComDLR;
	prodComDLR.left = nameNode(prodCom);
	prodComDLR.group = grpName;
	prodComDLR.bases = bs;
	prodComDLR.exps.push_back(prodExp);
	prodComDLR.exps.push_back(nameNode(r1));	
	
	// also need to associate values with proper group, and with commitment
	// and privacy settings
	// and add random exponent for later
	env.addRandomVariable(r1, groupPair);
	env.addCommittedVariable(xLoSubExpName, xLoCom, xLoComDLR, groupPair);
	env.addCommittedVariable(hiXSubExpName, hiXCom, hiXComDLR, groupPair);
	env.addCommittedVariable(prodExpName, prodCom, prodComDLR, groupPair);

	// increment counter before we do other descriptions, I suppose
	counter++;

	// now prove product is formed correctly and non-negative
	describeMultiplication(prodComDLR, xLoComDLR, hiXComDLR);
	describeNonNegative(prodComDLR);

	// the verifier will be computing the values for the commitments 
	// to x - lo and hi - x itself, so make them private
	env.privates[xLoCom] = 1;
	env.privates[hiXCom] = 1;
}

ASTExprIdentifierPtr Translator::nameNode(const string &name) {
	return new_ptr<ASTExprIdentifier>(name);
}
