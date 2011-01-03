
#ifndef GROUPRSA_H_
#define GROUPRSA_H_

#include "Group.h"

/*! \brief This represents a special RSA group (so one where p and q are
 * Germain primes). */

void setGeneratorCheckMode(int mode);

class GroupRSA : public Group {
	public:
		/*! Create a new RSA group with the specified owner name
		 * "owner", a modulus (n = p * q) of length modulusLength,
		 * and a statistical safety parameter stat */
		GroupRSA(const string &owner, int modulusLength,
				int stat);

		GroupRSA(const string &owner, ZZ &modulus, int s)
			: Group(owner, modulus), stat(s) {}

		GroupRSA() : Group(), p(0), q(0), stat(0) {}
		
		/*! Copy constructor for an RSA group */
		GroupRSA(const GroupRSA &o)
			: Group(o), p(o.p), q(o.q), stat(o.stat) {}

		/*! Constructor to load from file */
		GroupRSA(const char *fname)
			: Group(), p(0), q(0), stat(0) 
			{ loadFile(make_nvp("GroupRSA", *this), fname); }

		// getters
		virtual ZZ getOrder() const;
		virtual ZZ getP() const { return p; }
		int getStat() const { return stat; }

		/*! Adds a new generator to list of generators in group */
		virtual ZZ addNewGenerator();

		/*! Select a random exponent -- useful for creating new group
		 * generators */
		virtual ZZ randomExponent() const 
						{ return RandomLen_ZZ(modulusLength + stat); }
		long randomnessDomain() const { return modulusLength + stat; }
		
		virtual bool isElement(const ZZ &value) const;
		virtual bool isGenerator(const ZZ &number) const;

		void clearSecrets() { p = 0; q = 0; }

		/*! debug */
		virtual void debug() const;

		virtual bool operator==(const GroupRSA& o) const {
		    return (Group::operator==(o) &&
                    p == o.p &&
                    q == o.q &&
                    stat == o.stat);
		}

	protected:
		/*! checks to make sure preconditions are satisfied */
		virtual bool checkPreconditions() const;

	private:
		/*! n = p * q; n is known and p,q are private */
		ZZ p, q;
		int stat;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Group);
			ar	& auto_nvp(p)
				& auto_nvp(q)
				& auto_nvp(stat)
				;
		}
};

#endif /*GROUPRSA_H_*/
