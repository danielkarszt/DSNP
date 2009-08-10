/*
 * Copyright (c) 2008-2009, Adrian Thurston <thurston@complang.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <openssl/rand.h>
#include <openssl/bio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "sppd.h"

BIO *bioIn = 0;
BIO *bioOut = 0;

void read_rcfile( const char *confFile )
{
	FILE *rcfile = fopen( confFile, "r" );
	if ( rcfile == NULL ) {
		fprintf( stderr, "failed to open the config file \"%s\", exiting\n", confFile );
		exit(1);
	}

	/* FIXME: this must be fixed. */
	static char buf[1024*16];
	long len = fread( buf, 1, 1024*16, rcfile );
	rcfile_parse( buf, len );
}

const char *configFile = 0;
const char *siteName = 0;
bool runQueue = false;
bool test = false;

int check_args( int argc, char **argv )
{
	while ( true ) {
		int opt = getopt( argc, argv, "q:t" );

		if ( opt < 0 )
			break;

		switch ( opt ) {
			case 'q':
				runQueue = true;
				siteName = optarg;
				break;
			case 't':
				test = true;
				break;
		}
	}

	if ( optind < argc )
		configFile = argv[optind];
	else {
		fprintf( stderr, "expected config file argument\n" );
		exit(1);
	}

	return 0;
}


void dieHandler( int signum )
{
	error( "caught signal %d, exiting\n", signum );
	exit(1);
}

void setupSignals()
{
	signal( SIGHUP, &dieHandler );
	signal( SIGINT, &dieHandler );
	signal( SIGQUIT, &dieHandler );
	signal( SIGILL, &dieHandler );
	signal( SIGABRT, &dieHandler );
	signal( SIGFPE, &dieHandler );
	signal( SIGSEGV, &dieHandler );
	signal( SIGPIPE, &dieHandler );
	signal( SIGTERM, &dieHandler );
}

int run_test();

int main( int argc, char **argv )
{
	if ( check_args( argc, argv ) < 0 ) {
		fprintf( stderr, "expecting: sppd [options] config\n" );
		fprintf( stderr, "  options: -q<site>    don't listen, run queue\n" );
		exit(1);
	}

	/* Set up the input BIO to wrap stdin. */
	BIO *bioFdIn = BIO_new_fd( fileno(stdin), BIO_NOCLOSE );
	bioIn = BIO_new( BIO_f_buffer() );
	BIO_push( bioIn, bioFdIn );

	/* Set up the output bio to wrap stdout. */
	BIO *bioFdOut = BIO_new_fd( fileno(stdout), BIO_NOCLOSE );
	bioOut = BIO_new( BIO_f_buffer() );
	BIO_push( bioOut, bioFdOut );

	setupSignals();

	read_rcfile( configFile );

	RAND_load_file("/dev/urandom", 1024);

	openLogFile();

	if ( runQueue )
		run_queue( siteName );
	else if ( test )
		run_test();
	else 
		server_parse_loop();
}
