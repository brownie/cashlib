#include "UserWithdrawTool.h"
#include <assert.h>
#include "MultiExp.h"
#include "Timer.h"

UserWithdrawTool::UserWithdrawTool(int st, int l, const BankParameters *bp, 
								   const ZZ &userPK, const ZZ &userSK, 
								   const hashalg_t &ha, int ws, int denom)
	: stat(st), lx(l), bankParameters(bp), userSecretKey(userSK), 
	  userPublicKey(userPK), hashAlg(ha), walletSize(ws), coinDenom(denom),
	  signatureRecipient(0), verified(false)
{
}

UserWithdrawTool::UserWithdrawTool(const UserWithdrawTool &o)
	: stat(o.stat), lx(o.lx), bankParameters(o.bankParameters),
	  userSecretKey(o.userSecretKey), userPublicKey(o.userPublicKey),
	  hashAlg(o.hashAlg), walletSize(o.walletSize),
	  coinDenom(o.coinDenom), verified(o.verified)
{
}

UserWithdrawTool::~UserWithdrawTool() {
	delete signatureRecipient;
}

ZZ UserWithdrawTool::createPartialCommitment() {
	const GroupPrime* cashGroup = bankParameters->getCashGroup();
	// pick s' and t : s' and t should be in (Z, *, primeOrder)
	// ie. 1 <= s', t < primeOrder
	// where primeOrder is the order of the primeOrder group used for wallet
	ZZ one = to_ZZ(1);
	ZZ orderCap = cashGroup->getOrder() - one;

	ZZ sPrime = one + RandomBnd(orderCap);
	ZZ t = one + RandomBnd(orderCap);
	
	// create commitment to user secret key using bases from cash group
	// want to use g and h (second and third generators)
	vector<ZZ> bases;
	bases.push_back(cashGroup->getGenerator(1));
	bases.push_back(cashGroup->getGenerator(2));
	ZZ mod = cashGroup->getModulus();

	// create commitment to	sk_u 
	ZZ skRandomness = cashGroup->randomExponent();
	vector<ZZ> skExps;
	skExps.push_back(userSecretKey);
	skExps.push_back(skRandomness);
	ZZ skCom = MultiExp(bases, skExps, mod);

	// create commitment to s'
	ZZ sPrimeRandomness = cashGroup->randomExponent();
	vector<ZZ> sPrimeExps;
	sPrimeExps.push_back(sPrime);
	sPrimeExps.push_back(sPrimeRandomness);
	ZZ sPrimeCom = MultiExp(bases, sPrimeExps, mod);
	
	// create commitment to t
	ZZ tRandomness = cashGroup->randomExponent();
	vector<ZZ> tExps;
	tExps.push_back(t);
	tExps.push_back(tRandomness);
	ZZ tCom = MultiExp(bases, tExps, mod);

	// now create partial commitment according to specs
	vector<ZZ> result;
	result.push_back(skRandomness); 	
	result.push_back(skCom);
	result.push_back(sPrime);
	result.push_back(sPrimeRandomness);
	result.push_back(sPrimeCom);
	result.push_back(t);
	result.push_back(tRandomness);
	result.push_back(tCom);
	partialCommitment = result;
	return sPrimeCom;
}

ProofMessage* UserWithdrawTool::initiateSignature(const ZZ &bankPart) {
	bankContribution = bankPart;
	createSignatureRecipient();
	return preSignatureProof();
}

vector<ZZ> UserWithdrawTool::getIndividualCommitments() {
	// assemble individual commitments to user's secret key, s, and t
	vector<ZZ> privateComs;
	privateComs.push_back(partialCommitment[1]); // Com(sk_u)
	privateComs.push_back(partialCommitment[4]); // Com(s)
	privateComs.push_back(partialCommitment[7]); // Com(t)
	return privateComs;
}

ProofMessage* UserWithdrawTool::getCLProof() const {
	vector<pair<ZZ,ZZ> > secrets;
	// first sk_u, then s, then t
	secrets.push_back(make_pair(userSecretKey, partialCommitment[0]));
	secrets.push_back(make_pair(s, partialCommitment[3]));
	secrets.push_back(make_pair(partialCommitment[5], partialCommitment[6]));
	return signatureRecipient->getC(secrets, hashAlg);
}

ProofMessage* UserWithdrawTool::preSignatureProof() {
	// need to prove that commitment to sk_u and public key contain
	// the same value in the exponent
	InterpreterProver prover;
	group_map g;
	g["cashGroup"] = bankParameters->getCashGroup();
	prover.check("ZKP/examples/presig.txt", g);
	variable_map v;
	v["A"] = partialCommitment[1];
	v["r_u"] = partialCommitment[0];
	v["pk_u"] = userPublicKey;
	v["sk_u"] = userSecretKey;
	prover.compute(v);
	return new ProofMessage(prover.getPublicVariables(), 
							prover.computeProof(hashAlg));
}

void UserWithdrawTool::createSignatureRecipient() {
	if(bankContribution == 0) {
		throw CashException(CashException::CE_UNKNOWN_ERROR,
			"[UserWithdrawTool:createSignatureRecipient] bank's contribution "
			"has not been set yet.");
	}
	
	// compute s = s' + r'
	const GroupPrime* cGroup = bankParameters->getCashGroup();
	ZZ sPrime = partialCommitment[2];
	ZZ s = AddMod(sPrime, bankContribution, cGroup->getOrder());
	this->s = s;
	
	// instead of recomputing the whole commitment, just update
	ZZ partialBase = PowerMod(cGroup->getGenerator(1), bankContribution, 
							  cGroup->getModulus());
	partialCommitment[4] = MulMod(partialCommitment[4], partialBase, 
								  cGroup->getModulus());

	// need individual commitments to sk_u, s, and t
	startTimer();
	vector<ZZ> coms = getIndividualCommitments();

	// 3 private messages: sk_u, s, t
	// 1 public message: walletSize
	const GroupRSA* pk = bankParameters->getBankKey(coinDenom);
	const GroupPrime* comGroup = bankParameters->getCashGroup();								  
	signatureRecipient = new CLBlindRecipient(pk, comGroup, lx, coms, 3, 1);
	printTimer("[UserWithdrawTool] created recipient");
}
 
vector<ZZ> UserWithdrawTool::verify(const ProofMessage &proofEInverse) {
	SigmaProof proof = proofEInverse.proof;
	// XXX: is this important to look into?
	// it seems like we probably haven't done this check everywhere that we were
	// suppose to so far
	// first check that challenge was computed correctly
	// XXX: why isn't this version of hash here anymore?
	//ZZ myHash = Hash::hash(proof.getRandomizedProofs(), hashAlg);
	
	//if(myHash != proof.computeChallenge()[0]) {
	//	throw CashException(CashException::CE_UNKNOWN_ERROR,
	//		"[UserWithdrawTool:verify] bank's sigma proof had bad challenge");
	//}
	
	// for now, returning partial signature
	vector<ZZ> partial;
	partial.push_back(proofEInverse.vars.at("A"));
	partial.push_back(proofEInverse.vars.at("e"));
	partial.push_back(proofEInverse.vars.at("vdoubleprime"));
	if(signatureRecipient->verifySig(proofEInverse, stat)) {
		verified = true;
		return partial;
	} else {
		throw CashException(CashException::CE_FE_ERROR,
						"[UserWithdrawTool::verify] proof didn't verify");
	}
}

// returns a wallet with signature from bank
// will not return wallet if bank has not proved knowledge of e inverse
Wallet UserWithdrawTool::getWallet(const vector<ZZ> &partialSig) const {
	if(!verified) {
		throw CashException(CashException::CE_UNKNOWN_ERROR,
			"[UserWithdrawTool::getWallet] Bank's sigma proof not verified");
	}
	vector<ZZ> signature = signatureRecipient->createSig(partialSig);
	
	ZZ t = partialCommitment[5];
	return Wallet(userSecretKey, s, t, walletSize, coinDenom, 
				  bankParameters, stat, lx, hashAlg, signature);
}
