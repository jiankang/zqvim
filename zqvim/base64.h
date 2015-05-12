#ifndef _BASE64_INCLUDE__H__
#define _BASE64_INCLUDE__H__

#pragma once

unsigned long GetBase64EncodeSize(unsigned long src_len);
unsigned long GetBase64DecodeSize(unsigned long src_len);
unsigned long Base64Encode(char * base64code, const char * src, unsigned long src_len = 0);
unsigned long Base64Encode1(char * base64code, const char * src, unsigned long src_len = 0);
unsigned long Base64Decode(char * buf, const char * base64code, unsigned long src_len = 0);


#define B0(a) (a & 0xFF)
#define B1(a) (a >> 8 & 0xFF)
#define B2(a) (a >> 16 & 0xFF)
#define B3(a) (a >> 24 & 0xFF)


#endif // #ifndef _BASE64_INCLUDE__H__


