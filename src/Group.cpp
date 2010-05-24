#include "Group.h"
#include "CommonFunctions.h"
#include "CashException.h"

Group::Group(const string &owner, int modLength, int oLength)
	: modulusLength(modLength), orderLength(oLength)
{
	isTrusted = CommonFunctions::isTTP(owner);
}

Group::Group(const Group &o)
	: modulusLength(o.modulusLength), orderLength(o.orderLength),
	  generators(o.generators), isTrusted(o.isTrusted), modulus(o.modulus),
      type(o.type)
{
}

Group::Group(const string &owner, const ZZ &mod)
	: modulusLength(NumBits(mod)), orderLength(NumBits(mod)), modulus(mod)
{
	isTrusted = CommonFunctions::isTTP(owner);
}

ZZ Group::randomElement() {
	return PowerMod(getGenerator(), randomExponent(), getModulus());
}

void Group::debug() const {
	cout << "ModulusLength = " << modulusLength << endl;
	cout << "OrderLength = " << orderLength << endl;
}

void Group::makeTrusted() {
	if(isTrusted == false) {
		if(checkPreconditions()) {
			isTrusted = true;
		} else {
			throw CashException(CashException::CE_UNKNOWN_ERROR,
					"[Group::makeTrusted] Precondition checks failed");
		}
	}
}
