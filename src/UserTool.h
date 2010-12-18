#ifndef _USERTOOL_H_
#define _USERTOOL_H_

#include "UserWithdrawTool.h"
#include "Buyer.h"
#include "Seller.h"
#include "FEInitiator.h"
#include "FEResponder.h"
#include "BankParameters.h"

class UserTool {
	
	public:
		UserTool(int st, int l, Ptr<const BankParameters> bp, 
				 const VEPublicKey &arbiterVPK, const VEPublicKey &arbiterPK, 
				 const hashalg_t &ha);

		UserTool(const UserTool &o)
			: stat(o.stat), lx(o.lx), 
			bankParameters(o.bankParameters),
			vepk(o.vepk), pk(o.pk), userSecretKey(o.userSecretKey), 
			userPublicKey(o.userPublicKey), hashAlg(o.hashAlg), 
			idProof(o.idProof) {}

		UserTool(const char *fname, Ptr<const BankParameters> bp,
				 const char *fnameVEPK, const char *fnamePK)
			: bankParameters(bp), vepk(fnameVEPK), pk(fnamePK)
			{ loadFile(make_nvp("UserTool", *this), fname); }
		
		// XXX: this was causing a seg fault...
		//~UserTool() { delete bankParameters; }

		// getters
		ZZ getPublicKey() const { return userPublicKey; }
		Ptr<const VEPublicKey> getVEPublicKey() const { return &vepk; }
		SigmaProof getIdentityProof() const { return idProof; }
		Ptr<const BankParameters> getBankParameters() const { return bankParameters; }

		Ptr<UserWithdrawTool> getWithdrawTool(int walletSize, int coinDenom) const {
			return Ptr<UserWithdrawTool>(
                new UserWithdrawTool(stat, lx, bankParameters, 
                                     userPublicKey, userSecretKey, 
                                     hashAlg, walletSize, coinDenom)
                );
        }

		/* Factory functions for user */
		Ptr<Buyer> getBuyer(time_t timeout) const 
            { return Ptr<Buyer>(new Buyer(timeout, &vepk, stat)); }

		Ptr<Seller> getSeller(time_t timeout, int timeoutTolerance) const 
            { return Ptr<Seller>(new Seller(timeout, timeoutTolerance, &vepk, stat)); }
		
		Ptr<Seller> getSeller(Ptr<EncBuffer> ctext, int t, int timeoutTolerance) const
            { return Ptr<Seller>(new Seller(ctext, t, timeoutTolerance, &vepk, stat)); }

		Ptr<FEInitiator> getFEInitiator(time_t timeout) const 
            { return Ptr<FEInitiator>(new FEInitiator(timeout, &vepk, &pk, stat)); }

		Ptr<FEResponder> getFEResponder(time_t t, int tolerance) const 
            { return Ptr<FEReponder>(new FEResponder(t, tolerance, &vepk, &pk, stat)); }

	private:
		int stat, lx;
		Ptr<const BankParameters> bankParameters;
        VEPublicKey vepk;
		VEPublicKey pk;
		ZZ userSecretKey;
		ZZ userPublicKey;
		hashalg_t hashAlg;
		SigmaProof idProof;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(stat)
				& auto_nvp(lx)
				// userKeyGroup, repk & vepk are loaded in 
				// UserTool(fname, params, vepk, repk) constructor
				& auto_nvp(userSecretKey) // XXX should be encrypted
				& auto_nvp(userPublicKey)
				& auto_nvp(hashAlg)
				& auto_nvp(idProof);
		}
};

#endif /*_USERTOOL_H_*/
