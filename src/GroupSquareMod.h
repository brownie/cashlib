
#ifndef _GROUPSQUAREMOD_H
#define	_GROUPSQUAREMOD_H

#include "Group.h"

class GroupSquareMod : public Group {
	public:
		GroupSquareMod(const string &owner, ZZ modulus, int stat);
		
		/*! Copy constructor -- currently doesn't check pre- or
		 * post-conditions */
		GroupSquareMod(const GroupSquareMod &o) : Group(o), stat(o.stat) {}
		
		// XXX: should we actually fill this in?
		virtual ZZ getOrder() const { return ZZ(); }
		virtual ZZ addNewGenerator() { return ZZ(); }

		/*! Create a random exponent -- for use when we're trying
		 * to create new random generators for the group */
		virtual ZZ randomExponent() const 
								{ return RandomLen_ZZ(modulusLength + stat); }
		long randomnessDomain() const { return modulusLength + stat; }

		/*! Check if value is an element of our group */
		virtual bool isElement(const ZZ &value) const;

		/*! Check if gen is a generator for this group */
		virtual bool isGenerator(const ZZ &gen) const;
		
		virtual void debug() const;
		
		// XXX: fill this in?
		/*! Used to determine whether the group is trusted. */
		virtual bool checkPreconditions() const {return true;};

	private:
		int stat;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Group)
				& auto_nvp(stat);
		}
};



#endif	/* _GROUPSQUAREMOD_H */

