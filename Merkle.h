#ifndef _MERKLE_H_
#define _MERKLE_H_

#include "MerkleContract.h"

//constructs a binary representation
inline pathbits binaryRepresentation(int x){
	pathbits ret(x);
	return ret;
}

//needed for bit flipping 
inline unsigned binToInt(pathbits bin){
	return bin.to_ulong();
}

inline bool powerOfTwo(int x){
	return !(x & (x - 1)) && x;
}

// from http://www-graphics.stanford.edu/~seander/bithacks.html	
inline unsigned log_2(int x){
	union { unsigned int u[2]; double d; } t; // temp
	t.u[BYTE_ORDER==LITTLE_ENDIAN] = 0x43300000;
	t.u[BYTE_ORDER!=LITTLE_ENDIAN] = x;
	t.d -= 4503599627370496.0;
	return (t.u[BYTE_ORDER==LITTLE_ENDIAN] >> 20) - 0x3FF;
}

inline unsigned nextPowerOfTwo(int x){
	if(powerOfTwo(x)){
		return x;
	} else{
		return 1<<(log_2(x)+1);
	}
}

class MerkleTree {

	public:
		MerkleTree(const char* buff, int buffSize, 
				   const MerkleContract &contract);
		MerkleTree(const string& str, const MerkleContract &contract);
		MerkleTree(const vector<hash_t> &hashBlocks, 
				   const MerkleContract &contract);
		
		unsigned getHeight() const {return height;}
		vector<hash_t> getLeaves() const {return leaves;}
		hash_t getRoot() const {return root;}
		unsigned getNumLeaves() const {return 1 << height;}

	private:		
		vector<hash_t> initHashChunks(const char* buff, int buffSize);
		hash_t makeRoot(const vector<hash_t> &chunks);
		void init(const char* buff, int buffSize);
		void init(const vector<hash_t> &hashBlocks);
		vector<hash_t> pad(const vector<hash_t> &hashBlocks);
		
		MerkleContract contract;
		hash_t root;
		vector<hash_t> leaves;
		unsigned padStart;
		unsigned height;
};

#endif
