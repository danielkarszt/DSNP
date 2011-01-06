#include "dsnp.h"
#include "error.h"

#include <unistd.h>
#include <fcntl.h>

int TlsConnect::read( char *buf, long len )
{
	/* For the time being, most calls need to be blocking because not all calls
	 * are in this framework. We set nonblocking only temporarily */
	
//	/* Turn on non-blocking. */
//	long origFlags = fcntl( socketFd, F_GETFL );
//	fcntl( socketFd, F_SETFL, origFlags | O_NONBLOCK );

	int readRes = BIO_gets( sbio, buf, 8192 );
	//:b par	

//	/* Restore flags. */
//	fcntl( socketFd, F_SETFL, origFlags );
	return readRes;
}

void TlsConnect::publicKey( const char *user )
{
	BIO_printf( sbio, "public_key %s\r\n", user );
	(void)BIO_flush( sbio );

	/* Read the result. */
	static char buf[8192];
	int readRes = BIO_gets( sbio, buf, 8192 );

	/* If there was an error then fail the fetch. */
	if ( readRes <= 0 )
		throw ReadError();
	
	result = buf;
}

int TlsConnect::readParse( Parser &parser )
{
	static char buf[8192];

	int readRes = BIO_gets( sbio, buf, 8192 );

	/* If there was an error then fail the fetch. */
	if ( readRes <= 0 )
		throw ReadError();

	parser.data( buf, readRes );

	return readRes;
}

int TlsConnect::printf( const char *fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	long result = BIO_vprintf( sbio, fmt, args );
	va_end( args );
	(void)BIO_flush( sbio );
	return result;
}
