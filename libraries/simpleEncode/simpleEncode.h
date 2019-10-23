#pragma once
#ifndef SIMPLEENCODE_H
#define SIMPLEENCODE_H 

#define CONST_MAXSALT_SIMPLELEN 128
int Base64decode_len(const char *bufcoded);
int Base64decode(char *bufplain, const char *bufcoded);
int Base64encode_len(int len);
int Base64encode(char *encoded, const char *string, int len);

int bcd_encode(unsigned char *strOut,unsigned char *strIn,int nLen);
int bcd_decode(unsigned char *strOut,unsigned char *strIn,int nLen);

//加密编码
int random_data();
int simple_encode_data(unsigned char *strOut,unsigned char *strIn,int nLen);
int simple_decode_data(unsigned char *strOut,unsigned char *strIn,int nLen);
//带加密的base64编码
int simpleB64_encode(char *strOut, char *strIn,int nLen);
int simpleB64_decode(char *strOut, char *strIn,int nLen);

#endif
