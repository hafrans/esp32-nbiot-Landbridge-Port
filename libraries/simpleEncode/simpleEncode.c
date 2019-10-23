#include <string.h>

#include "simpleEncode.h"

//modified on 2017/4/2 by Steven Lian steven.lian@gmail.com


/*
  How to complile in linux:
  gcc -c -fPIC simpleEncode.c
  gcc -shared -fPIC -o simpleEncode.so simpleEncode.o
  How to use in python:
  from ctypes import *
  import os
  cLib=cdll.LoadLibrary(os.getcwd()+'/simpleEncode.so')
  str1="1111111111"
  s2=create_string_buffer(len(str1)*2+10)
  cLib.encode_data(str1,s2)
  str2=s2.value
  s3=create_string_buffer(len(str2)+10)
  cLib.decode_data(str2,s3)
  str3=s3.value
*/

#define ROTATE_LEFT(x, s, n) ((x) << (n)) | ((x) >> ((s) - (n)))
#define ROTATE_RIGHT(x, s, n) ((x) >> (n)) | ((x) << ((s) - (n)))

// base64 part
/*
   Copyright (c) 2003 Apple Computer, Inc. All rights reserved.

   @APPLE_LICENSE_HEADER_START@

   Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.

   This file contains Original Code and/or Modifications of Original Code
   as defined in and that are subject to the Apple Public Source License
   Version 2.0 (the 'License'). You may not use this file except in
   compliance with the License. Please obtain a copy of the License at
   http://www.opensource.apple.com/apsl/ and read it before using this
   file.

   The Original Code and all software distributed under the License are
   distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
   EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
   INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
   Please see the License for the specific language governing rights and
   limitations under the License.

   @APPLE_LICENSE_HEADER_END@
*/
/* ====================================================================
   Copyright (c) 1995-1999 The Apache Group.  All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.

   3. All advertising materials mentioning features or use of this
      software must display the following acknowledgment:
      "This product includes software developed by the Apache Group
      for use in the Apache HTTP server project (http://www.apache.org/)."

   4. The names "Apache Server" and "Apache Group" must not be used to
      endorse or promote products derived from this software without
      prior written permission. For written permission, please contact
      apache@apache.org.

   5. Products derived from this software may not be called "Apache"
      nor may "Apache" appear in their names without prior written
      permission of the Apache Group.

   6. Redistributions of any form whatsoever must retain the following
      acknowledgment:
      "This product includes software developed by the Apache Group
      for use in the Apache HTTP server project (http://www.apache.org/)."

   THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
   EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
   ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
   OF THE POSSIBILITY OF SUCH DAMAGE.
   ====================================================================

   This software consists of voluntary contributions made by many
   individuals on behalf of the Apache Group and was originally based
   on public domain software written at the National Center for
   Supercomputing Applications, University of Illinois, Urbana-Champaign.
   For more information on the Apache Group and the Apache HTTP server
   project, please see <http://www.apache.org/>.

*/

/* Base64 encoder/decoder. Originally Apache file ap_base64.c
*/


/* aaaack but it's fast and const should make it shared text page. */
static const unsigned char pr2six[256] =
{
  /* ASCII table */
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
  64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
  64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

int Base64decode_len(const char *bufcoded)
{
  int nbytesdecoded;
  //register const unsigned char *bufin;
  //register int nprbytes;
  const unsigned char *bufin;
  int nprbytes;

  bufin = (const unsigned char *) bufcoded;
  while (pr2six[*(bufin++)] <= 63);

  nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
  nbytesdecoded = ((nprbytes + 3) / 4) * 3;

  return nbytesdecoded + 1;
}

int Base64decode(char *bufplain, const char *bufcoded)
{
  int nbytesdecoded;
  const unsigned char *bufin;
  unsigned char *bufout;
  int nprbytes;

  bufin = (const unsigned char *) bufcoded;
  while (pr2six[*(bufin++)] <= 63);
  nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
  nbytesdecoded = ((nprbytes + 3) / 4) * 3;

  bufout = (unsigned char *) bufplain;
  bufin = (const unsigned char *) bufcoded;

  while (nprbytes > 4) {
    *(bufout++) =
      (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    *(bufout++) =
      (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    *(bufout++) =
      (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    bufin += 4;
    nprbytes -= 4;
  }

  /* Note: (nprbytes == 1) would be an error, so just ingore that case */
  if (nprbytes > 1) {
    *(bufout++) =
      (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
  }
  if (nprbytes > 2) {
    *(bufout++) =
      (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
  }
  if (nprbytes > 3) {
    *(bufout++) =
      (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
  }

  *(bufout++) = '\0';
  nbytesdecoded -= (4 - nprbytes) & 3;
  return nbytesdecoded;
}

static const char basis_64[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64encode_len(int len)
{
  return ((len + 2) / 3 * 4) + 1;
}

int Base64encode(char *encoded, const char *string, int len)
{
  int i;
  char *p;

  p = encoded;
  for (i = 0; i < len - 2; i += 3) {
    *p++ = basis_64[(string[i] >> 2) & 0x3F];
    *p++ = basis_64[((string[i] & 0x3) << 4) |
                    ((int) (string[i + 1] & 0xF0) >> 4)];
    *p++ = basis_64[((string[i + 1] & 0xF) << 2) |
                    ((int) (string[i + 2] & 0xC0) >> 6)];
    *p++ = basis_64[string[i + 2] & 0x3F];
  }
  if (i < len) {
    *p++ = basis_64[(string[i] >> 2) & 0x3F];
    if (i == (len - 1)) {
      *p++ = basis_64[((string[i] & 0x3) << 4)];
      *p++ = '=';
    }
    else {
      *p++ = basis_64[((string[i] & 0x3) << 4) |
                      ((int) (string[i + 1] & 0xF0) >> 4)];
      *p++ = basis_64[((string[i + 1] & 0xF) << 2)];
    }
    *p++ = '=';
  }

  *p++ = '\0';
  return p - encoded;
}

//base64 end

static unsigned short anSaltListSimple[CONST_MAXSALT_SIMPLELEN + 1] = \
{ 27169, 40118, 15746, 46781, 24687, 24123, 47311, 59611, 18160, 45475, 50579, 43737, 4781, \
  25186, 20206, 17980, 45823, 59517, 52886, 18223, 40892, 39343, 44800, 63708, 29754, 35217, \
  40416, 11340, 27150, 53350, 47583, 28213, 10210, 50380, 51411, 58484, 39959, 17223, 20773, \
  35736, 10074, 30263, 32031, 61941, 58707, 34553, 39676, 31879, 35627, 22845, 14821, 39610, \
  47017, 14078, 44191, 26197, 53273, 6856, 40221, 27320, 48440, 29960, 5534, 41934, 45138, \
  32004, 20489, 929, 46244, 50484, 23904, 35713, 10679, 34919, 32634, 25969, 60846, 63039, \
  21902, 29667, 57289, 24611, 9723, 25560, 49950, 21004, 24033, 17802, 56595, 47018, 37543, \
  18319, 51295, 47608, 38813, 41381, 9342, 14593, 3305, 40749, 28682, 14670, 22975, 50454, \
  44675, 49047, 31683, 42053, 37760, 18555, 15989, 6479, 45929, 26445, 6048, 25906, 15145, \
  4550, 33112, 15218, 31667, 46126, 43391, 56453, 57349, 39364, 7036, 50021
};

int random_data()
{
  //    struct timeval t_val;
  int nT1;
  //    gettimeofday(&t_val, NULL);
  //srand( (unsigned)time( NULL ) );
  //srand();
  nT1 = rand() % 60000;
  //nT1=random(100,60000);
  return nT1;
};


static const char bcdCodeTable[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

int bcd_encode(unsigned char *strOut, unsigned char *strIn, int nLen)
{
  //covert a data to BCD code
  int rtn = 0;
  int i;
  for (i = 0; i < nLen; i++) {
    strOut[rtn] = bcdCodeTable[(strIn[i] >> 4) & 0xF];
    rtn++;
    strOut[rtn] = bcdCodeTable[strIn[i] & 0xF];
    rtn++;
  }
  strOut[rtn] = 0;
  return rtn;
}

int bcd2int(char chIn)
{
  int rtn;
  if (chIn >= '0' && chIn <= '9') {
    rtn = chIn - '0';
  }
  else if (chIn >= 'A' && chIn <= 'F') {
    rtn = chIn - 'A' + 10;

  }
  else if (chIn >= 'a' && chIn <= 'f') {
    rtn = chIn - 'a' + 10;

  }
  else {
    rtn = 0;
  }
  return rtn;
}


int bcd_decode(unsigned char *strOut, unsigned char *strIn, int nLen)
{
  //covert a data to BCD code
  int rtn = 0;
  int i;
  for (i = 0; i < nLen; i += 2) {
    char nT1;
    nT1 = bcd2int(strIn[i]);
    nT1 <<= 4;
    nT1 += bcd2int(strIn[i + 1]);
    strOut[rtn] = nT1;
    rtn++;
  }
  strOut[rtn] = 0;
  return rtn;

}


int simple_encode_data(unsigned char*strOut, unsigned char *strIn, int nLen)
{
  int rtn = 0;
  int i, j, k, m;
  int nT1, nAscII, nXorSeed, nSalt, nSaltLen;
  short n;
  //unsigned char chT;
  j = 0;
  k = 2;
  nSaltLen = CONST_MAXSALT_SIMPLELEN;

  nT1 = random_data();
  strOut[0] = (unsigned char) (nT1 >> 8) & 0xFF;
  //chT = strOut[0];
  strOut[1] = (unsigned char) (nT1) & 0xFF;
  //chT = strOut[1];

  m = nT1 % (nSaltLen);
  for (i = 0; i < nLen; i++) {
    nAscII = strIn[i];
    //printf("\n%d \n",nAscII);
    nSalt = anSaltListSimple[m] & 0xFF;

    // shift salt
    n = anSaltListSimple[m] & 0x7;
    nSalt = ROTATE_LEFT(nSalt, 8, n);

    nXorSeed = strOut[j] ^ nSalt;
    //printf("\n%d \n",nXorSeed);
    nT1 = nAscII ^ nXorSeed;
    strOut[k] = nT1;
    k++;
    j++;
    if (j >= 2) {
      j = 0;
    }
    m++;
    if (m >= nSaltLen) {
      m = 0;
    }
  }
  rtn = nLen + 2;
  return rtn;
};


int simple_decode_data(unsigned char *strOut, unsigned char *strIn, int nLen)
{
  int rtn = 0;
  int i, j, k, m;
  int nT1, nAscII, nXorSeed, nSalt, nSaltLen;
  int nSeed;
  short n;
  //unsigned char chT;
  j = 0;
  k = 0;
  nSaltLen = CONST_MAXSALT_SIMPLELEN;
  //chT = strIn[0];
  nSeed = strIn[0] << 8;
  //chT = strIn[1];
  nSeed |= strIn[1];

  m = nSeed % (nSaltLen);
  for (i = 2; i < nLen; i++) {
    nAscII = strIn[i];
    nSalt = anSaltListSimple[m] & 0xFF;

    // shift salt
    n = anSaltListSimple[m] & 0x7;
    nSalt = ROTATE_LEFT(nSalt, 8, n);

    nXorSeed = strIn[j] ^ nSalt;
    //printf("\n%d \n",nXorSeed);
    nT1 = nAscII ^ nXorSeed;
    //printf("  %d ",nT1);
    strOut[k] = nT1;
    k++;
    j++;
    if (j >= 2) {
      j = 0;
    }
    m++;
    if (m >= nSaltLen) {
      m = 0;
    }
  }
  rtn = nLen - 2;
  return rtn;
}


int simpleB64_encode(char *strOut,char *strIn,int nLen)
{
  int rtn=0;
  //int i;
  rtn=simple_encode_data(strOut,strIn,nLen);
  /*
  printf("\n\nafter simple_encode_data:\n");
  for (i=0;i<rtn;i++){
    printf("%02X ",strOut[i]);
  }
  */
  memcpy(strIn,strOut,rtn);
  rtn=Base64encode(strOut,strIn,rtn);
  /*
  printf("\n\nafter Base64encode: [len]:%d [string]:[%s] \n",strlen(strOut),strOut);
  for (i=0;i<rtn;i++){
    printf("%02X ",strOut[i]);
  }
  */
  return rtn;
}

int simpleB64_decode(char *strOut, char *strIn,int nLen)
{
  int rtn=0;

  //int i;
  //printf("\n\nstrIn: [len]:%d [string]:[%s] ",strlen(strIn),strIn);

  rtn=Base64decode(strOut,strIn);

  //printf("\n\nafter Base64decode:\n");
  //for (i=0;i<rtn;i++){
  //  printf("%02X ",strOut[i]);
  //}

  memcpy(strIn,strOut,rtn);
  rtn=simple_decode_data(strOut,strIn,rtn);

  *(strOut+rtn)=0;  
  return rtn;
}




