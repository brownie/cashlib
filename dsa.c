#include <openssl/dsa.h>
#include <openssl/asn1.h>

// wrapper for broken DSAparams_dup macro, as in: 
//   http://www.mail-archive.com/openssl-users@openssl.org/msg37985.html
//   http://napali.borg.ch/ACE/tao/ssliop/a00154.html

DSA *DSAparams_dup_wrap(const DSA* dsa)
{
	return DSAparams_dup(dsa);
}
