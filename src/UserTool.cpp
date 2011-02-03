#include "UserTool.h"

UserTool::UserTool(int st, int l, 
                   Ptr<const BankParameters> bp,
				   Ptr<const VEPublicKey> vPK, 
                   Ptr<const VEPublicKey> rPK, 
				   const hashalg_t &ha)
	: stat(st), lx(l), 
      bankParameters(bp),
	  vepk(vPK), pk(rPK), 
      hashAlg(ha)
{
	Ptr<const GroupPrime> cashGroup = bankParameters->getCashGroup(); 
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
	p.check(CommonFunctions::getZKPDir()+"/userid.txt", groups);
	p.compute(v);
	idProof = p.computeProof(hashAlg);
}
