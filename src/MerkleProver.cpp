#include "MerkleProver.h"

MerkleProver::MerkleProver(const vector<hash_t> &hashBlocks, 
						   const MerkleContract &contract) 
	: contract(contract)
{
	tree = new MerkleTree(hashBlocks, contract);
}

MerkleProver::MerkleProver(const char* buff, int buffSize, 
						   const MerkleContract& contract) 
	: contract(contract), tree(0) 
{
	init(buff, buffSize);
}


MerkleProver::MerkleProver(const string &str, const MerkleContract &contract) 
	: contract(contract)
{
	init(str.data(), str.size());
}

MerkleProver::MerkleProver(const vector<Ptr<EncBuffer> > &encBuffs, 
						   const MerkleContract &contract) 
	: contract(contract)
{
	vector<hash_t> hashBlocks(encBuffs.size());
	for(unsigned x=0; x<encBuffs.size(); x++){
		hashBlocks[x] = contract.hash(encBuffs[x]);
	}
	tree = new MerkleTree(hashBlocks, contract);
}

MerkleProver::MerkleProver(const vector<Ptr<const Buffer> >& buffs, 
						   const MerkleContract &contract) 
	: contract(contract)
{
	vector<hash_t> hashBlocks(buffs.size());
	for(unsigned x=0; x<buffs.size(); x++){
		hashBlocks[x] = contract.hash(buffs[x]);
	}
	tree = new MerkleTree(hashBlocks, contract);
}
hash_matrix MerkleProver::generateProofs(const vector<unsigned> &challenges){
	hash_matrix toReturn;
	unsigned treeSize = 2*getNumBlocks()-1;
	vector<hash_t> m_tree(treeSize);
	//initialize the leaves to chunks
	vector<hash_t> leaves = tree->getLeaves();
	int x;
	int y;
	for(x = treeSize -1, y = getNumBlocks()-1; y>=0; x--, y--){
		m_tree[x]=leaves[y];
	}
	
	for(unsigned x = 0; x<challenges.size(); x++){
		toReturn.push_back(generateProof(challenges[x], m_tree));
	}
	return toReturn;
}

vector<hashDirect> MerkleProver::generateProof(unsigned challenge, 
											   vector<hash_t> &m_tree){
	vector<hashDirect> toReturn;
	
	//construct a binary represention of the chunk index
	pathbits path = binaryRepresentation(challenge);

	//push back the chunk itself
	hashDirect chunk;
	chunk.node=m_tree[getNumBlocks() - 1 + challenge];
	chunk.path = path;
	toReturn.push_back(chunk);
	
	//should get all the neighbors
	for(unsigned height = tree->getHeight(); height>=1; height--){
		hashDirect temp;
		path.flip(0);
		unsigned index = (1<<height)- 1 + binToInt(path);
		temp.node = computeSubTree(index, m_tree);
		temp.path = path;
		//get the neighbor
		toReturn.push_back(temp);
		path >>= 1;
	}
	return toReturn;
}

hash_t MerkleProver::computeSubTree(unsigned index, vector<hash_t> &m_tree){
	if(m_tree[index].empty()){
		m_tree[index] = contract.hash(computeSubTree(index*2 +1, m_tree), 
									  computeSubTree(index*2+2, m_tree));
	}
	return m_tree[index];
}


void MerkleProver::init(const char* buff, int buffSize){
	tree = new MerkleTree(buff, buffSize, contract);
}



