#include "Hash.h"
#include "CashException.h"
#include "base64.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <boost/thread/tss.hpp>
#include <string>
#include <assert.h>
#include "CommonFunctions.h"
#include "MerkleContract.h"
#include "Merkle.h"
#include "Serialize.h"

const char* Hash::alg_names[] = { "sha1", "sha256", "md5" };
//const Hash::alg_t Hash::SHA1 = 0;
//const Hash::alg_t Hash::SHA256 = 1;
//const Hash::alg_t Hash::MD5 = 2;

hashalg_t Hash::get_algbyname(const string &name) {
    for (int i=0; i < MAXALG; i++) {
        if (name == alg_names[i]) return (hashalg_t)i;
    }
    throw CashException(CashException::CE_UNKNOWN_ERROR,
                        "[Hash::get_algbyname] Can't find digest for %s", 
                        name.c_str());
}

ZZ Hash::hash(const vector<ZZ> &vec, const alg_t alg)
{
    ostringstream buf;
    // hash the values' decimal representations, concatenated
    for (unsigned i=0; i < vec.size(); i++)
		buf << vec[i];
    return hash(buf.str().data(), buf.str().length(), alg);
}

ZZ Hash::hash(const ZZ &z, const alg_t alg, const string& key)
{
    // hash the value's binary representation
    return hash(ZZToBytes(z), alg, key, Hash::TYPE_PLAIN);
}

hash_t Hash::hash(const string& data, const alg_t alg, const string &key, const int hashType)
{
	return hash(data.data(), data.length(), alg, key, hashType);
}

hash_t Hash::hash(const char* data, size_t len,
                  const alg_t alg, const string &key, const int hashType)
{
	if (hashType == Hash::TYPE_PLAIN) {
		return hash(data, len, alg, key);
	} else if (hashType == Hash::TYPE_MERKLE) {
		MerkleContract contract(key, alg);
		MerkleTree tree(data, len, contract);
		return tree.getRoot();
	} else {
		throw CashException(CashException::CE_HASH_ERROR,
							"[Hash::hash] Unknown hash type used");
	}
}

hash_t Hash::hash(const char* data, size_t len, const alg_t alg, const string &key)
{
	if (key.empty())
		return Hash::hash(data, len, alg);
	else
		return Hash::hmac(data, len, alg, key);
}

// provide OpenSSL MD CTX cleanup function to thread_specific_ptr
boost::thread_specific_ptr<EVP_MD_CTX> _brownie_hash_ctx(EVP_MD_CTX_destroy);

hash_t Hash::hash(const char *buf, int len, const alg_t alg)
{
	if (_brownie_hash_ctx.get() == 0)
		_brownie_hash_ctx.reset(EVP_MD_CTX_create());
	EVP_MD_CTX *ctx = _brownie_hash_ctx.get();

    const EVP_MD *m;
    char digest[EVP_MAX_MD_SIZE];
    unsigned int dlen;

    m = get_MD(alg);
    if(!m)
        throw CashException(CashException::CE_UNKNOWN_ERROR,
                            "[Hash::hash] Can't find digest %s", alg);

    EVP_DigestInit_ex(ctx, m, NULL);
    EVP_DigestUpdate(ctx, buf, len);
    EVP_DigestFinal_ex(ctx, (unsigned char *)digest, &dlen);

    // return HashVal container of digest
    hash_t ret(digest, dlen, alg, TYPE_PLAIN);
    return ret;
}

hash_t Hash::hmac(const char* data, size_t len, const alg_t alg, const string& key)
{
    const EVP_MD *m;
    char digest[EVP_MAX_MD_SIZE];
    unsigned int dlen;

    m = get_MD(alg);

	if(!m)
		throw CashException(CashException::CE_UNKNOWN_ERROR,
							"[Hash::hmac] Can't find digest %s", alg);

    HMAC(m, key.data(), key.length(), 
	 (unsigned char *)data, len, 
	 (unsigned char *)digest, &dlen);

    // return C++ string container of digest
    hash_t ret(digest, dlen, alg, TYPE_PLAIN, key);
    return ret;
}

hash_t Hash::hash(const vector<ZZ> &vec, const string& label, const alg_t alg, const string &key) {
	return hash(saveString(vec) + label, alg, key, Hash::TYPE_PLAIN);
}

#define IS_POW2(x) (x) && !( ((x)-1) & (x) )

vector<hash_t> Hash::hashChunks(const string& data, const alg_t alg,
								const string& key, size_t &chunksz_out)
{
	return Hash::hashChunks(data.data(), data.length(), alg, key, chunksz_out);
}


// if chunksz <= 0, pick chunksz for you
vector<hash_t> Hash::hashChunks(const char *buf, size_t len, const alg_t alg, 
								const string& key, size_t &chunksz_out)
{
  vector<hash_t> out;
  assert(len);
			
  if (chunksz_out <= 0) { // pick chunk size
#if 0
    if ( !IS_POW2(len) ) {    // len not a power of two
      chunksz_out = (len >= 4) ? (len/4+1) : len; // just make 4 chunks (XXX)
    } else {                  // len is power of two
      if (len >= (1 << 6)) {  //   len >= 64 B
        chunksz_out = (len >> 6); //     make 64 chunks
      } else {                //   len < 64 bytes
        chunksz_out = len;        //     make 1 chunk
      }
    }
#else
    if (len >= (1 << 10)) {   // len >= 2KB
        chunksz_out = (len >> 6); //  make 64 chunks
    } else {
        chunksz_out = len/2;      //  make two chunks
    }
#endif
  }
    
  // XXX ignores unaligned data (i.e. 16 bytes AES padding)
  for (unsigned i=0; i < len/chunksz_out; i++) {
    out.push_back( hash(buf + i*chunksz_out, chunksz_out, alg, key) );
  }

#if 0 
  // XXX doesn't guarantee assertion below
  for (size_t off = 0; off < len; off += chunksz_out) {
    out.push_back( hash(buf+off, 
                        chunksz_out < (len - off) ? chunksz_out : (len - off),
                        alg) );
  }
#endif
  assert(IS_POW2(out.size()));
  
  return out;
}



/* HMACFUNCTION */
/*
HmacFunction::HmacFunction(const hashalg_t hashAlgorithm, const string& hashKey)
{
   key = hashKey;
   evpmd = Hash::get_MD(hashAlgorithm);
   alg = hashAlgorithm;
   if(!evpmd)
   {
      throw CashException(CashException::CE_UNKNOWN_ERROR,
                          "[HmacFunction::HmacFunction] Can't find digest");
   }
}

hash_t HmacFunction::hash(const string& data) const
{
   char digest[EVP_MAX_MD_SIZE];
   unsigned int dlen;
   
   HMAC(evpmd, key.data(), key.length(),
        (unsigned char *)data.data(), data.length(),
         (unsigned char*)digest, &dlen);
   
   // XXX HmacFunction is only for Merkle proofs, so we'll return TYPE_MERKLE
   // but this isn't really a good solution
   hash_t ret(digest, dlen, alg, Hash::TYPE_MERKLE, key);
   return ret;
}

hash_t HmacFunction::hash(const string& left, const string& right) const
{
   return hash(left+right);
}
*/

/* used by FEContract, FEInitiator, FEResponder, etc */

hash_t Hash::hash(const vector<Ptr<const Buffer> >& buf, const hashalg_t& alg,
							const string &key, const int hashType)
{
	vector<hash_t> hash;
	for (unsigned i = 0; i < buf.size(); i++)
	{
		hash.push_back(buf[i]->hash(alg, key, Hash::TYPE_PLAIN));
	}
	return Hash::hash(hash, alg, key, hashType);
}

hash_t Hash::hash(const vector<Ptr<Buffer> >& buf, const hashalg_t& alg,
							const string &key, const int hashType)
{
	vector<hash_t> hash;
	for (unsigned i = 0; i < buf.size(); i++)
	{
		hash.push_back(buf[i]->hash(alg, key, Hash::TYPE_PLAIN));
	}
	return Hash::hash(hash, alg, key, hashType);
}

hash_t Hash::hash(const vector<Ptr<EncBuffer> >& buf, const hashalg_t& alg,
							const string &key, const int hashType)
{
	vector<hash_t> hash;
	for (unsigned i = 0; i < buf.size(); i++)
	{
		hash.push_back(buf[i]->hash(alg, key, Hash::TYPE_PLAIN));
	}
	return Hash::hash(hash, alg, key, hashType);
}

hash_t Hash::hash(const vector<Ptr<const EncBuffer> >& buf, const hashalg_t& alg,
							const string &key, const int hashType)
{
	vector<hash_t> hash;
	for (unsigned i = 0; i < buf.size(); i++)
	{
		hash.push_back(buf[i]->hash(alg, key, Hash::TYPE_PLAIN));
	}
	return Hash::hash(hash, alg, key, hashType);
}

hash_t Hash::hash(const vector<hash_t>& hashes, const hashalg_t& alg,
							const string &key, const int hashType)
{
	if (hashType == Hash::TYPE_PLAIN) {
		string str = saveString(hashes);
		return Hash::hash(str, alg, key, hashType);
	} else if (hashType == Hash::TYPE_MERKLE) {
		MerkleContract contract(key, alg);
		MerkleTree tree(hashes, contract);
		return tree.getRoot();
	} else {
		throw CashException(CashException::CE_HASH_ERROR,
							"[Hash::hash] Unknown hash type used");
	}
}
