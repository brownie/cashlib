/*!
 * \brief This represents a prime-order group, which is a Group.
 */

#ifndef GROUPPRIME_H_
#define GROUPPRIME_H_

#include "Group.h"

class GroupPrime : public Group {
	public:
		/*! Create a new prime-order group with the specified owner,
		 * a modulus of length modulusLength, a group
		 * order of length orderLength, and a statistical security
		 * parameter "stat" */
		GroupPrime(const string &owner, int modulusLength,
				   int orderLength, int stat);
		
		/*! Copy constructor -- currently doesn't check pre- or
		 * post-conditions */
		GroupPrime(const GroupPrime &o)
			: Group(o), order(o.order), factor(o.factor), stat(o.stat) {}

		/*! Constructor to load from file */
		GroupPrime(const char *fname)
			: Group(), order(0), factor(0), stat(0) 
			{ loadFile(make_nvp("GroupPrime", *this), fname); }

		virtual ZZ getOrder() const { return order; }
		virtual ZZ randomExponent() const { return RandomBnd(order); }
		long randomnessDomain() const { return orderLength; }

		/*! Adds a new generator to list of generators in group */
		virtual ZZ addNewGenerator();

		/*! Check if value is an element of our group */
		virtual bool isElement(const ZZ &value) const;

		/*! Check if gen is a generator for this group */
		virtual bool isGenerator(const ZZ &gen) const;

		/*! Check if all generators stored in group actually
		 * generate group */
		bool checkGroupSetup() const;

		virtual void debug() const;

		virtual bool operator==(const GroupPrime& o) const {
		    return (Group::operator==(o) &&
                    order == o.order &&
                    factor == o.factor &&
                    stat == o.stat);
		}

	protected:
		/*! returns true iff 1 <= exp < order */
		bool isProperExponent(const ZZ &exp) const;

		/*! checks to make sure the group satisfies certain properties */
		virtual bool checkPreconditions() const;

	private:
		/*! this is for serialization */
		GroupPrime() : Group(), order(0), factor(0), stat(0) {}

		/*! modulus = order*factor + 1 */
		ZZ order;
		ZZ factor;
		int stat;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Group);
			ar 	& auto_nvp(order)
				& auto_nvp(stat)
				& auto_nvp(factor)
				;
		}
};

#endif /*GROUPPRIME_H_*/
