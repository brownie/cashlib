
#include "Bank.h"

ZZ Bank::randomNumber() {
	ZZ result = groupPrime->randomExponent();
	while(result == 0) {
		result = groupPrime->randomExponent();
	}
	return result;
}

ZZ Bank::fullCommitment(const ZZ &partial, const ZZ &bankPart) {
	ZZ g = groupPrime->getGenerator(0);
	ZZ mod = groupPrime->getModulus();
	return  MulMod(partial, PowerMod(g, bankPart, mod), mod);
}
