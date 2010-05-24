#include "SigmaProver.h"

SigmaProof SigmaProver::getSigmaProof(const hashalg_t &hashAlg) {
	SigmaProof result(randomizedProofs(), getCommitments(), hashAlg);
	var_map hm = respond(result.computeChallenge());
	result.setResponse(hm);

	return result;
}
