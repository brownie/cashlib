/**
 * This represents a group of prime order, which is a Group.
 *
 * In addition to a Group, a prime group has the following:
 * - The modulus is known
 */

#include "GroupPrime.h"
#include "CashException.h"

GroupPrime::GroupPrime(const string &owner, int modLength, int oLength, int st)
	: Group(owner, modLength, oLength)
{
	type = Group::TYPE_PRIME;

	/* check preconditions */
	if(modulusLength < 512)
		throw CashException(CashException::CE_SECURITY_ERROR,
				"GroupPrime: Tried to create a group with "
				"modulusLength < 512");
	if(orderLength < 160)
		throw CashException(CashException::CE_SECURITY_ERROR,
				"GroupPrime: Tried to create a group with "
				"orderLength < 160");
	if(orderLength < 2*st)
		throw CashException(CashException::CE_SECURITY_ERROR,
				"GroupPrime: Tried to create a group with "
				"orderLength < 2*stat");

	stat = st;

	// decide on an order for the group
	order = GenPrime_ZZ(orderLength, stat);
	int factorLength = modulusLength - orderLength;

	// find a prime modulus of suitable length
	ZZ tempFactor;
	do {
		tempFactor = RandomLen_ZZ(factorLength);
		modulus = order*tempFactor + 1;
	} while(!ProbPrime(modulus, stat) || NumBits(modulus) != modulusLength);
	factor = tempFactor;

	// come up with a generator for the group
	ZZ gammaPrime, generator;
	do {
		gammaPrime = RandomBnd(modulus);
		generator = PowerMod(gammaPrime, factor, modulus);
	} while(generator == 1);
	generators.push_back(generator);

	// check postconditions
	if(NumBits(modulus) != modulusLength)
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupPrime postcondition check: |modulus| != modulusLength");
	if(NumBits(order) != orderLength)
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupPrime postcondition check: |order| != orderLength");
}

ZZ GroupPrime::addNewGenerator() {
	ZZ gammaPrime, generator;
	do {
		gammaPrime = RandomBnd(modulus);
		generator = PowerMod(gammaPrime, factor, modulus);
	} while(generator == 1);
	generators.push_back(generator);
	return generator;
}

/* Check if a given value is an element of our group */
bool GroupPrime::isElement(const ZZ &value) const {
	ZZ modulus = getModulus();
	if(value < 0)
		return false;
	if(value >= modulus)
		return false;
	if(PowerMod(value, getOrder(), modulus) != 1)
		return false;
	return true;
}


bool GroupPrime::isGenerator(const ZZ &gen) const {
	// check if it's an element of our group
	if(!isElement(gen))
		return false;
	// check if gen == 1 (mod modulus), ie. the identity => not a generator
	if(gen == to_ZZ(1))
		return false;
	// everything looks OK
	return true;
}

bool GroupPrime::checkGroupSetup() const {
	// check if the group order is prime
	if(!ProbPrime(order, stat)) 
		return false;
	// check if the modulus is prime
	if(!ProbPrime(modulus, stat)) 
		return false;
	// check that (modulus - 1) is a multiple of order
	if((modulus - 1) % order != 0) 
		return false;

	// for each generator x, check if x has order == order of g
	// and that x is not 1 (mod modulus)
	vector<ZZ> gens = getGenerators();
	for(unsigned i = 0; i < gens.size(); i++) {
		if(!isGenerator(gens[i])) {
			// one of the generators is not valid
			return false;
		}
	}
	// group now adheres to guidelines in the SETUP
	return true;
}

void GroupPrime::debug() const {
	cout << "GroupPrime" << endl;
	cout << "Modulus = " << modulus << endl
		 << "Order = " << order << endl
		 << "stat = " << stat << endl;
	Group::debug();
}

bool GroupPrime::isProperExponent(const ZZ &exp) const {
	if(order == 0) {
		cerr << "GroupPrime::isExponent: Attempt to call with a group of "
			<< "unknown order" << endl;
		return false;
	}
	return (exp >= to_ZZ(1)) && (exp < order);
}

bool GroupPrime::checkPreconditions() const {
	return checkGroupSetup();
}

