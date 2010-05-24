
#include "GroupRSA.h"
#include "MultiExp.h"
#include "CashException.h"

static int generatorCheckMode = 0;
void setGeneratorCheckMode(int mode) { generatorCheckMode = mode; }

GroupRSA::GroupRSA(const string &owner, int modulusLength, int st)
  : Group(owner, modulusLength, modulusLength)
{
	type = Group::TYPE_RSA;

	// TODO: orderLength = modulusLength most, but not all of the time,
	// ie. iff NumBits(p * q) = NumBits((p-1) * (q-1))
	/* variables and useful constants */
	const ZZ one = to_ZZ(1), two = to_ZZ(2);
	ZZ pPrime, qPrime;
	ZZ arbitrary, qrp, qrq, generator;
	stat = st;

	/* check preconditions */
	if(modulusLength < 1024)
		throw CashException(CashException::CE_SECURITY_ERROR,
				"GroupRSA requires a minimum modulus length of 1024 bits");

	// number of generators isn't checked here -- we don't create them
	// until later because C++ hates calling virtual functions in
	// constructors

	do{ 
		/* generate p and p' */
		pPrime = GenGermainPrime_ZZ(modulusLength/2-1, stat);
		p = 2*pPrime + 1;

		/* generate q and q' */
		qPrime = GenGermainPrime_ZZ(modulusLength/2-1, stat);
		q = 2*qPrime + 1;

		/* compute n = p * q */
		modulus = p * q;
	} while(NumBits(modulus) < modulusLength);

	/* select an initial generator */
	if (generatorCheckMode == 0) {
		do {
			arbitrary = RandomBnd(modulus);
			generator = PowerMod(arbitrary, 2, modulus);
		} while(GCD(generator, modulus) != one || !isGenerator(generator));
	} else {
		ZZ hp, hq;

		/* Come up with hp s.t. 0 < hp < p, ie. 0 <= hp - 1 < p - 1 */
		hp = one + RandomBnd(p - one);

		/* Come up with hq s.t. 0 < hq < q */
		hq = one + RandomBnd(q - one);

		/* Compute the generator, h = hp^(q-1) * hq^(p-1) (mod n) */
		vector<ZZ> generatorBases;
		generatorBases.push_back(hp);
		generatorBases.push_back(hq);

		vector<ZZ> generatorExponents;
		generatorExponents.push_back(q-1);
		generatorExponents.push_back(p-1);
		generator = MultiExp(generatorBases, generatorExponents, modulus);
	}

	generators.push_back(generator);

	/* check postconditions */
	if(NumBits(modulus) != modulusLength)
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupRSA postcondition check: |n| != modulusLength");
	if(modulus != p * q)
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupRSA postcondition check: n != p * q");
	if(!ProbPrime(p))
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupRSA postcondition check: p is not prime");
	if(!ProbPrime(q))
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupRSA postcondition check: q is not prime");
	if(!ProbPrime((p-one)/two))
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupRSA postcondition check: p' is not prime");
	if(!ProbPrime((q-one)/two))
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupRSA postcondition check: q' is not prime");
	if(p % to_ZZ(4) != to_ZZ(3))
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupRSA postcondition check: p != 3 (mod 4)");
	if(q % to_ZZ(4) != to_ZZ(3))
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupRSA postcondition check: q != 3 (mod 4)");
	if(NumBits(p) != modulusLength/2)
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupRSA postcondition check: |p| != modulusLength/2");
	if(NumBits(q) != modulusLength/2)
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"GroupRSA postcondition check: |q| != modulusLength/2");
}

ZZ GroupRSA::getOrder() const {
	ZZ zero = to_ZZ(0), one = to_ZZ(1);
	if(p == 0 || q == 0) {
		return zero;
	}
	else {
		return (p - one) * (q - one);
	}
}

ZZ GroupRSA::addNewGenerator() {
	ZZ arbitrary, generator;
	const ZZ one = to_ZZ(1);

	if (generatorCheckMode == 0) {
		do {
			arbitrary = RandomBnd(modulus);
			generator = PowerMod(arbitrary, 2, modulus);
		} while(GCD(generator, modulus) != one || !isGenerator(generator));
	} else {
		ZZ hp, hq;

		/* Come up with hp s.t. 0 < hp < p, ie. 0 <= hp - 1 < p - 1 */
		hp = one + RandomBnd(p - one);

		/* Come up with hq s.t. 0 < hq < q */
		hq = one + RandomBnd(q - one);

		/* Compute the generator, h = hp^(q-1) * hq^(p-1) (mod n) */
		vector<ZZ> generatorBases;
		generatorBases.push_back(hp);
		generatorBases.push_back(hq);

		vector<ZZ> generatorExponents;
		generatorExponents.push_back(q-1);
		generatorExponents.push_back(p-1);
		generator = MultiExp(generatorBases, generatorExponents, modulus);
	}

	generators.push_back(generator);
	return generator;
}

bool GroupRSA::isElement(const ZZ &value) const {
	ZZ zero = to_ZZ(0), one = to_ZZ(1), modulus = getModulus();

	if(value <= zero)
		return false;
	if(value >= modulus)
		return false;
	if(GCD(value, modulus) != one)
		return false;

	return true;
}

void GroupRSA::debug() const {
	cout << "GroupRSA" << endl;
	cout << "p = " << p << endl;
	cout << "q = " << q << endl;
	cout << "n = " << modulus << endl;
	cout << "stat = " << stat << endl;
	Group::debug();
}

bool GroupRSA::isGenerator(const ZZ &number) const {
	ZZ one = to_ZZ(1), two = to_ZZ(2);

	if(!isElement(number))
		return false;
	// Should not be 1 or -1
	if(PowerMod(number, two, modulus) == one)
		return false;


	if(p == 0 || q == 0) {
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"Tried to call GroupRSA::isGenerator without knowing the "
				"prime factorization of the group modulus");
		/* TODO: Set an error flag here */
		return false;
	}
	ZZ pPrime = (p - one) / two;
	ZZ qPrime = (q - one) / two;


	/* The order of our group is lcm(2p', 2q') = 2p'q'. The order of QR_n
	 * is (2p'q'/2) = p'q'. So x is a non-generating element iff it satisfies
	 * one of the following equations:
	 * 	x^(p') == 1 (mod n)
	 * 	x^(q') == 1 (mod n)
	 */
	if(PowerMod(number, pPrime, modulus) == one)
		return false;
	if(PowerMod(number, qPrime, modulus) == one)
		return false;

	return true;
}

bool GroupRSA::checkPreconditions() const {
	// to be replaced with
	// return (isTTP(g->owner) || proof that g->getGenerators()
	// generates QR_n);
	return true;
	//TODO: Finish this
}
