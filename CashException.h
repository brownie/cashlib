#ifndef CASHEXCEPTION_H_
#define CASHEXCEPTION_H_

#include <exception>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

class CashException : public exception
{
	public:
		CashException() throw ();
		CashException(const CashException &e) throw ();
		CashException(int ecode) throw ();
		/*! create CashException with printf()-like format string */
		CashException(int ecode, const char *fmt, ...) throw ();
		CashException(int ecode, string &explanation) throw ();
		static const int FMTBUF_LEN = 4096;

		virtual ~CashException() throw ();

		int getErrorCode() const throw ();
		virtual const char *what() const throw ();
        
        static string getOpenSSLError();

		// a list of error codes
		static const int CE_NO_ERROR = 0; // No error
		static const int CE_UNKNOWN_ERROR = -1; // Unknown error
		static const int CE_SECURITY_ERROR = -2; // Problem w/ security params
		static const int CE_SIZE_ERROR = -3; // Problem w/ vector/array sizes
		static const int CE_IO_ERROR = -4; // Problem w/ input/output
		static const int CE_FE_ERROR = -5; // Problem w/ fair exchange
		static const int CE_HASH_ERROR = -6; // Problem w/ hashes
		static const int CE_OPENSSL_ERROR = -7; // Problem w/ OpenSSL
		static const int CE_TIMER_ERROR = -8; // Problem w/ Timer
		static const int CE_PARSE_ERROR = -9; // Problem w/ parsing
		static const int CE_NTL_ERROR = -10; // Problem w/ NTL

	private:
		int errorCode;
		string description;
        
};

#endif
