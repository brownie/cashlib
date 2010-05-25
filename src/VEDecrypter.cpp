
#include "VEDecrypter.h"
#include "CommonFunctions.h"
#include "Timer.h"
#include "GroupSquareMod.h"
#include "ZKP/InterpreterProver.h"

VEDecrypter::VEDecrypter(const int m, const int modLength, const int stat) {
	setup(m, modLength, stat, 0);
}

VEDecrypter::VEDecrypter(const int m, const int modLength, const int stat, 
						 GroupRSA* auxGroup) {
	setup(m, modLength, stat, auxGroup);
}

void VEDecrypter::setup(const int m, const int modLength, 
						const int stat, GroupRSA* auxGroup) {
	GroupRSA group1("arbiter", modLength, stat);
	ZZ bigN = group1.getModulus();
	ZZ bigP = group1.getP();
	ZZ bigQ = bigN / bigP;
	ZZ bigNsquared = power(bigN, 2);
	// f0 is random element of Z_{N^2}*
	ZZ f0 = RandomBnd(bigNsquared);
	while (GCD(f0, bigNsquared) != 1) {
		f0 = RandomBnd(bigNsquared);
	}
	GroupSquareMod* grp = new GroupSquareMod("arbiter", bigNsquared, stat);
	group_map g;
	variable_map v;
	g["RSAGroup"] = &group1;
	g["G"] = grp;
	v["f0"] = f0;

	InterpreterProver prover;
	input_map inputs;
	inputs["m"] = m;
	// XXX: should we give groups in at beginning?  this program is really
	// only run once...
	prover.check("ZKP/examples/vedecrypter.txt", inputs, g);
	prover.compute(v);
	Environment env = prover.getEnvironment();
	ZZ d = env.variables.at("d");
	ZZ b = env.variables.at("b");
	ZZ e = env.variables.at("e");
	ZZ f = env.variables.at("f");
	vector<ZZ> as(m);
	vector<ZZ> xs(m+2);
	for (variable_map::iterator it = env.variables.begin();
								it != env.variables.end(); ++it) {
		if (it->first.find("x_") != string::npos) {
			int index = lexical_cast<int>(it->first.substr(2));
			xs[index-1] = it->second;
		} else if (it->first.find("a_") != string::npos) {
			int index = lexical_cast<int>(it->first.substr(2));
			as[index-1] = it->second;
		}
	}
	xs[m] = env.variables.at("y");
	xs[m+1] = env.variables.at("z");

	GroupRSA *group2;
	// if we weren't given a group, make one
	if (auxGroup == 0) {
		group2 = createSecondGroup(m, modLength, stat);
	}
	else {
		group2 = auxGroup;
	}
	
	ZZ n = group2->getModulus();
	ZZ p = group2->getP();
	ZZ q = n / p;
	sk = new VESecretKey(bigP, bigQ, xs, p, q);
	group2->clearSecrets();
	// XXX obviously this will not stay with hashKey as ""
	string hashKey = ""; // won't call HMAC
	hashalg_t hashAlg = Hash::SHA1; // XXX should be input parameter
	pk = new VEPublicKey(bigN, as, b, d, e, f, *group2, hashAlg, hashKey);
}


GroupRSA* VEDecrypter::createSecondGroup(const int m, const int modLength, 
										 const int stat) {
	// now initialize second group
	GroupRSA *group2 = new GroupRSA("arbiter", modLength, stat);
	for (int i = 0; i < m; i++) {
		// create m additional generators
		group2->addNewGenerator();
	}
	return group2;
}


vector<ZZ> VEDecrypter::decrypt(const vector<ZZ> &ciphertext, 
								const string& label, 
								const hashalg_t& hashAlg) const {
#ifdef TIMER
	startTimer();
#endif
	// get ciphertext u_1, ..., u_m, v, w
	unsigned n = ciphertext.size();
	unsigned m = n - 2;
	ZZ v = ciphertext[n-2];
	ZZ w = ciphertext[n-1];
	ZZ bigN = pk->getN();
	ZZ bigNsquared = power(bigN, 2);
	// need to have a vector u_1, ..., u_m, v
	vector<ZZ> hashvec = CommonFunctions::subvector(ciphertext, 0, n-1);
	ZZ y = sk->getY();
	ZZ z = sk->getZ();
	string hkey = pk->getHashKey();
	
	ZZ h = ZZFromBytes(Hash::hash(hashvec, label, hashAlg, hkey));
	ZZ v2 = PowerMod(v, 2, bigNsquared);
	ZZ vyz = PowerMod(v2, y + z*h, bigNsquared);
	
	if (CommonFunctions::abs(w, bigNsquared) != w || 
			vyz != PowerMod(w, 2, bigNsquared)) {
		throw CashException(CashException::CE_UNKNOWN_ERROR, 
						"[VEDecrypter::decrypt] ciphertext not formed properly");
	}
	
	ZZ t = InvMod(to_ZZ(2), bigN);
	ZZ vinv = InvMod(v, bigNsquared);
	ZZ t2 = t*2;
	
	vector<ZZ> mValues;
	vector<ZZ> xs = sk->getXValues();
	for (unsigned i = 0; i < m; i++) {
		ZZ ui = ciphertext[i];
		ZZ xi = xs[i];
		
		ZZ vinvx = PowerMod(vinv, xi, bigNsquared);
		ZZ uv = MulMod(ui , vinvx , bigNsquared);
		ZZ mi0 = PowerMod(uv , t2, bigNsquared);
		
		ZZ mi = ((mi0 - 1) / bigN) % bigNsquared;
		if (mi <= 0 || mi >= bigN) {
			throw CashException(CashException::CE_UNKNOWN_ERROR,
						"[VEDecrypter::decrypt] error in forming plaintext"); 
		}
		else {
			mValues.push_back(mi);
		}
	}
	
#ifdef TIMER
	printTimer("[VEDecrypter] decrypt");
#endif
	return mValues;
}

