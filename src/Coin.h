
#ifndef _COIN_H_
#define _COIN_H_

#include "ZKP/Environment.h"
#include "SigmaProof.h"
#include "Hash.h"
#include "BankParameters.h"

class Coin {

	public:
		/*! coin stores the user's secret sk_u, s, and t, as well
		 * as a CL signature on these values, the index within the 
		 * wallet, the coin denomination, and the R value for the coin */
		Coin(Ptr<const BankParameters> params, int wSize, int index,
			 const ZZ &skIn, const ZZ &sIn, const ZZ &tIn, 
			 const vector<ZZ> &clSig, int st, int l, const ZZ &rVal, 
			 int denom, const hashalg_t &hashAlg);

		Coin(const char *fname, Ptr<const BankParameters> params)
			: parameters(params)
			{ loadFile(make_nvp("Coin", *this), fname); }

		Coin(const string& s, Ptr<const BankParameters> params)
			: parameters(params)
			{ loadGZString(make_nvp("Coin", *this), s); }

		Coin(const Coin &o);
	
		Coin() {} // required by seller (for now at least)

		bool verifyCoin() const;

		/*! clears endorsement */
		void unendorse() { endorsement.clear(); }

		/*! checks to see if endorsement matches our stored y value */
		bool verifyEndorsement(const vector<ZZ> &e);

		/*! endorses coin if endorsement is valid */
		bool endorse(const vector<ZZ> &e);

		// getters
		Ptr<const GroupPrime> getCashGroup() const 
						{ return parameters->getCashGroup(); } 
		ZZ getWalletSize() const { return walletSize; }
		ZZ getB() const { return B; }
		ZZ getSCommitment() const { return C; }
		ZZ getTCommitment() const { return D; }
		ZZ getS() const { return S; }
		ZZ getT() const { return T; }
		vector<ZZ> getEndorsement() const { return endorsement; }
		ZZ getEndorsementCom() const { return y; }
		ZZ getR() const { return R; }
		int getDenom() const { return coinDenom; }
		// XXX: right now, I think this only exists for testing
		int getIndex() const { return coinIndex; }

		ZZ getSPrime() const;
		ZZ getTPrime() const;

		void setParameters(Ptr<const BankParameters> p) { parameters = p; }

		// XXX: can write debug later
		void debug() const;

		hash_t hash() const;

	private:
		int stat, lx, coinDenom;
		Ptr<const BankParameters> parameters; // NOT serialized
		int walletSize; // this is W
		int coinIndex; // this is J
		ZZ sk_u, s, t; // NOT serialized
		ZZ R;
		hashalg_t hashAlg; // XXX NOT serialized (??)
		ZZ B, C, D; // B is commitment to sk_u, C to s, and D to t
		ZZ S, T; // S = g^(1/(s+J)), T = g^(1/(t+J))
		vector<ZZ> endorsement; // this is x1, x2, r_y
		vector<ZZ> signature; // this is A, e, v (signature on sk_u, s, t)
		ZZ y; // y = h1^x1 * h2^x2 * f^r_y (endorsement commitment)
		ProofMessage coinProof, clProof;
		
		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(stat)
			    & auto_nvp(lx)
			    & auto_nvp(signature)
			    & make_nvp("B", B)
			    & make_nvp("C", C)
			    & make_nvp("D", D)
			    & make_nvp("W", walletSize)
			    & make_nvp("S", S)
			    & make_nvp("T", T)
			    & make_nvp("J", coinIndex)
			    & make_nvp("R", R)
				& auto_nvp(coinProof)
				& auto_nvp(clProof)
			    & auto_nvp(endorsement)
			    & auto_nvp(y)
			    & auto_nvp(coinDenom)
				;
		}
};

#endif /*_COIN_H_*/
