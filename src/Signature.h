#ifndef _SIGNATURE_H_
#define _SIGNATURE_H_

#include <string>
#include "NTL/ZZ.h"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/engine.h>
#include "Hash.h"
#include "CashException.h"
#include "Serialize.h"

#define SIG_NUMBITS 1024

class Signature {
public:

    class Key {
    public:
        Key(EVP_PKEY* key, bool isPrivate) : _key(key), isPrivate(isPrivate) {}
        /// load from file
        Key(const string& file, const string& passwd, bool isPrivate=true);
        Key(const Signature::Key& original);
		Key(const string& str, const bool isPrivate = false) {
			// XXX enforce isPrivate
			loadString(*this, str);
		}
		Key() : _key(0), isPrivate(false) {}

        /// private key: can sign & verify
        static Ptr<Key> loadPrivateKey(const string& file, const string& passwd) {
            return Ptr<Key>(new Key(file, passwd, true));
        }
        /// public key: can only verify
        static Ptr<Key> loadPublicKey(const string& file, const string& passwd=string()) {
            return Ptr<Key>(new Key(file, passwd, false));
        }
        // generates public/private keypair
        static Ptr<Key> generateDSAKey(int numbits=SIG_NUMBITS);
        static Ptr<Key> generateRSAKey(int modlen=SIG_NUMBITS);
        inline static Ptr<Key> generateKey(const string& alg) {
            if      (alg == "DSA") return generateDSAKey();
            else if (alg == "RSA") return generateRSAKey();
            else throw CashException(CashException::CE_UNKNOWN_ERROR,
                                     "[Signature::Key::generateKey] no alg %s", 
                                     alg.c_str());
        }
		
		// get public key for this key: allocates & returns new Key
		Ptr<Key> getPublicKey() const {
			if (!isPrivate){
				return Ptr<Key>(new Key(*this));
			} else { // XXX convert to public key more efficiently?
				string d = toDER(false);
				return fromDER(d, false);
			}
		}
		
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive>
		void save(Archive & ar, const unsigned int version) const {
			ar & isPrivate;
			string der = toDER(isPrivate); // XXX CCE always saves private key
			ar & der;
		}
		template<class Archive>
		void load(Archive & ar, const unsigned int version) {
			ar & isPrivate;
			string d;
			ar & d;
			const unsigned char *buf = (const unsigned char *)d.data();
			if (isPrivate)
				_key = d2i_AutoPrivateKey(NULL, &buf, d.size());
			else
				_key = d2i_PUBKEY(NULL, &buf, d.size());
			assert(_key);
		}

        // to & from strings
        //
        // PEM strings (starts with -----BEGIN PUBLIC KEY-----)
        string toPEM(bool save_private_key=false) const;
        inline string publicKeyString() const { return toPEM(false); }
        inline string privateKeyString() const { return toPEM(true); }
        // binary DER encoding
        string toDER(bool save_private_key=false) const;
        static Ptr<Key> fromDER(const string& buf, bool isPrivate=false);

        ~Key() { EVP_PKEY_free(_key); }
        EVP_PKEY* _key;
        bool isPrivate;
    private:
        Key& operator=(const Key& k) { return *this; } // disallow k1 = k2
    };

    // signing and verifying
    typedef ENGINE* engine_t;
    static const engine_t DEFAULT_ENGINE;
    
    static string sign(const Key& key, const string& data,
                       const hashalg_t& alg=Hash::SHA1, const engine_t& engine=DEFAULT_ENGINE);
    
    static bool verify(const Key& key, const string& sign, const string &data, 
                       const hashalg_t& alg=Hash::SHA1, const engine_t& engine=DEFAULT_ENGINE);
    
};

#endif
