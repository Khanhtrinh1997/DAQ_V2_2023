/*
 * Dataconverter.h
 *
 *  Created on: Nov 23, 2016
 *      Author: thaoptb2
 */
//#include "Header.h"
#ifndef DRIVERFUNC_H_
#define DRIVERFUNC_H_

void InitCrc32(CRC_Type *base, uint32_t seed);
void hex2char(unsigned char *src, uint16_t hexnum, uint8_t len);
uint8_t Checksum8(unsigned char *src,uint32_t len);
uint16_t Checksum16(unsigned char *src,uint32_t len);
int8_t char2hex(unsigned char c);
float hex2float(unsigned char* src,uint32_t startbyte);
extern uint16_t hex2byte2(unsigned char* src,uint32_t startbyte);
extern uint32_t hex2byte4(unsigned char* src,uint32_t startbyte);
extern uint8_t hex2byte(unsigned char* src,uint32_t startbyte);
extern int16_t ihex2byte2(unsigned char* src,uint32_t startbyte);
extern int16_t iihex2byte2(unsigned char* src,uint32_t startbyte);
int16_t iiihex2byte2(unsigned char* src,uint32_t startbyte);


extern void u82char(unsigned char *src, uint8_t value, uint8_t len);
extern void u162char(unsigned char *src, uint16_t value, uint8_t len);
extern void u322char(unsigned char *src, uint32_t value, uint8_t len);

extern uint8_t htmlTextExtract2(const char* pcExtString, int32_t* pcDst, uint8_t keywordLen);
extern uint8_t htmlTextExtract(const char* pcExtString, uint32_t* pcDst, uint8_t keywordLen);
extern bool htmlIsValidHexDigit(const char cDigit);
extern unsigned char htmlHexDigit(const char cDigit);
extern bool htmlDecodeHexEscape(const char *pcEncoded, char *pcDecoded);
extern uint32_t htmlDecodeFormString(const  char *pcEncoded, char *pcDecoded,uint32_t ulLen);
void RectID_Calculation(char* dst, unsigned char* src);

uint32_t revertIP(uint32_t ipaddr);

#endif /* DRIVERFUNC_H_ */
