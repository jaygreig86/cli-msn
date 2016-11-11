/*
 MD5 hash function 
 by phased

*/

#include "md5.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

char *MD5Data(char *buf, unsigned int len, char *hash)
{
/*    int i;
    int status = 0; */

	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)buf, len);
	md5_finish(&state, digest);
	for (di = 0; di < 16; ++di)
	    sprintf(hex_output + di * 2, "%02x", digest[di]);
	snprintf(hash, strlen(hex_output)+1, "%s", hex_output); 
	return(hash);
}

