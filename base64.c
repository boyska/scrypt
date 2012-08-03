#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "base64.h"

/*
** Translation Table as described in RFC1113
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
static void encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

/*
** encode
**
** base64 encode a stream adding padding and line breaks as per spec.
*/
void be64_encode( const uint8_t infile[], size_t in_len, char **outfile, int linesize )
{
    unsigned char in[3], out[4];
    int i, len, blocksout = 0;
	signed long int inlen = in_len;
	char *outstring = calloc(inlen*4/3 + 4, sizeof(char));
	*outfile = outstring;

    while( inlen > 0 ) {
        len = 0;
        for( i = 0; i < 3 ; i++ ) {
/*            printf("Read %d while remaining %ld, done %d\n", *infile, inlen, len);*/
/*            fsync(stdout);*/
			if(inlen > 0)
				in[i] = (unsigned char) *infile;
			else
				in[i] = 0;
            if( inlen-- > 0 ) {
                len++;
				infile++;
            }
        }
        if( len ) {
            encodeblock( in, out, len );
            for( i = 0; i < 4; i++ ) {
                *(outstring++) = out[i];
            }
            blocksout++;
        }
        if( linesize && (blocksout >= (linesize/4) || inlen <= 0 ) ) {
            if( blocksout ) {
                *( outstring++ ) = '\n';
            }
            blocksout = 0;
        }
    }
	*outstring = '\0';
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
static void decodeblock( unsigned char in[4], unsigned char out[3] )
{   
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

/*
** decode
**
** decode a base64 encoded stream discarding padding, line breaks and noise
** will allocate memory for outfile (but you have to free it yourself);
*/
size_t be64_decode( const char infile[], uint8_t **outfile )
{
    unsigned char in[4], out[3], v;
    int i, len;
	uint8_t *outstring = calloc(strlen(infile)*3/4+strlen(infile)%3, sizeof(uint8_t));
	*outfile = outstring;
	size_t totallen = 0;

    while( *infile ) {
        for( len = 0, i = 0; i < 4 && *infile; i++,infile++ ) {
            v = 0;
            while( *infile && v == 0 ) { //v==0 means "invalid"; otherwise, it is the decoding
                v = (unsigned char) *( infile++ );
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
                if( v ) {
                    v = (unsigned char) ((v == '$') ? 0 : v - 61);
                }
            }
			infile--;//soo ugly
            if( *infile ) {
                if( v ) {
					len++;
                    in[ i ] = (unsigned char) (v - 1);
                }
            }
            else {
                in[i] = 0;
            }
        }
        if( len ) {
            decodeblock( in, out );
            for( i = 0; i < len - 1; i++ ) {
				*(outstring++) = out[i];
				totallen++;
            }
        }
    }
	return totallen;
}

