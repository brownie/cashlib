#ifndef _HASH_H_
#define _HASH_H_

#include <string>
#include <vector>
#include "NTL/ZZ.h"
#include <openssl/evp.h>
#include "CashException.h"

using namespace std;
using NTL::ZZ;

class Buffer;
class EncBuffer;

class Hash {
	
	public:
		/* DEFINITIONS, STATIC CONSTANTS */
		// Hashing

		// there are only 3 hash algorithms
		enum alg_t { SHA1, SHA256, MD5, MAXALG };
		static const char *alg_names[];
		
		static const int TYPE_PLAIN = 0;
		static const int TYPE_MERKLE = 1;
		
		// HashVal: a Hash value, the output of a hash() function
		// -- contains its own alg, type, key
		struct HashVal {
			// construct as a tuple of hashValue, hashAlg, hashType, hashKey
			HashVal(const char *buf, size_t len, alg_t alg, int type, const string& key=string())
				: val(buf, len), alg(alg), type(type), key(key) {}
			HashVal(const string& val, alg_t alg, int type, const string& key=string())
				: val(val), alg(alg), type(type), key(key) {}
			// empty HashVal
			HashVal() : val(), alg(SHA1), type(TYPE_PLAIN), key() {}
#if 0
			// string-style constructors (XXX deprecated: doesn't store hash args)
			HashVal(const char *buf, size_t len) 
				: val(buf, len),  alg(SHA1), type(TYPE_PLAIN), key() {}
			HashVal(const string& s) 
				: val(s),  alg(SHA1), type(TYPE_PLAIN), key() {}
#endif
			string val; // stores binary representation -- may contain null bytes
			alg_t alg;
			int type;
			string key;
			operator string() const { return val; }
			string str() const { return val; }
			size_t size() const { return val.size(); }
			const char* c_str() const { return val.c_str(); }
			const char* data() const { return val.data(); }
			bool operator==(const HashVal& o) const {
				return val == o.val && alg == o.alg && type == o.type && key == o.key;
			}
			bool operator!=(const HashVal& o) const { 
				return val != o.val || alg != o.alg || type != o.type || key != o.key;
			}
			NTL::ZZ to_ZZ() const { 
				return NTL::ZZFromBytes((const unsigned char *)val.c_str(), val.size());
			}
			operator NTL::ZZ() const { return to_ZZ(); }
			
			bool empty() const {return val.empty();}

			friend class boost::serialization::access;
			template <class Archive> 
			void serialize(Archive& ar, const unsigned int ver) {
				ar & auto_nvp(val) & auto_nvp(alg) & auto_nvp(type) & auto_nvp(key);
			}
		};
		typedef HashVal hash_t;
	
		/* Helper Functions */
		static alg_t get_algbyname(const string &name);
    
		/* USE ONLY THESE TWO FOR STRING HASHING */
		/*! Perform regular hash if key is empty string, otherwise keyed hash */
		/*! Perform Plain or Merkle hash */
		static hash_t hash(const string &data, const alg_t alg, 
				const string &key, const int hashType);
		static hash_t hash(const char* data, size_t len, const alg_t alg, 
				const string &key, const int hashType);
	
		static hash_t merkleHash(const string& data, const alg_t alg, 
				const string &key, size_t &chunksz_out);
		static hash_t merkleHash(const char* data, size_t len, const alg_t alg, 
				const string &key, size_t &chunksz_out);
	
		static ZZ hash(const vector<ZZ> &vec, const alg_t alg);
		static ZZ hash(const ZZ &z, const alg_t alg, const string& key);
		/*****************************************/
	
		/* SPECIAL USE: ONLY IN Verifiable Encryption to handle Labels */
		static hash_t hash(const vector<ZZ> &vec, const string& label, 
				const alg_t alg, const string &key);
	
		/** 
		* split a block of data into chunks, and return a vector of chunk hashes
		* @param chunksz if 0, a chunk size will be chosen and returned
		* @param out vector of hashes
		*/
		static vector<hash_t> hashChunks(const string& data, const alg_t alg, 
				const string& key, size_t& chunksz_out);
		static vector<hash_t> hashChunks(const char *buf, size_t len, 
				const alg_t alg, const string& key, size_t& chunksz_out);

		/* Used by FEContract for contract-checking */
		static hash_t hash(const vector<const Buffer*>& buf, const alg_t& alg,
						   const string &key, const int hashType);
		static hash_t hash(const vector<Buffer*>& buf, const alg_t& alg,
						   const string &key, const int hashType);
		static hash_t hash(const vector<const EncBuffer*>& buf, const alg_t& alg,
						   const string &key, const int hashType);
		static hash_t hash(const vector<EncBuffer*>& buf, const alg_t& alg,
						   const string &key, const int hashType);
		static hash_t hash(const vector<hash_t>& hashes, const alg_t& alg,
						   const string &key, const int hashType);
	protected:
		// Plain hash functions on char*
		static hash_t hash(const char* data, size_t len, 
				const alg_t alg, const string &key);
		static hash_t hash(const char *buf, int len, const alg_t alg);
	
		// Plain keyed-hash functions on char*
		static hash_t hmac(const char* data, size_t len, const alg_t alg, 
				const string &key);
		
		// Merkle hash helpers
		static hash_t partialMHash(const vector<hash_t>& chunks, 
				const unsigned leaf, const unsigned hieght, const alg_t alg);
	
		// inline helper function
		static inline const EVP_MD* get_MD(const alg_t alg) {
			switch (alg) {
				case Hash::SHA1:
					return EVP_sha1();
				case Hash::SHA256:
					return EVP_sha256();
				case Hash::MD5:
					return EVP_md5();
				default:
					throw CashException(CashException::CE_UNKNOWN_ERROR,
                                	"[Hash::get_MD] Can't find digest %d", alg);
        		}
    	}
};

typedef Hash::hash_t hash_t;
typedef Hash::alg_t hashalg_t;

namespace __gnu_cxx {
	// XXX make this more efficient
	inline size_t hash_string(const char* __s, size_t len) {
		unsigned long __h = 0;
		for ( ; len; ++__s, --len)
			__h = 5 * __h + *__s;
		return size_t(__h);
	}

	template <>	struct hash<Hash::HashVal> {
		size_t operator() (const Hash::HashVal& x) const {
			return hash_string(x.val.data(), x.val.size());
		}
	};
}

#endif
