/* MSNP11 Challenge Handler v2
 * Based on the C++ implementation. 
 * This implementation works on big endian systems as well.
 * Compile: 'gcc chl.c -o chl -lcrypto -Wall'
 *
 * Licensed for distribution under the GPL version 2
 * Copyright 2005 Sanoi <sanoix@gmail.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>
#define BUFSIZE 256

int isBigEndian(void)
{
  short int word = 0x0100;
  char *byte = (char *)&word;
  return(byte[0]);
}

unsigned int swapInt(unsigned int dw)
{
  unsigned int tmp;
  tmp =  (dw & 0x000000FF);
  tmp = ((dw & 0x0000FF00) >> 0x08) | (tmp << 0x08);
  tmp = ((dw & 0x00FF0000) >> 0x10) | (tmp << 0x08);
  tmp = ((dw & 0xFF000000) >> 0x18) | (tmp << 0x08);
  return(tmp);
}

void MSN_handle_chl(char *input, char *output)
{
  char *productKey = "PK}_A_0N_K%O?A9S",
       *productID  = "PROD0114ES4Z%Q5W",
       *hexChars   = "0123456789abcdef",
       buf[BUFSIZE];
  unsigned char md5Hash[16], *newHash;
  unsigned int *md5Parts, *chlStringParts, newHashParts[5];
  
  long long nHigh=0, nLow=0;
  
  int i, bigEndian;

  /* Determine our endianess */
  bigEndian = isBigEndian();

  /* Create the MD5 hash */
  snprintf(buf, BUFSIZE-1, "%s%s", input, productKey);
  MD5((unsigned char *)buf, strlen(buf), md5Hash);

  /* Split it into four integers */
  md5Parts = (unsigned int *)md5Hash;
  for(i=0; i<4; i++)
  {  
    /* check for endianess */
    if(bigEndian)
      md5Parts[i] = swapInt(md5Parts[i]);
    
    /* & each integer with 0x7FFFFFFF          */
    /* and save one unmodified array for later */
    newHashParts[i] = md5Parts[i];
    md5Parts[i] &= 0x7FFFFFFF;
  }
  
  /* make a new string and pad with '0' */
  snprintf(buf, BUFSIZE-5, "%s%s", input, productID);
  i = strlen(buf);
  memset(&buf[i], '0', 8 - (i % 8));
  buf[i + (8 - (i % 8))]='\0';
  
  /* split into integers */
  chlStringParts = (unsigned int *)buf;
  
  /* this is magic */
  for (i=0; i<(strlen(buf)/4)-1; i+=2)
  {
    long long temp;

    if(bigEndian)
    {
      chlStringParts[i]   = swapInt(chlStringParts[i]);
      chlStringParts[i+1] = swapInt(chlStringParts[i+1]);
    }

    temp=(md5Parts[0] * (((0x0E79A9C1 * (long long)chlStringParts[i]) % 0x7FFFFFFF)+nHigh) + md5Parts[1])%0x7FFFFFFF;
    nHigh=(md5Parts[2] * (((long long)chlStringParts[i+1]+temp) % 0x7FFFFFFF) + md5Parts[3]) % 0x7FFFFFFF;
    nLow=nLow + nHigh + temp;
  }
  nHigh=(nHigh+md5Parts[1]) % 0x7FFFFFFF;
  nLow=(nLow+md5Parts[3]) % 0x7FFFFFFF;
  
  newHashParts[0]^=nHigh;
  newHashParts[1]^=nLow;
  newHashParts[2]^=nHigh;
  newHashParts[3]^=nLow;
  
  /* swap more bytes if big endian */
  for(i=0; i<4 && bigEndian; i++)
    newHashParts[i] = swapInt(newHashParts[i]); 
  
  /* make a string of the parts */
  newHash = (unsigned char *)newHashParts;
  
  /* convert to hexadecimal */
  for (i=0; i<16; i++)
  {
    output[i*2]=hexChars[(newHash[i]>>4)&0xF];
    output[(i*2)+1]=hexChars[newHash[i]&0xF];
  }
  
  output[32]='\0';
}

