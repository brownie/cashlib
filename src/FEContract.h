
#ifndef _FECONTRACT_H_
#define _FECONTRACT_H_

#include "Ciphertext.h"

/*! \brief This class is a parent class for BuyContract and BarterContract and 
 * is used to store information about participator's hashes */

class FEContract {
	
	public:
		/*! contract stores timeout, session ID, plaintext and ciphertext 
		 * hashes for responder, and all hashAlg/hashKey/hashType 
		 * information for these hashes */
		FEContract(const int t, const ZZ& i) : timeout(t), id(i) {}

		/*! copy constructor */
		FEContract(const FEContract &o)
			: timeout(o.timeout), id(o.id), encAlgA(o.encAlgA), 
			  ptHashA(o.ptHashA), ctHashA(o.ctHashA), encAlgB(o.encAlgB),
			  ptHashB(o.ptHashB), ctHashB(o.ctHashB), 
			  ptHashBlocksB(o.ptHashBlocksB), ctHashBlocksB(o.ctHashBlocksB) {}
		
		/*! serialization constructor */
        //FEContract(const string& str) { loadGZString(*this, str); }
		
		FEContract() {}

		/*! destructor */
		~FEContract() {}
		
		// add more files to the contract
		void setEncAlgA(const cipher_t& encAlg) {encAlgA = encAlg;}
		void setEncAlgB(const cipher_t& encAlg) {encAlgB = encAlg;}
		void setPTHashA(const hash_t& hash) {ptHashA = hash;}
		void setCTHashA(const hash_t& hash) {ctHashA = hash;}
		void setPTHashB(const hash_t& hash) {ptHashB = hash;}
		void setCTHashB(const hash_t& hash) {ctHashB = hash;}
		void setPTHashBlocksB(unsigned ptNumBlocksResp)
				{ptHashBlocksB = ptNumBlocksResp;}
		void setCTHashBlocksB(unsigned ctNumBlocksResp)
				{ctHashBlocksB = ctNumBlocksResp;}
		
		/*! check if values stored in contract match those in input */
		bool checkTimeout(const int timeoutTolerance) const;
		bool checkEncAlgB(const cipher_t& encAlgR) const;
		
		bool checkAFiles(const vector<Ptr<const Buffer> >& ptext, 
						 const vector<Ptr</*const*/ EncBuffer> >& ctext) const;
		bool checkBFiles(const vector<Ptr<const Buffer> >& ptext, 
						 const vector<Ptr</*const*/ EncBuffer> >& ctext) const;
		bool checkAFile(Ptr<const Buffer> ptext, Ptr</*const*/ EncBuffer> ctext) const;
		bool checkBFile(Ptr<const Buffer> ptext, Ptr</*const*/ EncBuffer> ctext) const;
		
		bool checkAHash(const hash_t& ptHash, const hash_t& ctHash) const;
		bool checkBHash(const hash_t& ptHash, const hash_t& ctHash) const;
		
		// getters
		const ZZ& getID() const { return id; }
		long getTimeout() const { return timeout; }
		unsigned getNumPTHashBlocksB(){return ptHashBlocksB;}
		unsigned getNumCTHashBlocksB(){return ctHashBlocksB;}
		const hash_t& getPTHashA() const { return ptHashA; }
		const hash_t& getPTHashB() const { return ptHashB; }
		const hash_t& getCTHashA() const { return ctHashA; }
		const hash_t& getCTHashB() const { return ctHashB; }
		const cipher_t& getEncAlgA() const { return encAlgA; }
		const cipher_t& getEncAlgB() const { return encAlgB; }
		
	protected:
		bool checkHashes(const vector<Ptr<const Buffer> >& ptext, 
						 const vector<Ptr</*const*/ EncBuffer> >& ctext, 
						 const hash_t& ptHash, const hash_t& ctHash) const;
		bool checkHash(const hash_t& hashGiven, const hash_t& hashStored) const;
		
	private:
		int timeout;
		ZZ id;

		cipher_t encAlgA;
		hash_t ptHashA;
		hash_t ctHashA;
		
		cipher_t encAlgB;
		hash_t ptHashB;
		hash_t ctHashB;
		unsigned ptHashBlocksB;
		unsigned ctHashBlocksB;
		
		// also need to add:
		// - Arbiter public key? (implicit in ctHashKey?)
		// - application-specific details? (BT piece id, byte range)

		friend class boost::serialization::access;
		template <class Archive> 
		void serialize(Archive& ar, const unsigned int ver) {
			ar  & auto_nvp(timeout)
				& auto_nvp(id)
				& auto_nvp(encAlgA)
				& auto_nvp(ptHashA)
				& auto_nvp(ctHashA)
				& auto_nvp(encAlgB)
				& auto_nvp(ptHashB)
				& auto_nvp(ctHashB)
				& auto_nvp(ptHashBlocksB)
				& auto_nvp(ctHashBlocksB);
		}
};

#endif /*_FECONTRACT_H_*/
