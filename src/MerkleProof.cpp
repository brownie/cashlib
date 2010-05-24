
#include "MerkleProof.h"

MerkleProof::MerkleProof(const vector<EncBuffer*> &ctextBlocks, 
						 const hash_matrix &ctextProof,
                         const hash_matrix &fileProof, 
						 MerkleContract* ctContract,
						 MerkleContract* ptContract) 
  	: ctextBlocks(ctextBlocks), ctextProof(ctextProof), fileProof(fileProof),
	  ctContract(ctContract), ptContract(ptContract) 
{
	plaintext = "";
	empty=false;
}

MerkleProof::MerkleProof(const vector<EncBuffer*> &ctextBlocks, 
						 const hash_matrix &ctextProof,
                         const string &plaintext, MerkleContract* ctContract) 
  	: ctextBlocks(ctextBlocks), ctextProof(ctextProof), plaintext(plaintext), 
	  ctContract(ctContract) {empty=false;}

MerkleProof::MerkleProof(const vector<EncBuffer*> &ctextBlocks, 
						 const hash_matrix &ctextProof,
						 MerkleContract* ctContract) 
	: ctextBlocks(ctextBlocks), ctextProof(ctextProof), ctContract(ctContract)
{
}

MerkleProof::MerkleProof(const MerkleProof &o) 
	: ctextBlocks(o.ctextBlocks), ctextProof(o.ctextProof), 
	  fileProof(o.fileProof), plaintext(o.plaintext), ctContract(o.ctContract),
	  ptContract(o.ptContract), empty(o.empty) 
{
}
