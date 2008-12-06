/*
 * Copyright (c) 2008, Adrian Thurston <thurston@cs.queensu.ca>
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
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/md5.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mysql.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "sppd.h"

char *strend( char *s )
{
	return s + strlen(s);
}

void pass_hash( char *dest, const char *user, const char *pass )
{
	unsigned char pass_bin[16];
	char pass_comb[1024];
	sprintf( pass_comb, "%s:spp:%s", user, pass );
	MD5( (unsigned char*)pass_comb, strlen(pass_comb), pass_bin );
	sprintf( dest, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x", 
		pass_bin[0], pass_bin[1], pass_bin[2], pass_bin[3],
		pass_bin[4], pass_bin[5], pass_bin[6], pass_bin[7],
		pass_bin[8], pass_bin[9], pass_bin[10], pass_bin[11],
		pass_bin[12], pass_bin[13], pass_bin[14], pass_bin[15] );
}

void new_user( const char *key, const char *user, const char *pass, const char *email )
{
	char *n, *e, *d, *p, *q, *dmp1, *dmq1, *iqmp;
	RSA *rsa;
	MYSQL *mysql, *connect_res;
	char pass_hashed[33];
	char *query;
	long query_res;

	/* Check the authentication. */
	if ( strcmp( key, CFG_COMM_KEY ) != 0 ) {
		printf( "ERROR communication key invalid\r\n" );
		goto flush;
	}

	/* Generate a new key. */
	rsa = RSA_generate_key( 1024, RSA_F4, 0, 0 );
	if ( rsa == 0 ) {
		printf( "ERROR key generation failed\r\n");
		goto flush;
	}

	/* Extract the components to hex strings. */
	n = BN_bn2hex( rsa->n );
	e = BN_bn2hex( rsa->e );
	d = BN_bn2hex( rsa->d );
	p = BN_bn2hex( rsa->p );
	q = BN_bn2hex( rsa->q );
	dmp1 = BN_bn2hex( rsa->dmp1 );
	dmq1 = BN_bn2hex( rsa->dmq1 );
	iqmp = BN_bn2hex( rsa->iqmp );

	/* Open the database connection. */
	mysql = mysql_init(0);
	connect_res = mysql_real_connect( mysql, CFG_DB_HOST, CFG_DB_USER, 
			CFG_ADMIN_PASS, CFG_DB_DATABASE, 0, 0, 0 );
	if ( connect_res == 0 ) {
		printf( "ERROR failed to connect to the database\r\n");
		goto close;
	}

	/* Hash the password. */
	pass_hash( pass_hashed, user, pass );

	/* Create the query. */
	query = (char*)malloc( 1024 + 256*15 );
	strcpy( query, "INSERT INTO user VALUES('" );
	mysql_real_escape_string( mysql, strend(query), user, strlen(user) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), pass_hashed, strlen(pass_hashed) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), email, strlen(email) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), n, strlen(n) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), e, strlen(e) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), d, strlen(d) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), p, strlen(p) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), q, strlen(q) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), dmp1, strlen(dmp1) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), dmq1, strlen(dmq1) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), iqmp, strlen(iqmp) );
	strcat( query, "' );" );

	/* Execute the query. */
	query_res = mysql_query( mysql, query );
	if ( query_res != 0 ) {
		printf( "ERROR internal error: %s %d\r\n", __FILE__, __LINE__ );
		goto free_query;
	}

	printf( "OK\r\n" );

free_query:
	free( query );
close:
	OPENSSL_free( n );
	OPENSSL_free( e );
	OPENSSL_free( d );
	OPENSSL_free( p );
	OPENSSL_free( q );
	OPENSSL_free( dmp1 );
	OPENSSL_free( dmq1 );
	OPENSSL_free( iqmp );

	RSA_free( rsa );
	mysql_close( mysql );
flush:
	fflush( stdout );
}

void public_key( const char *user )
{
	MYSQL *mysql, *connect_res;
	char *query;
	long query_res;
	MYSQL_RES *result;
	MYSQL_ROW row;

	mysql = mysql_init(0);
	connect_res = mysql_real_connect( mysql, CFG_DB_HOST, CFG_DB_USER, 
			CFG_ADMIN_PASS, CFG_DB_DATABASE, 0, 0, 0 );
	if ( connect_res == 0 ) {
		printf( "ERROR failed to connect to the database\r\n");
		goto close;
	}

	/* Make the query. */
	query = (char*)malloc( 1024 + 256*15 );
	strcpy( query, "SELECT rsa_n, rsa_e FROM user WHERE user = '" );
	mysql_real_escape_string( mysql, strend(query), user, strlen(user) );
	strcat( query, "';" );

	/* Execute the query. */
	query_res = mysql_query( mysql, query );
	if ( query_res != 0 ) {
		printf( "ERROR internal error: %s %d\r\n", __FILE__, __LINE__ );
		goto query_fail;
	}

	/* Check for a result. */
	result = mysql_store_result( mysql );
	row = mysql_fetch_row( result );
	if ( !row ) {
		printf( "ERROR user not found\r\n" );
		goto free_result;
	}

	/* Everythings okay. */
	printf( "OK %s/%s\n", row[0], row[1] );

free_result:
	mysql_free_result( result );
query_fail:
	free( query );
close:
	mysql_close( mysql );
	fflush(stdout);
}

long open_inet_connection( const char *hostname, unsigned short port )
{
	sockaddr_in servername;
	hostent *hostinfo;
	long socketFd, connectRes;

	/* Create the socket. */
	socketFd = socket( PF_INET, SOCK_STREAM, 0 );
	if ( socketFd < 0 )
		return ERR_SOCKET_ALLOC;

	/* Lookup the host. */
	servername.sin_family = AF_INET;
	servername.sin_port = htons(port);
	hostinfo = gethostbyname (hostname);
	if ( hostinfo == NULL ) {
		::close( socketFd );
		return ERR_RESOLVING_NAME;
	}

	servername.sin_addr = *(in_addr*)hostinfo->h_addr;

	/* Connect to the listener. */
	connectRes = connect( socketFd, (sockaddr*)&servername, sizeof(servername) );
	if ( connectRes < 0 ) {
		::close( socketFd );
		return ERR_CONNECTING;
	}

	return socketFd;
}

long fetch_public_key_db( PublicKey &pub, MYSQL *mysql, const char *identity )
{
	long result = 0;
	char *query;
	long query_res;
	MYSQL_RES *select_res;
	MYSQL_ROW row;

	/* Make the query. */
	query = (char*)malloc( 1024 + 256*15 );
	strcpy( query, "SELECT rsa_n, rsa_e FROM public_key WHERE identity = '" );
	mysql_real_escape_string( mysql, strend(query), identity, strlen(identity) );
	strcat( query, "';" );

	/* Execute the query. */
	query_res = mysql_query( mysql, query );
	if ( query_res != 0 ) {
		result = ERR_QUERY_ERROR;
		goto query_fail;
	}

	/* Check for a result. */
	select_res= mysql_store_result( mysql );
	row = mysql_fetch_row( select_res );
	if ( row ) {
		pub.n = strdup( row[0] );
		pub.e = strdup( row[1] );
		result = 1;
	}

	/* Done. */
	mysql_free_result( select_res );

query_fail:
	free( query );
	return result;
}

long store_public_key( MYSQL *mysql, const char *identity, PublicKey &pub )
{
	long result = 0, query_res;
	char *query;

	/* Make the query. */
	query = (char*)malloc( 1024 + 256*3 );
	strcpy( query, "INSERT INTO public_key VALUES('" );
	mysql_real_escape_string( mysql, strend(query), identity, strlen(identity) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), pub.n, strlen(pub.n) );
	strcat( query, "', '" );
	mysql_real_escape_string( mysql, strend(query), pub.e, strlen(pub.e) );
	strcat( query, "' );" );

	/* Execute the query. */
	query_res = mysql_query( mysql, query );
	if ( query_res != 0 ) {
		result = ERR_QUERY_ERROR;
		goto query_fail;
	}

query_fail:
	free( query );
	return result;
}

long fetch_public_key( PublicKey &pub, MYSQL *mysql, const char *identity, 
		const char *host, const char *user )
{
	/* First try to fetch the public key from the database. */
	long result = fetch_public_key_db( pub, mysql, identity );
	if ( result < 0 ) {
		printf("ERROR db fetch error: %ld\n", result );
		return result;
	}

	/* If the db fetch failed, get the public key off the net. */
	if ( result == 0 ) {
		result = fetch_public_key_net( pub, host, user );
		if ( result < 0 ) {
			printf("ERROR net fetch failed: %ld\n", result );
			return result;
		}

		/* Store it in the db. */
		result = store_public_key( mysql, identity, pub );
		if ( result < 0 ) {
			printf("ERROR db store fetch failed: %ld\n", result );
			return result;
		}
	}

	return 0;
}

void friend_req( const char *user, const char *identity, 
		const char *id_host, const char *id_user )
{
	/* a) verifies challenge response
	 * b) fetches $URI/id.asc (using SSL)
	 * c) randomly generates a one-way relationship id ($FR-RELID)
	 * d) randomly generates a one-way request id ($FR-REQID)
	 * e) encrypts $FR-RELID to friender and signs it
	 * f) makes message available at $FR-URI/friend-request/$FR-REQID.asc
	 * g) redirects the user's browser to $URI/return-relid?uri=$FR-URI&reqid=$FR-REQID
	 */

	MYSQL *mysql, *connect_res;
	long fr;
	unsigned char FR_RELID[16];
	unsigned char FR_REQID[16];

	/* Open the database connection. */
	mysql = mysql_init(0);
	connect_res = mysql_real_connect( mysql, CFG_DB_HOST, CFG_DB_USER, 
			CFG_ADMIN_PASS, CFG_DB_DATABASE, 0, 0, 0 );
	if ( connect_res == 0 ) {
		printf( "ERROR failed to connect to the database\r\n");
		goto close;
	}

	PublicKey pub;
	fr = fetch_public_key( pub, mysql, identity, id_host, id_user );
	if ( fr < 0 ) {
		printf("ERROR fetch_public_key failed: %ld\n", fr );
		goto close;
	}

	RAND_bytes( FR_RELID, 16 );
	RAND_bytes( FR_REQID, 16 );

	printf( "relid: " );
	for ( int i = 0; i < 16; i++ )
		printf( "%x", FR_RELID[i] );
	printf( "\nreqid: " );
	for ( int i = 0; i < 16; i++ )
		printf( "%x", FR_REQID[i] );
	printf( "\n" );

close:
	mysql_close( mysql );
	fflush( stdout );
}

