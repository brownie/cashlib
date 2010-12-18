#include <openssl/evp.h>
#include <openssl/rand.h>
#include <assert.h>
#include <string.h>
#include "Signature.h"
#include "CashException.h"

const Signature::engine_t Signature::DEFAULT_ENGINE = ENGINE_get_default_DSA();

// precompute the DSA parameters once per cashlib, as the openssl command line can do: 
// http://www.madboa.com/geek/openssl/#key-dsa (XXX this is secure, right?)
static const DSA *DSA_PARAMS = DSA_generate_parameters(SIG_NUMBITS,NULL,0,NULL,NULL,NULL,NULL);
extern "C" DSA *DSAparams_dup_wrap(const DSA * dsa);

string Signature::sign(const Key& key, const string& data,
                       const hashalg_t& alg, const engine_t& engine) 
{    
    EVP_MD_CTX ctx;
    const EVP_MD *m;
    unsigned int len;
    if (!key.isPrivate)
        throw CashException(CashException::CE_UNKNOWN_ERROR,
                            "[Signature::sign] can't sign with public key");
    assert(key._key);
    if (EVP_PKEY_type(key._key->type) == EVP_PKEY_DSA)
        m = EVP_dss1(); // SHA1 for DSA
    else if (EVP_PKEY_type(key._key->type) == EVP_PKEY_RSA)
        m = EVP_sha1();
    else
        throw CashException(CashException::CE_OPENSSL_ERROR,
                            "[Signature::sign] can't find digest");

    EVP_MD_CTX_init(&ctx); // really only needed for EVP_SignInit_ex
    EVP_SignInit(&ctx, m);
    if (!EVP_SignUpdate(&ctx, data.data(), data.size())) 
        throw CashException(CashException::CE_OPENSSL_ERROR,
                            "[Signature::sign] Error in EVP_SignUpdate");
    
    unsigned char sig[EVP_PKEY_size(key._key)];
    if (!EVP_SignFinal(&ctx, sig, &len, key._key)) 
        throw CashException(CashException::CE_OPENSSL_ERROR,
                            "[Signature::sign] Error in EVP_SignFinal");
    EVP_MD_CTX_cleanup(&ctx);
    return string((char *)sig, len);
}

bool Signature::verify(const Key& key, const string& sig, const string &data, 
                       const hashalg_t& alg, const engine_t& engine) 
{
    EVP_MD_CTX ctx;
    const EVP_MD *m;
    assert(key._key);
    if (EVP_PKEY_type(key._key->type) == EVP_PKEY_DSA)
        m = EVP_dss1(); // SHA1 for DSA
    else if (EVP_PKEY_type(key._key->type) == EVP_PKEY_RSA)
        m = EVP_sha1();
    else
        throw CashException(CashException::CE_OPENSSL_ERROR,
                            "[Signature::verify] can't find digest");

    EVP_MD_CTX_init(&ctx);
    EVP_VerifyInit(&ctx, m);
    if (!EVP_VerifyUpdate(&ctx, data.data(), data.size()))
        throw CashException(CashException::CE_OPENSSL_ERROR,
                            "[Signature::sign] Error in EVP_VerifyUpdate");
    int err = EVP_VerifyFinal(&ctx, (const unsigned char*)sig.data(), 
                              sig.size(), key._key);
//    if (err < 0)
//        throw CashException(CashException::CE_OPENSSL_ERROR,
//                            "[Signature::sign] Error in EVP_VerifyFinal");
    EVP_MD_CTX_cleanup(&ctx);

    return (err == 1); // signature OK
}

Ptr<Signature::Key> Signature::Key::generateDSAKey(int numbits) {
	DSA *dsa;
    if (numbits == SIG_NUMBITS)
		dsa = DSAparams_dup_wrap(DSA_PARAMS);
	else 
		dsa = DSA_generate_parameters(numbits,NULL,0,NULL,NULL,NULL,NULL);
    if (!dsa)
        throw CashException(CashException::CE_OPENSSL_ERROR,
                            "[Signature::Key::generateDSAKey] no DSA params");
	if (!DSA_generate_key(dsa))
		throw CashException(CashException::CE_OPENSSL_ERROR,
                            "[Signature::Key::generateDSAKey] DSA gen key");
    EVP_PKEY *k = EVP_PKEY_new();
    EVP_PKEY_assign_DSA(k, dsa); // dsa now owned by k: don't need to free it
    return new Key(k, true);
}

Ptr<Signature::Key> Signature::Key::generateRSAKey(int modlen) {
    RSA *rsa = NULL;
    rsa = RSA_generate_key(modlen, RSA_F4, NULL, NULL);
    if (!RSA_check_key(rsa))
        throw CashException(CashException::CE_OPENSSL_ERROR,
                            "[Signature::Key::generateRSAKey] RSA gen params");
    EVP_PKEY *k = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(k, rsa); // rsa now owned by k: don't need to free it
    return new Key(k, true);
}

void BIO_free_to_string(BIO *bio, string &out) {
    assert(bio);
    char *d;
    size_t len = BIO_get_mem_data(bio, &d);
    out.assign(d, len);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free(bio);
}
inline string BIO_free_to_string(BIO *bio) { string s; BIO_free_to_string(bio, s); return s; }

string Signature::Key::toPEM(bool save_private_key) const {
    BIO *bio = BIO_new(BIO_s_mem());
    int e;
    if (save_private_key && isPrivate) // XXX could encrypt private key
        e = PEM_write_bio_PrivateKey(bio, _key, NULL, NULL, 0, NULL, NULL);
    else 
        e = PEM_write_bio_PUBKEY(bio, _key);
    if (!e) {
        BIO_set_close(bio, BIO_NOCLOSE);
        BIO_free(bio);
        throw CashException(CashException::CE_OPENSSL_ERROR,
                            "[Signature::Key::publicKeyString] can't write BIO");
    }
    return BIO_free_to_string(bio);
}

string Signature::Key::toDER(bool save_private_key) const {
    unsigned char *buf, *next;
    int len;
    if (save_private_key && isPrivate) {
        buf = next = (unsigned char *)malloc(i2d_PrivateKey(_key, NULL));
        len = i2d_PrivateKey(_key, &next);
    } else {
        buf = next = (unsigned char *)malloc(i2d_PUBKEY(_key, NULL));
        len = i2d_PUBKEY(_key, &next);
    }
    string ret((char *)buf, len);
    free(buf);
    return ret;
}

Ptr<Signature::Key> Signature::Key::fromDER(const string& str, bool isPrivate) {
    EVP_PKEY *k;
    const unsigned char *buf = (const unsigned char*)str.data();
    if (isPrivate) {
        k = d2i_AutoPrivateKey(NULL, &buf, str.size());
    } else {
        k = d2i_PUBKEY(NULL, &buf, str.size());
    }
    if (!k)
         throw CashException(CashException::CE_OPENSSL_ERROR,
                             "[Signature::Key::fromDER] can't load d2i");
    return new Key(k, isPrivate);
}

Signature::Key::Key(const string& file, const string& passwd, bool isPrivate)
    : isPrivate(isPrivate) {
    FILE* keyf = fopen(file.c_str(), "r");
    if (!keyf) throw CashException(CashException::CE_IO_ERROR,
                                   "[Signature::Key::Key] can't open key file %s", 
                                   file.c_str());
    if (isPrivate)
        _key = PEM_read_PrivateKey(keyf, NULL, 0, (void*)passwd.c_str());
    else
        _key = PEM_read_PUBKEY(keyf, NULL, 0, (void*)passwd.c_str());
    fclose(keyf);
    if (!_key) throw CashException(CashException::CE_OPENSSL_ERROR,
                                   "[Signature::Key::Key] can't read key file %s", 
                                   file.c_str());
}

Signature::Key::Key(const Signature::Key& original) 
  : isPrivate(original.isPrivate) {
    assert(original._key);
    _key = EVP_PKEY_new();
    if (EVP_PKEY_type(original._key->type) == EVP_PKEY_DSA) {
        DSA *dsa = EVP_PKEY_get1_DSA(original._key);
        EVP_PKEY_set1_DSA(_key, dsa);
        DSA_free(dsa);
    } else if (EVP_PKEY_type(original._key->type) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(original._key);
        EVP_PKEY_set1_RSA(_key, rsa);
        RSA_free(rsa);
    }
}
