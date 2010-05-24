#include "GroupSquareMod.h"

GroupSquareMod::GroupSquareMod(const string &owner, ZZ modulus, int st)
  : Group(owner, modulus)
{
	type = Group::TYPE_SQUARE;
	stat = st;
}

bool GroupSquareMod::isElement(const ZZ &value) const {
	if(value < 0)
		return false;
	if(value >= modulus)
		return false;
	if(GCD(value, modulus) != 1)
		return false;
	return true;
}

bool GroupSquareMod::isGenerator(const ZZ &gen) const {
	// check if it's an element of our group
	if(!isElement(gen))
		return false;
	// check if gen == 1 (mod modulus), ie. the identity => not a generator
	if(gen == to_ZZ(1))
		return false;
	// we cannot check anything else unfortunately
	return true;
}

void GroupSquareMod::debug() const
{
	cout << "GroupSquareMod" << endl;
	cout << "Modulus = " << modulus << endl
		 << "stat = " << stat << endl;
	Group::debug();
}

