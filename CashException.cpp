#include "CashException.h"
#include <openssl/bio.h>
#include <openssl/err.h>

namespace {
    struct _init_openssl_errors { 
        _init_openssl_errors() { ERR_load_crypto_strings(); }
        ~_init_openssl_errors() { ERR_free_strings(); }
    } _initerrors;
}

CashException::CashException() throw ()
	: errorCode(-1), description()
{
}

CashException::CashException(const CashException &original) throw ()
	: exception(original), errorCode(original.errorCode)
{
	description = original.description;
}

CashException::CashException(int ecode) throw ()
	: errorCode(ecode), description()
{
    if (ecode == CE_OPENSSL_ERROR) description += getOpenSSLError();
}

CashException::CashException(int ecode, string &explanation) throw ()
	: errorCode(ecode), description(explanation)
{
    if (ecode == CE_OPENSSL_ERROR) description += getOpenSSLError();
}

CashException::CashException(int ecode, const char *fmt, ...) throw ()
    : errorCode(ecode)
{
    char buf[FMTBUF_LEN];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, FMTBUF_LEN, fmt, args);
    va_end(args);
    
    description = string(buf);
    if (ecode == CE_OPENSSL_ERROR) description += "\n[OpenSSL err] " + getOpenSSLError();
}


CashException::~CashException() throw ()
{
}

string CashException::getOpenSSLError() {
	char buffer[1024];
	BIO *bio = BIO_new(BIO_s_mem());
	ERR_print_errors(bio);
	ERR_clear_error();
	(void)BIO_flush(bio);

	int len = BIO_gets(bio, buffer, 1024);
	(void)BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free(bio);

	if (len < 0) {
		return "CashException::getOpenSSLError: can't read error";
	}
    
    return string(buffer, len);
}

int CashException::getErrorCode() const throw () {
	return errorCode;
}

const char *CashException::what() const throw () {
	if (description.size())
		return description.c_str();

	// OK -- no description -- just go by what the error code says
	switch (errorCode)
	{
		case CashException::CE_NO_ERROR:
			return "No error. Not sure why we threw an exception.";
		case CashException::CE_UNKNOWN_ERROR:
			return "Unknown error";
		case CashException::CE_SECURITY_ERROR:
			return "Security input parameter error";
		case CashException::CE_SIZE_ERROR:
			return "Size error";
		default:
			return "Illegal error code";
	}
}

