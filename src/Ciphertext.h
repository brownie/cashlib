#ifndef _CIPHERTEXT_H_
#define _CIPHERTEXT_H_

#include <string>
#include "NTL/ZZ.h"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include "Hash.h"
#include <boost/function.hpp>

using namespace std;
using NTL::ZZ;

class Ciphertext {

	public:
		// encrypting and decrypting
		typedef string cipher_t;
		static const cipher_t AES_128_CBC;
		static const cipher_t AES_128_ECB;
		static const cipher_t AES_128_CTR;
		static const cipher_t AES_192_CTR;
		static const cipher_t AES_256_CTR;
		static bool _is_counter_cipher(const Ciphertext::cipher_t& c) {
			return (c == AES_128_CTR || c == AES_192_CTR || c == AES_256_CTR);
    		}

		/** Encrypt <b>fromlen</b> bytes from <b>from</b> with the
		* symmetric key <b>key</b> to <b>to</b>. The length of <b>to</b>
		* should be encryptedLength(fromlen) bytes.  On success, return the
		* number of bytes written, on failure, throw exception.
		*/
		static int encrypt(const char *key, char *to, const char *from, 
				size_t fromlen, const cipher_t& cipher);
		static char* encrypt(const char *key, const char *from, size_t fromlen, 
				const cipher_t& cipher, size_t* outlen);
		static int encrypt(const string& key, string& to, 
				const string& from, const cipher_t& cipher=AES_128_CBC);
		// Return the maximum number of bytes required for encrypting len bytes 
		// with cipher
		static size_t encryptedLength(size_t len, const cipher_t& cipher);

		/** Decrypt <b>fromlen</b> bytes from <b>from</b> with the
		* symmetric key <b>key</b> to <b>to</b>. The length of <b>to</b>
		* should be at most <b>fromlen</b> bytes. On success, return the
		* number of bytes written, on failure, throw exception.
		*/
		static int decrypt(const char *key, char *to, 
				const char *from, size_t fromlen, const cipher_t& cipher, 
				size_t offset=0);
		static char* decrypt(const char *key, const char *from, size_t fromlen, 
				const cipher_t& cipher, size_t *outlen);
		static int decrypt(const string &key, string &to, 
				const string &from, const cipher_t& cipher=AES_128_CBC);
		static int decrypt(const string &key, string &to, 
				const string &from, size_t offset, 
				const cipher_t& cipher=AES_128_CBC);

		static string generateKey(const cipher_t& cipher=AES_128_CBC);
		static size_t keyLength(const cipher_t& cipher);

		// generate and set key + IV for AES counter mode
		// assumes R is hash of shared secret
		static string generateKey(const cipher_t& cipher, const ZZ& r); 
		static string makeKey(const cipher_t& cipher, const string& key, 
				const ZZ& r);

		/** AES counter mode: encrypt/decrypt are the same function (uses XOR)
		* - encrypt/decrypt whole file, given key, IV, data
		* - decrypt/encrypt specific range from file, given key, IV, offset, data
		*   - automatically convert offset to counter (offset/blksize)
		* @param key     AES key (128, 192, or 256)
		* @param keylen  key length in bytes: 16, 24, or 32
		* @param iv      128-bit IV (will be combined with counter)
		* @param offset  set initial value of counter corresponding to this byte offset
		*/
		static void AES_counter_crypt(const unsigned char *in, unsigned char *out,
					size_t length, const unsigned char *key, 
					size_t keylen, const unsigned char *iv,
					size_t offset=0);
};

    class EncBuffer;
    class Buffer {
    public:
        char *buf;
        size_t len;
        bool isEncrypted;
        bool isHashed; // has hashVal been computed?
		typedef boost::function<void(char*)> destroyFnType;
        destroyFnType destroyFn; // frees the buffer        
//        void (*destroyFn)(void*); // NULL if freeing buf unnecessary

        hash_t    hashVal;
		
		Buffer() : buf(0), len(0), destroyFn(destroyFnType()) {}
        Buffer(char* d, size_t l, bool isEnc=false, void (*DestroyFn)(void *)=NULL) 
            : buf(d), len(l), isEncrypted(isEnc), isHashed(false), 
			  destroyFn(DestroyFn ? DestroyFn : destroyFnType()) {}
        template<class Destructor>
        Buffer(char* d, size_t l, bool isEnc=false, Destructor DestroyFn=destroyFnType()) 
            : buf(d), len(l), isEncrypted(isEnc), isHashed(false), destroyFn(DestroyFn) {}
        Buffer(const string& s, bool isEnc=false)
            : buf(0), isEncrypted(isEnc), isHashed(false) { copyString(s); }
        virtual ~Buffer() { if (buf && destroyFn) destroyFn(buf); }
        inline const char* data() const { return buf; }
        inline size_t size() const { return len; }
        inline string str() const { return string(buf, len); }
        operator string() const { return string(buf, len); }
        Buffer& operator=(const string& s) { copyString(s); return *this; }

        // encrypt a Buffer: returns new Ptr<EncBuffer> containing key, alg
        //Ptr<EncBuffer> encrypt(const string& key, const Ciphertext::cipher_t& alg) const;
        Ptr<EncBuffer> encrypt(const Ciphertext::cipher_t& alg, const string& key = string()) const;/* {
            return encrypt((NULL != key) ? key : Ciphertext::generateKey(alg), alg);
        }*/
        Ptr<EncBuffer> encrypt(const ZZ& r, const Ciphertext::cipher_t& alg) const {
            return encrypt(alg, Ciphertext::generateKey(alg, r));
        }
        // decrypt a Buffer: returns new Ptr<Buffer> 
        virtual Ptr<Buffer> decrypt(const string& key, const Ciphertext::cipher_t& alg) const {
            size_t ptl;
            char *pt = Ciphertext::decrypt(key.data(), buf, len, alg, &ptl);
			return new_ptr<Buffer>(pt, ptl, false);
        }
        // hash a Buffer: caches hash computation
        hash_t hash(const hashalg_t& halg, const string& hkey, int htype) {
            setHash(Hash::hash(buf, len, halg, hkey, htype));
            return hashVal;
        }
        // const version of hash (doesn't save hash)
        hash_t hash(const hashalg_t& halg, const string& hkey, int htype) const {
            return Hash::hash(buf, len, halg, hkey, htype); 
        }
        // set precomputed hash, e.g. from external application or published BT info
        void setHash(const hash_t& h) { hashVal = h; }
        // check a Buffer against a hash (and parameters) published in
        // a contract or torrent file
        bool checkHash(const hash_t& h) {
            // check for precomputed hashVal, otherwise compute new hashVal
            if (!(isHashed && h.alg == hashVal.alg && h.key == hashVal.key && h.type == hashVal.type))
                hash(h.alg, h.key, h.type);
            return (h == hashVal);
        }
        // const version of checkHash (doesn't save hash in Buffer)
        bool checkHash(const hash_t& h) const {
            if (isHashed && h.alg == hashVal.alg && h.key == hashVal.key && h.type == hashVal.type) {
                return (hashVal.val == h.val);
            } else {
                return (h == hash(h.alg, h.key, h.type));
            }
        }
        // clear buffer
        virtual void clear() { 
			if (buf && destroyFn) { 
				destroyFn(buf); destroyFn=destroyFnType(); buf=0; len=0; 
				isHashed=false; isEncrypted=false; hashVal=hash_t();
			} 
		}
        void copyString(const string& s) {
            if (buf && destroyFn) destroyFn(buf);
            buf = (char *)malloc(s.size());
            memcpy(buf, s.data(), s.size());
            len = s.size();
            destroyFn = std::free;
        }
    };
    class EncBuffer: public Buffer {
    public:
        string key;
        Ciphertext::cipher_t encAlg;
        EncBuffer(char* d, size_t l, const string& k, const Ciphertext::cipher_t& a)
            : Buffer(d, l, true), key(k), encAlg(a) {}
        EncBuffer(char* d, size_t l) : Buffer(d, l, true), key(), encAlg() {}

        Ptr<Buffer> decrypt(const string& key, const Ciphertext::cipher_t& alg) const {
            return Buffer::decrypt(key, alg);
        }
        Ptr<Buffer> decrypt() const {
            return Buffer::decrypt(key, encAlg);
        }
		void clear() { Buffer::clear(); key = string(); }

		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int ver) {
			ar	& auto_nvp(key)
				& auto_nvp(encAlg)
				& auto_nvp(*buf) 
				& auto_nvp(len)
				;
		}
    };

typedef Ciphertext::cipher_t cipher_t;

#endif
