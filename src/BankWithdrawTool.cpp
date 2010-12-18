	
#include "BankWithdrawTool.h"
#include "CashException.h"
#include "CLBlindIssuer.h"

BankWithdrawTool::BankWithdrawTool(Ptr<const BankParameters> bp, const ZZ &userPK, 
								   int st, int l, int wSize, int denom, 
								   const hashalg_t &ha)
	: bankParameters(bp), userPublicKey(userPK), stat(st), lx(l), 
	  walletSize(wSize), coinDenom(denom), hashAlg(ha)
{
	int allowedWalletSizes[] = {1,5,10,25,50,100,250,500,1000};
	bool walletSizeValid = false;
	for (int i = 0; i < NUM_WALLET_SIZES; i++) {
		if (walletSize == allowedWalletSizes[i])
			walletSizeValid = true;
    }
	if (!walletSizeValid)
		throw CashException(CashException::CE_SIZE_ERROR,
			"[BankWithdrawTool::BankWithdrawTool] invalid wallet size given");
}

BankWithdrawTool::BankWithdrawTool(const BankWithdrawTool &o)
	: bankParameters(new_ptr<BankParameters>(*o.bankParameters)), 
	  userPublicKey(o.userPublicKey), stat(o.stat), lx(o.lx),
	  walletSize(o.walletSize), coinDenom(o.coinDenom),
	  bankContribution(o.bankContribution), hashAlg(o.hashAlg)
{
}

void BankWithdrawTool::computeFullCommitment(const ZZ &partialCommitment) {
	ZZ g = bankParameters->getCashGroup()->getGenerator(0);
	ZZ mod = bankParameters->getCashGroup()->getModulus();
	ZZ zero = to_ZZ(0);
	while(bankContribution == zero) {
		bankContribution = bankParameters->getCashGroup()->randomExponent();
	}
	ZZ partialBase = PowerMod(g, bankContribution, mod);
	ZZ resultA = MulMod(partialCommitment, partialBase, mod);
	fullCommitment = resultA;
}

Ptr<ProofMessage> BankWithdrawTool::sign(Ptr<ProofMessage> id, Ptr<ProofMessage> cl){
	// first verify ID proof, then create issuer for the CL part
	InterpreterVerifier verifier;
	group_map g;
	g["cashGroup"] = bankParameters->getCashGroup();
	verifier.check("ZKP/examples/presig.txt", g);
	variable_map v;
	v["pk_u"] = userPublicKey;
	verifier.compute(v, id->publics);
	bool idVerified = verifier.verify(id->proof, stat);
	if (!idVerified) {
		throw CashException(CashException::CE_UNKNOWN_ERROR,
				"[BankWithdrawTool::sign] Proof of ID did not verify");
	}
	variable_map clPubs = cl->publics;
	Ptr<const GroupRSA> sk = bankParameters->getBankKey(coinDenom);
	Ptr<const GroupPrime> comGroup = bankParameters->getCashGroup();
	vector<ZZ> coms;
	for (int i = 0; i < 3; i++) {
		string name = "c_"+lexical_cast<string>(i+1);
		coms.push_back(clPubs.at(name));
	}

	CLBlindIssuer issuer(sk, comGroup, lx, coms, 3, 1);
	vector<ZZ> publicMsg;
	publicMsg.push_back(to_ZZ(walletSize));
	ZZ C = cl->vars.at("C");
	return issuer.getPartialSignature(C, publicMsg, *cl, stat, hashAlg);
}
