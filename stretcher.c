
#include "scrypt_platform.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "scryptenc.h"
#include "warn.h"
#include "base64.h"

/* ***********************************
 * scrypt-strecth
 *
 * with no parameters (or, TODO, with just maxresource parameters), will
 *      calculate appropriate parameters and output them
 * with parameters will stretch the key (which will be expected as input)

 * TODO: params maxmem, maxmemfrac, maxtime
 * TODO: --batch for the current, machine readable output: otherwise it will be clearer
*/
void usage(char *argv[]) {
	fprintf(stderr, "Usage:\t%s\n"
			        "      \t%s logN r p saltfile\n", argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
	int logN;
	uint64_t N;
	uint32_t r, p;
	size_t maxmem = 0;
	double maxmemfrac = 0.5;
	double maxtime = 5.0;
	int rc;
	uint8_t *salt;
	uint8_t dk[64]; //derived key
	uint8_t *passwd;
	char *salt_encoded = NULL;
	FILE *f;
	char *dk_be64;

	int count;

	/* Pick values for N, r, p. */
	if(argc == 1) {
		if ((rc = pickparams(maxmem, maxmemfrac, maxtime,
			&logN, &r, &p)) != 0)
			return (rc);
		salt = calloc(32, sizeof(uint8_t));
		if ((rc = getsalt(salt)) != 0)
			return (rc);
		be64_encode(salt, 32, &salt_encoded, 0);
		printf("%d\n%" PRIu32 "\n%" PRIu32 "\n", logN, r, p);
		printf("%s\n", salt_encoded);
		free(salt_encoded);
		return 0;
	} 
	if( argc < 5) {
		usage(argv);
		return 127;
	}
	sscanf(argv[1], "%d", &logN);
	sscanf(argv[2], "%" SCNu32, &r);
	sscanf(argv[3], "%" SCNu32, &p);
	sscanf(argv[4], "%ms", &salt_encoded);
	fprintf(stderr, "Insert password: ");
	scanf("%ms", &passwd);
	N = (uint64_t)(1) << logN;

	//TODO: stop saltfile madness! we want a base64encoding as a parameter!
	if(argc >= 5) {
	}
	if(salt_encoded) { //got salt as input
		if(be64_decode(salt_encoded, &salt) < 32) {
			fprintf(stderr, "Error: your salt is too short, this is not secure. Please provide 32bytes of salt");
			return 12;
		}
		free(salt_encoded);

	} else {
	}

/*    printf("Salt is: ");*/
	for(count=0; count < 32; count++) {
/*        printf("%" PRIu8" ", salt[count]);*/
	}
/*    printf("Done!\n");*/

	/* Generate the derived keys. */
	for(count=0; count < 64; count++)
		dk[count] = 0;
	//NOTA: il dk sono 64 byte: 32 sono la chiave, altri 32 un hmac!
	if (crypto_scrypt(passwd, (size_t)4, salt, 32, N, r, p, dk, 64))
		return (3);
	free(salt);
	be64_encode(dk, 32, &dk_be64, 0);
	printf("%s", dk_be64);
	free(dk_be64);
	return 0;
}
