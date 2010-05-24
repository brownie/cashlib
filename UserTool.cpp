#include "UserTool.h"

UserTool::UserTool(int st, int l, const BankParameters *bp,
				   const VEPublicKey &vPK, const VEPublicKey &rPK, 
				   const hashalg_t &ha)
	: stat(st), lx(l), bankParameters(new BankParameters(*bp)),
	  vepk(vPK), pk(rPK), hashAlg(ha)
{
	const GroupPrime* cashGroup = bankParameters->getCashGroup(); 
	userSecretKey = cashGroup->randomExponent();
	 
	// now compute user public key = g^sk_u
	ZZ g = cashGroup->getGenerator(1);
	userPublicKey = PowerMod(g, userSecretKey, cashGroup->getModulus());
	// also compute PoK of sk_u (useful later)
	// set up environment
	group_map groups;
	groups["G"] = cashGroup;
	variable_map v;
	v["pk_u"] = userPublicKey;
	v["sk_u"] = userSecretKey;
	// now compute proof and save it
	InterpreterProver p;
	p.check("ZKP/examples/userid.txt", groups);
	p.compute(v);
	idProof = p.computeProof(hashAlg);
}
