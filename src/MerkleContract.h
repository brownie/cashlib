#ifndef _MERKLECONTRACT_H_
#define _MERKLECONTRACT_H_

#include <bitset>
#include "Ciphertext.h"
#define CHUNK_SIZE 1024

typedef bitset<32> pathbits;

struct hashDirect{
	hash_t node;
	pathbits path;
};

typedef vector<vector<hashDirect> > hash_matrix;	

class MerkleContract {
	public:	
		MerkleContract(){}
		MerkleContract(const string key, hashalg_t alg): key(key), alg(alg) {}
		MerkleContract(const MerkleContract &o) : key(o.key), alg(o.alg) {}

		hash_t hash(hash_t left, hash_t right) const {
			return Hash::hash(left.str() + right.str(), alg, key, 
							  Hash::TYPE_PLAIN);
		}
	
		hash_t hash(const char * buff, int buffSize) const {
			return Hash::hash(buff, buffSize, alg, key, Hash::TYPE_PLAIN);
		}
	
		template <class T>
		hash_t hash(T* buff) const {
			return hash(buff->data(), buff->size());
		}
	
		hash_t hash(const string& str) const {
			return hash(str.data(), str.size());
		}
		
	private:
		string key;
		hashalg_t alg;
		
		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(key)
				& auto_nvp(alg);
		}
};

#endif
