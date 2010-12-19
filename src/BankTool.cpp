		
#include "BankTool.h"
#include "ZKP/InterpreterVerifier.h"

BankTool::BankTool(int st, int l, int modLen, const hashalg_t &ha, 
				   vector<int> &coinDenoms)
	: stat(st), lx(l), hashAlg(ha)
{
	vector<Ptr<GroupRSA> > secretKeys;
	for(unsigned i = 0; i < coinDenoms.size(); i++) {
		Ptr<GroupRSA> g = new_ptr<GroupRSA>("bank", modLen, stat);
		// initial generator is f
		// want to add g_1,...,g_4,h
		for (int j = 0; j < 5; j++) {
			g->addNewGenerator();
		}
		secretKeys.push_back(g);
	}

	// generator: f (different from RSA group)
	Ptr<GroupPrime> cashGroup = 
        new_ptr<GroupPrime>("bank", modLen, 2*stat, stat);
	// additional endorsed ecash group generators: g, h, h1, h2
	for (int i = 0; i < 4; i++) {
		cashGroup->addNewGenerator();
	}
	
	bankParameters = new_ptr<BankParameters>(secretKeys, cashGroup, coinDenoms);
		
	publicBankParameters = new_ptr<BankParameters>(secretKeys, cashGroup, 
													coinDenoms);
	publicBankParameters->makePublic();
}

BankTool::BankTool(int st, int l, const hashalg_t &ha,
				   BankParameters bp)
	: stat(st), lx(l), hashAlg(ha)
{
	bankParameters = new_ptr<BankParameters>(bp);
	publicBankParameters = new_ptr<BankParameters>(bp);
	publicBankParameters->makePublic();
}

Ptr<BankWithdrawTool> BankTool::getWithdrawTool(const ZZ &userPK, int wSize, 
											int coinDenom) const {
	return new_ptr<BankWithdrawTool>(bankParameters, userPK, stat, lx, wSize, 
								coinDenom, hashAlg);
}

bool BankTool::verifyIdentity(Ptr<ProofMessage> idProof, const ZZ &userPK) const {
	// check user's PoK of sk_u such that pk_u = g^sk_u
	InterpreterVerifier verifier;
	group_map g;
	g["G"] = bankParameters->getCashGroup();
	verifier.check("ZKP/examples/userid.txt", input_map(), g);
	variable_map v;
	v["pk_u"] = userPK;
	verifier.compute(v, idProof->publics);
	return verifier.verify(idProof->proof, stat);
}

bool BankTool::verifyCoin(const Coin &coin) const {
	return coin.verifyCoin();
}

bool BankTool::isCoinDoubleSpent(const Coin &coin1, const Coin &coin2) const {
	// throw an exception if these coins do not share the same S value
	if(coin1.getSPrime() != coin2.getSPrime())
	{
		throw CashException(CashException::CE_UNKNOWN_ERROR,
			"[BankTool::checkIfCoinDoubleSpent] Coins do not share same serial " 
			"coin point and are not valid candidates for checking double "
			"spending.");
	}
	
	// if contract hashes are same, then both coins were spent in the same 
	// transaction so the coin was not double spent
	return (coin1.getR() != coin2.getR());	 
}

ZZ BankTool::identifyDoubleSpender(const Coin& coin1, const ZZ &tPrime2, 
								   const ZZ& rValue2) const {
	// Should probably check that the R values are different
	ZZ mod = coin1.getCashGroup()->getModulus();
	ZZ order = coin1.getCashGroup()->getOrder();
	ZZ t1 = coin1.getTPrime();
    ZZ t2 = tPrime2;
	ZZ r1 = coin1.getR();
    ZZ r2 = rValue2;

    if (r2 > r1) {
		NTL::swap(r2, r1);
		NTL::swap(t2, t1);
    }

	ZZ exp = InvMod(r1 - r2, order);
	ZZ num = PowerMod(t2, r1, mod);
	ZZ denom = InvMod(PowerMod(t1, r2, mod), mod);
	ZZ base = MulMod(num, denom, mod);
	ZZ publicKeyUser = PowerMod(base, exp, mod);
	
	return publicKeyUser;
}

ZZ BankTool::identifyDoubleSpender(const Coin& coin1, const Coin& coin2) const {
    return identifyDoubleSpender(coin1, coin2.getTPrime(), coin2.getR());
}
