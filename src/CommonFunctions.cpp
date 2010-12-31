
#include "CommonFunctions.h"
#include "Debug.h"
#include "FourSquares.h"
#include "base64.h"

// Singleton object holding some global config values
struct Config {
    Config() : zkp_program_dir("./ZKP/examples") {}

    // location of programs in cashlib/src/ZKP/examples/ used by various Cashlib wrapper classes
    std::string zkp_program_dir;
};

Config& getConfig() {
    static Config pc; // lazy-initialized singleton
    return pc;
}

// accessors for config
string CommonFunctions::getZKPDir() {
    return getConfig().zkp_program_dir;
}

void CommonFunctions::setZKPDir(const string& dir) {
    getConfig().zkp_program_dir = dir;
}

bool CommonFunctions::isTTP(const string &name) {
	if ( name.compare("bank") == 0 ||  name.compare("TTP") == 0
		|| name.compare("arbiter") == 0)
		return true;
	return false;
}

// p is prime and p = 1 mod 4
vector<ZZ> CommonFunctions::decomposePrime(const ZZ &p, int stat) {
	ZZ b;
	if((p % 8) == to_ZZ(5)) {
		b = to_ZZ(2);
	}
	else {
		b = to_ZZ(3);
		while(PowerMod(b, (p-1)/2, p) == 1) {
			// next prime(b) returns the smallest prime larger than b
			b = NextPrime(b, stat);
		}
	}

	b = PowerMod(b, (p-1)/4, p);
	// b is now an imaginary unit, i.e. b^2 = -1 mod p

	ZZ a;
	a = p;
	while(power(b, 2) > p) {
		ZZ temp = a;
		a = b;
		b = temp % b;
	}
	// cout << "a : " << a << "   b : " << b << endl;
	vector<ZZ> twoSquares;
	twoSquares.push_back(b);
	twoSquares.push_back(a % b);
	return twoSquares;
}


#include <sys/time.h>

/* Determine the difference, in milliseconds, between two struct timevals. */
#define TV_DIFF_MS(a, b) \
    (((b).tv_sec - (a).tv_sec) * 1000 + ((b).tv_usec - (a).tv_usec) / 1000)

void dispt(timeval start, timeval end, int cnt, const char *name) {
    long tot = TV_DIFF_MS(start, end);
    cout << "time for " << name << " (" << cnt << " decomps): "
         << tot << "ms, " << tot/(float)cnt << "ms/decomp" << endl;
}

//#define DEBUG
vector<ZZ> CommonFunctions::decompose(const ZZ &a) {
#ifdef DEBUG
	cout << a << " bits: " << NumBits(a) << endl;
#endif
	timeval start, end;
	gettimeofday(&start, NULL);
	vector<ZZ> res = FourSquares::decompose(a);
	gettimeofday(&end, NULL);
    //dispt(start, end, 1, "CommonFunctions::decompose");

	vector<ZZ> result(4);
	result[0] = res[0];
	result[1] = res[1];
	result[2] = res[2];
	result[3] = res[3];

#ifdef DEBUG
	cout << result[0] << " " << result[1] << " " << result[2] << " " << result[3] << endl;
#endif
	return result;
}

vector<ZZ> CommonFunctions::subvector(const vector<ZZ> &vec, unsigned start, 
									  unsigned length) {
	if(length < 1) {
		vector<ZZ> blank;
		return blank;
	}
	if(start >= vec.size() || start+length-1 >= vec.size()) {
		// the 1st check is actually redundant
		throw CashException(CashException::CE_SIZE_ERROR,
				"[CommonFunctions::subvector] Attempt to access elements "
				"beyond the end of the input vector");
	}
	vector<ZZ> result;
	for(unsigned i = 0; i < length; i++) {
		result.push_back(vec[start+i]);
	}
	return result;
}

ZZ CommonFunctions::abs(const ZZ& x, const ZZ& bigNsquared) {
	// should we check precondition that x is in (0, N^2)?
	if (x > bigNsquared / 2) {
		return (bigNsquared - x) % bigNsquared;
	}
	else {
		return x % bigNsquared;
	}
}
