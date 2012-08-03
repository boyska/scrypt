#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base64.h"

int main(int argc, char *argv[]) {
	char *encoded = "bGVhc3VyZS4=";
	if(argc > 1)
		encoded = argv[1];
	uint8_t *out = NULL;
	int len;
	char *reenc = NULL;
	len = be64_decode(encoded, &out);
	be64_encode((uint8_t*)out, len, &reenc, 0);
	printf("%s\n", out);
	fprintf(stderr, "%s\n", reenc);

	return 0;
}
