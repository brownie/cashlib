#include <openssl/evp.h>
#include <openssl/rand.h>
#include <assert.h>
#include <string.h>
#include "Ciphertext.h"
#include "CashException.h"
#include "CommonFunctions.h"


const Ciphertext::cipher_t Ciphertext::AES_128_CBC = "aes-128-cbc";
const Ciphertext::cipher_t Ciphertext::AES_128_ECB = "aes-128-ecb";
const Ciphertext::cipher_t Ciphertext::AES_128_CTR = "aes-128-ctr";
const Ciphertext::cipher_t Ciphertext::AES_192_CTR = "aes-192-ctr";
const Ciphertext::cipher_t Ciphertext::AES_256_CTR = "aes-256-ctr";

namespace {
    struct _init_ciphers {
        _init_ciphers()  { OpenSSL_add_all_ciphers(); }
        ~_init_ciphers() { EVP_cleanup(); }
    } _initciphers;
}

// following is based on Tor SVN tor/branches/114-dist-storage/src/common/crypto.c

static const EVP_CIPHER* _get_CIPHER(const Ciphertext::cipher_t& cipher) {
    const EVP_CIPHER *c = EVP_get_cipherbyname(cipher.c_str());
    if (!c)
        throw CashException(CashException::CE_OPENSSL_ERROR,
                            "[Ciphertext::_get_CIPHER] can't find cipher %s", 
                            cipher.c_str());
    return c;
}

static const size_t _get_counter_keylen(const Ciphertext::cipher_t& cipher) {
    if      (cipher == Ciphertext::AES_128_CTR) return 16;
    else if (cipher == Ciphertext::AES_192_CTR) return 24;
    else if (cipher == Ciphertext::AES_256_CTR) return 32;
    else throw CashException(CashException::CE_OPENSSL_ERROR,
                             "[Ciphertext::_get_counter_keylen] can't find cipher %s", 
                             cipher.c_str());
}

int Ciphertext::encrypt(const char *key, char *to, 
                        const char *from, size_t fromlen, 
                        const cipher_t& cipher) {
  int outlen, tmplen, ivlen = 0;
  EVP_CIPHER_CTX ctx;
  if (_is_counter_cipher(cipher)) {
      AES_counter_crypt((const unsigned char*)from, (unsigned char*)to, 
                        fromlen, (const unsigned char*)key, _get_counter_keylen(cipher), 
                        (const unsigned char*)(key+_get_counter_keylen(cipher)));
      return fromlen;
  }
  const EVP_CIPHER *c = _get_CIPHER(cipher);

  assert(key);
  assert(to);
  assert(from);
  assert(fromlen);
  /* initialize cipher contex */
  EVP_CIPHER_CTX_init(&ctx);

  if (EVP_CIPHER_mode(c) != EVP_CIPH_ECB_MODE) {
      /* generate random initialization vector and write it to the first ivlen bytes
       * of the result */
      ivlen = EVP_CIPHER_iv_length(c);
      unsigned char iv[ivlen];
      RAND_pseudo_bytes((unsigned char *)iv, ivlen);
      /* copy initialization vector to result */
      memcpy((unsigned char *)to, iv, ivlen);
      /* set up cipher context for encryption with cipher type,
       * default implementation, given key, and initialization vector */
      EVP_EncryptInit_ex(&ctx, c, NULL, (unsigned char *)key, iv);
  } else {
      // ECB mode: no IV
      EVP_EncryptInit_ex(&ctx, c, NULL, (unsigned char *)key, NULL);
  }
  /* encrypt fromlen bytes from buffer from and write the encrypted version to
   * buffer to */
  if(!EVP_EncryptUpdate(&ctx, ((unsigned char *)to) + ivlen, &outlen,
                        (const unsigned char *)from, (int)fromlen)) {
      throw CashException(CashException::CE_OPENSSL_ERROR,
                          "[Ciphertext::encrypt] "
                          "Error in EVP_EncryptUpdate");
  }

  /* encrypt the final data */
  if(!EVP_EncryptFinal_ex(&ctx, ((unsigned char *)to) + ivlen + outlen, &tmplen)) {
      throw CashException(CashException::CE_OPENSSL_ERROR,
                          "[Ciphertext::encrypt] "
                          "Error in EVP_EncryptFinal");
  }
  outlen += tmplen;

  /* clear all information from cipher context and free up any allocated memory
   * associate with it */
  EVP_CIPHER_CTX_cleanup(&ctx);

  /* return number of written bytes */
  return outlen + ivlen;
}

int Ciphertext::decrypt(const char *key, char *to, 
                        const char *from, size_t fromlen,
                        const cipher_t& cipher, size_t offset) {
  
  int outlen, tmplen, ivlen;
  EVP_CIPHER_CTX ctx;
  if (_is_counter_cipher(cipher)) {
      AES_counter_crypt((const unsigned char*)from, (unsigned char*)to, 
                        fromlen, (const unsigned char*)key, _get_counter_keylen(cipher), 
                        (const unsigned char*)(key+_get_counter_keylen(cipher)), offset);
      return fromlen;
  }
  const EVP_CIPHER *c = _get_CIPHER(cipher);

  assert(key);
  assert(to);
  assert(from);
  assert(fromlen);

  /* initialize cipher contex */
  EVP_CIPHER_CTX_init(&ctx);

  ivlen = (EVP_CIPHER_mode(c) == EVP_CIPH_ECB_MODE) 
      ? 0 : EVP_CIPHER_iv_length(c);
  unsigned char iv[ivlen];
  /* copy initialization vector from buffer */
  if (ivlen) memcpy(iv, (unsigned const char *)from, ivlen);

  /* set up cipher context for decryption with cipher type AES-128 in CBC mode,
   * default implementation, given key, and initialization vector */
  EVP_DecryptInit_ex(&ctx, c, NULL, (unsigned char *)key, iv);

  /* decrypt fromlen-ivlen bytes from buffer from and write the decrypted version
   * to buffer to */
  if(!EVP_DecryptUpdate(&ctx, (unsigned char *)to, &outlen,
                        ((const unsigned char *)from) + ivlen,
                        (int)fromlen - ivlen)) {
      throw CashException(CashException::CE_OPENSSL_ERROR,
                          "[Ciphertext::decrypt] "
                          "Error in EVP_DecryptUpdate");
  }

  /* encrypt the final data */
  if(!EVP_DecryptFinal_ex(&ctx, ((unsigned char *)to) + outlen, &tmplen)) {
      throw CashException(CashException::CE_OPENSSL_ERROR,
                          "[Ciphertext::decrypt] "
                          "Error in EVP_DecryptFinal");
  }
  outlen += tmplen;

  /* clear all information from cipher context and free up any allocated memory
   * associate with it */
  EVP_CIPHER_CTX_cleanup(&ctx);

  /* return number of written bytes */
  return outlen;
}

size_t Ciphertext::encryptedLength(size_t len, const cipher_t& cipher) {
    if (_is_counter_cipher(cipher)) return len;
    const EVP_CIPHER *c = _get_CIPHER(cipher);
    size_t pad = EVP_CIPHER_block_size(c) + EVP_CIPHER_iv_length(c);
    return len + pad;
}

char* Ciphertext::encrypt(const char *key, const char *from, size_t fromlen, 
              const cipher_t& cipher, size_t* outlen) {
    char *tobuf = (char *)malloc(encryptedLength(fromlen, cipher));
    assert(tobuf);
    *outlen = encrypt(key, tobuf, from, fromlen, cipher);
    return tobuf;
}

int Ciphertext::encrypt(const string &key, string &to, 
                        const string &from, const cipher_t& cipher) {

    char *tobuf = (char *)malloc(encryptedLength(from.size(), cipher));
    assert(tobuf);

    int ret = encrypt(key.data(), tobuf, from.data(), from.size(), cipher);

    to.assign(tobuf, ret);
    free(tobuf);

    return ret;
}

char* Ciphertext::decrypt(const char *key, const char *from, size_t fromlen, 
                          const cipher_t& cipher, size_t *outlen) {

    char *tobuf = (char *)malloc(fromlen);
    assert(tobuf);

    *outlen = decrypt(key, tobuf, from, fromlen, cipher);
    return tobuf;
}

int Ciphertext::decrypt(const string &key, string &to, 
                        const string &from, const cipher_t& cipher) {

    char *tobuf = (char *)malloc(from.size());
    assert(tobuf);

	// throw exception if key not long enough
	if (key.size() < keyLength(cipher))
        throw CashException(CashException::CE_SIZE_ERROR,
                            "[Ciphertext::decrypt] key not long enough for cipher %s", 
                            cipher.c_str());

    int ret = decrypt(key.data(), tobuf, from.data(), from.size(), cipher);

    to.assign(tobuf, ret);
    free(tobuf);

    return ret;
}

int Ciphertext::decrypt(const string &key, string &to, 
                        const string &from, size_t offset, const cipher_t& cipher) {

    char *tobuf = (char *)malloc(from.size());
    assert(tobuf);

    int ret = decrypt(key.data(), tobuf, from.data(), from.size(), cipher, offset);

    to.assign(tobuf, ret);
    free(tobuf);

    return ret;
}

string Ciphertext::generateKey(const cipher_t& cipher) {
    if (_is_counter_cipher(cipher)) return generateKey(cipher, NTL::to_ZZ(0));
    const EVP_CIPHER *c = _get_CIPHER(cipher);
    size_t keysz = EVP_CIPHER_key_length(c);
    unsigned char key[keysz];
    RAND_pseudo_bytes(key, keysz);
    return string((char *)key, keysz);
}

// generate key + IV for AES counter mode
// assumes R is hash of shared secret
string Ciphertext::generateKey(const cipher_t& cipher, const ZZ& r) {
    if (!_is_counter_cipher(cipher)) return generateKey(cipher);
    size_t keysz = _get_counter_keylen(cipher);
    unsigned char key[keysz + AES_BLOCK_SIZE];
    RAND_pseudo_bytes(key, keysz);
    if (r != 0)
        BytesFromZZ(key + keysz, r, AES_BLOCK_SIZE); // use R as shared-secret IV
    else
        RAND_pseudo_bytes(key+keysz, AES_BLOCK_SIZE); // make random IV
    return string((char *)key, keysz + AES_BLOCK_SIZE);
}

// set key + IV for AES counter mode
// assumes R is hash of shared secret
string Ciphertext::makeKey(const cipher_t& cipher, const string& k, const ZZ& r) {
    if (!_is_counter_cipher(cipher)) return k;
    size_t keysz = _get_counter_keylen(cipher);
    unsigned char key[keysz + AES_BLOCK_SIZE];
    assert(k.size() >= keysz);
    memcpy(key, k.data(), keysz);
    BytesFromZZ(key + keysz, r, AES_BLOCK_SIZE); // add IV 
    return string((char *)key, keysz + AES_BLOCK_SIZE);    
}

size_t Ciphertext::keyLength(const cipher_t& cipher) {
	if (_is_counter_cipher(cipher)) return _get_counter_keylen(cipher);
	const EVP_CIPHER *c = _get_CIPHER(cipher);
    return EVP_CIPHER_key_length(c);
}

#define GETU32(pt) (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ ((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))
#define PUTU32(ct, st) { (ct)[0] = (u8)((st) >> 24); (ct)[1] = (u8)((st) >> 16); (ct)[2] = (u8)((st) >>  8); (ct)[3] = (u8)(st); }
typedef unsigned long u32;
typedef unsigned char u8;

// this function from openssl-0.9.8i/crypto/aes/aes_ctr.c AES_ctr128_inc 
static void AES_counter128_inc(unsigned char *counter) {
	unsigned long c;

	/* Grab bottom dword of counter and increment */
	c = GETU32(counter + 12);
	c++;	c &= 0xFFFFFFFF;
	PUTU32(counter + 12, c);

	/* if no overflow, we're done */
	if (c) return;

	/* Grab 1st dword of counter and increment */
	c = GETU32(counter +  8);
	c++;	c &= 0xFFFFFFFF;
	PUTU32(counter +  8, c);

	/* if no overflow, we're done */
	if (c) return;

	/* Grab 2nd dword of counter and increment */
	c = GETU32(counter +  4);
	c++;	c &= 0xFFFFFFFF;
	PUTU32(counter +  4, c);

	/* if no overflow, we're done */
	if (c) return;

	/* Grab top dword of counter and increment */
	c = GETU32(counter +  0);
	c++;	c &= 0xFFFFFFFF;
	PUTU32(counter +  0, c);
}

static void AES_counter128_set(unsigned char *counter, const unsigned char *iv, 
                               unsigned long val) {
    unsigned long c;
    c = GETU32(iv);
    PUTU32(counter, c);

    c = GETU32(iv + 4);
    PUTU32(counter + 4, c);

    c = GETU32(iv + 8);
    PUTU32(counter + 8, c);

    // just clear bottom 32 bits (use 64?) of IV for counter value: 
    // could also use XOR, addition, hashing
    PUTU32(counter + 12, val);
}

void Ciphertext::AES_counter_crypt(const unsigned char *in, unsigned char *out,
                                   size_t length, 
                                   const unsigned char *keybuf,
                                   size_t keybytes, // 16, 24, or 32
                                   const unsigned char *iv,
                                   size_t offset) 
{
    size_t l = length;
    unsigned char enc_ctrs[AES_BLOCK_SIZE];
    unsigned char counter_iv[AES_BLOCK_SIZE];
    AES_KEY key;
    assert(in && out && keybuf && iv);
    assert(keybytes == 16 || keybytes == 24 || keybytes == 32);

    // pick counter (and AES block offset n) from byte offset
    unsigned long blocknum = offset / AES_BLOCK_SIZE;
    size_t n = offset % AES_BLOCK_SIZE;

    AES_set_encrypt_key(keybuf, keybytes*8, &key);
    AES_counter128_set(counter_iv, iv, blocknum);

    if (n) { // offset not aligned with block size
        AES_encrypt(counter_iv, enc_ctrs, &key);
        AES_counter128_inc(counter_iv);
    }

    while (l--) {
        if (n == 0) { // encrypt next 16-byte counter value
            AES_encrypt(counter_iv, enc_ctrs, &key);
            AES_counter128_inc(counter_iv);
        }
        // XOR counter byte n with input data
        *(out++) = *(in++) ^ enc_ctrs[n];
        n = (n+1) % AES_BLOCK_SIZE;
    }
}

EncBuffer* Buffer::encrypt(const Ciphertext::cipher_t& alg, const string& k) const 
{
	string key = (!k.empty()) ? k : Ciphertext::generateKey(alg);
    size_t ctl;
    char *ct = Ciphertext::encrypt(key.data(), buf, len, alg, &ctl);
    return new EncBuffer(ct, ctl, key, alg);
}
