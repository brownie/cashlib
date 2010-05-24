/*
 * A group has the following:
 * - group type
 * - modulus
 * - order
 * - generators
 *
 * For debugging purposes, we need to keep the "owner" of a Group.
 */

#ifndef GROUP_H_
#define GROUP_H_

#include <NTL/ZZ.h>
#include <iostream>
#include <vector>
#include "Serialize.h"

NTL_CLIENT

/*! \brief This class represents an algebraic group.
 */

class Group {
	public:		
		static const int TYPE_RSA = 0;
		static const int TYPE_PRIME = 1;
		static const int TYPE_SQUARE = 2;
		
		/*! Represent a group with the specified owner (a string),
		 * and a modulus of length modulusLength. */
		Group(const string &owner, int modulusLength, int orderLength);

		/*! Copy constructor. */
		Group(const Group &original);

		Group(const string &owner, const ZZ &mod);

		/*! Destructor. */
		virtual ~Group() {}

		/*! get a random group element */
		ZZ randomElement();
		
		/*! Perform the precondition checks, and set isTrusted to true if
		 * they pass. */
		void makeTrusted();		
		
		// getters
		int getModulusLength() const { return modulusLength; }
		int getOrderLength() const { return orderLength; }
		ZZ getModulus() const { return modulus; }
		vector<ZZ> getGenerators() const { return generators; }
		// returns the first generator
		ZZ getGenerator() const { return generators[0]; }
		ZZ getGenerator(int i) const { 
			assert((size_t)i < generators.size());
			return generators[i]; 
		}
		int numGenerators() const { return generators.size(); }
		bool getIsTrusted() const { return isTrusted; }
		int getType() const { return type; }

		/*! The order (number of elements) in the group. This may return 0
		 * if the order is not known. The length of the order should be
		 * equal to orderLength. */
		virtual ZZ getOrder() const = 0;

		/*! Adds a new generator to list of generators in group */
		virtual ZZ addNewGenerator() = 0;

		/*! Returns an exponent in the right range according to the group
		 * order. */
		virtual ZZ randomExponent() const = 0;
		
		/*! Returns randomness domain (number of random bits necessary for
		 * random exponents */
		virtual long randomnessDomain() const = 0;

		/*! Determines whether a value is an element of the group. */
		virtual bool isElement(const ZZ &value) const = 0;
		virtual bool isGenerator(const ZZ &value) const = 0;

		/*! Returns a copy of the group. */
		Group* copy() const;

		/*! Debugging code. */
		virtual void debug() const = 0;

	protected:
		/*! An empty constructor for loading serialized Groups
		 * from derived classes. */
		Group() 
			: modulusLength(0), orderLength(0), generators(),
			  isTrusted(false), modulus(to_ZZ(0)), type(0) {}

		/*! Used to determine whether the group is trusted. */
		virtual bool checkPreconditions() const = 0;

		int modulusLength;
		int orderLength;
		vector<ZZ> generators;
		bool isTrusted;
		ZZ modulus;
		int type;

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(modulusLength)
				& auto_nvp(modulus)
				& auto_nvp(orderLength)
				& auto_nvp(generators)
				& auto_nvp(isTrusted)
				& auto_nvp(type)
				;
		}
};			   
			   
#endif /*GROUP_H_*/
