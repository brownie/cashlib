#include "Merkle.h"

MerkleTree::MerkleTree(const char* buff, int buffSize, 
					   const MerkleContract &contract) 
	: contract(contract)
{
	init(buff, buffSize);
}

MerkleTree::MerkleTree(const string &str, const MerkleContract &contract) 
	: contract(contract)
{
	init(str.data(), str.size());
}


MerkleTree::MerkleTree(const vector<hash_t> &hashBlocks, 
					   const MerkleContract &contract) 
	: contract(contract)
{
	init(hashBlocks);
}

vector<hash_t> MerkleTree::initHashChunks(const char* buff, int buffSize){	
	unsigned numChunks = buffSize/CHUNK_SIZE;
	padStart = numChunks;

	unsigned nextTwoPow = nextPowerOfTwo(numChunks);
	height = log_2(nextTwoPow);

	//create a hash_t array of the leaf nodes
	vector<hash_t> hashChunks(nextTwoPow);

	for(unsigned i = 0; i<nextTwoPow; i++){
		if(i<numChunks){
			hashChunks[i] = contract.hash(buff+i*CHUNK_SIZE , ((i==numChunks-1 && buffSize %CHUNK_SIZE) ? buffSize % CHUNK_SIZE  : CHUNK_SIZE ));
		}else {
			hashChunks[i] = contract.hash((char *)&i, sizeof(i));
		}
	}
	//return a pointer to the leaf nodes array
	return hashChunks;
}

vector<hash_t> MerkleTree::pad(const vector<hash_t> &hashBlocks){
	unsigned numChunks = hashBlocks.size();
	padStart = numChunks;
	
	unsigned nextTwoPow = nextPowerOfTwo(numChunks);
	height = log_2(nextTwoPow);

	//create a hash_t array of the leaf nodes
	vector<hash_t> hashChunks(nextTwoPow);

	for(unsigned i = 0; i<nextTwoPow; i++){
		if(i<numChunks){
			hashChunks[i] = hashBlocks[i];
		}else {
			hashChunks[i] = contract.hash((char *)&i, sizeof(i));
		}
	}
	//return a pointer to the leaf nodes array
	return hashChunks;
	
	
}
hash_t MerkleTree::makeRoot(const vector<hash_t> &chunks){
	//a binary tree has 2 * numLeaves -1 nodes
	unsigned treeSize = 2*getNumLeaves()-1;
	//create an array to hold the binary tree
	vector<hash_t> m_tree(treeSize);
	//initialize the leaves to chunks
	int x;
	int y;
	for(x = treeSize -1, y = getNumLeaves()-1; y>=0; x--, y--){
		m_tree[x]=chunks[y];
	}
	//compute the rest of the hash tree
	for(int x = treeSize-1-getNumLeaves(); x>=0; x--){
		m_tree[x] = contract.hash(m_tree[2*x+1], m_tree[2*x+2]);
	}
	//return the root
	return m_tree[0];
}

void MerkleTree::init(const char* buff, int buffSize){
	leaves = initHashChunks(buff, buffSize);
	root = makeRoot(leaves);
	root.type=Hash::TYPE_MERKLE;
}

void MerkleTree::init(const vector<hash_t> &hashBlocks){
	leaves = pad(hashBlocks);
	root = makeRoot(leaves);
	root.type=Hash::TYPE_MERKLE;
}


