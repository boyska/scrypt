
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

//TODO: separate into a clean calculate_pass(passwd, passwdlen, N, r, p, salt, saltlen) <-- this is crypto_scrypt !!
//TODO: design a useful interface!
void usage(char *argv[]) {
	fprintf(stderr, "Usage: %s [logN r p] [saltfile]", argv[0]);
}

int main(int argc, char *argv[]) {
	int logN;
	uint64_t N;
	uint32_t r, p;
	size_t maxmem = 0;
	double maxmemfrac = 0.5;
	double maxtime = 10.0;
	int rc;
	uint8_t salt[32];
	uint8_t dk[64]; //derived key
	uint8_t passwd[4];
	char *saltfile = "salt";
	FILE *f;

	int count;
	for(count=0; count < 4; count++)
		passwd[count]=(uint8_t)count;

	/* Pick values for N, r, p. */
	if(argc == 1) {
		if ((rc = pickparams(maxmem, maxmemfrac, maxtime,
			&logN, &r, &p)) != 0)
			return (rc);
	} else if (argc >= 4) {
		sscanf(argv[1], "%d", &logN);
		sscanf(argv[2], "%" SCNu32, &r);
		sscanf(argv[3], "%" SCNu32, &p);
	} else {
		usage(argv);
		return 127;
	}
	N = (uint64_t)(1) << logN;

	fprintf(stderr, "Param:\t%d=>%lu %" PRIu32 " %" PRIu32 "\n", logN, N, r, p);
	if(argc >= 5) {
		printf("Reading saltfile...\n");
		sscanf(argv[4], "%ms", &saltfile);
	}
	printf("Salt file is: %s\n", saltfile);
	if(argc<2) { //TODO: change in exists()
		if ((rc = getsalt(salt)) != 0)
			return (rc);
		f = fopen("salt", "w");
		fwrite(salt, sizeof(uint8_t), 32, f);
		fclose(f);
	} else {
/*        printf("Loading salt...\n");*/
		f = fopen("salt", "r");
		if(fread(salt, sizeof(uint8_t), 32, f) != 32) {
			fprintf(stderr, "Error reading salt\n");
			return 1;
		}
		fclose(f);
	}

/*    printf("Salt is: ");*/
	for(count=0; count < 32; count++) {
/*        printf("%" PRIu8" ", salt[count]);*/
	}
/*    printf("Done!\n");*/

	fprintf(stderr, "Param: %d %lu %d %d\n", logN, N, r, p);
	/* Generate the derived keys. */
	for(count=0; count < 64; count++)
		dk[count] = 0;
	//NOTA: il dk sono 64 byte: 32 sono la chiave, altri 32 un hmac!
	if (crypto_scrypt(passwd, (size_t)4, salt, 32, N, r, p, dk, 64))
		return (3);
	f = fopen("dk", "w");
	fwrite(dk, sizeof(uint8_t), 32, f); //il resto non ci serve!
	fclose(f);
	printf("derived key is...\n");
	for(count=0; count < 32; count++)
		printf("%02x ", dk[count]);
	printf("\n\n");
	printf("\nDone! security is now f***ed up\n");
	return 0;
}
