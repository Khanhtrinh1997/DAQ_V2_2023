/*
 * Dataconverter.c
 *
 *  Created on: Nov 24, 2016
 *      Author: thaoptb2
 */

#include "Header.h"

/*!
 * @brief Init for CRC-32.
 * @details Init CRC peripheral module for CRC-32 protocol.
 *          width=32 poly=0x04c11db7 init=0xffffffff refin=true refout=true xorout=0xffffffff check=0xcbf43926
 *          name="CRC-32"
 *          http://reveng.sourceforge.net/crc-catalogue/
 */
void InitCrc32(CRC_Type *base, uint32_t seed)
{
    crc_config_t config;

    config.polynomial = 0x04C11DB7U;
    config.seed = seed;
    config.reflectIn = true;
    config.reflectOut = true;
    config.complementChecksum = true;
    config.crcBits = kCrcBits32;
    config.crcResult = kCrcFinalChecksum;

    CRC_Init(base, &config);
}
//void hex2char(unsigned char *src, uint16_t hexnum, uint8_t len);
// Tinh  cho cau lenh tu zte voi "len" phan tu
uint8_t Checksum8(unsigned char *src,uint32_t len)
{
	uint32_t i = 0;
	uint8_t u8val=0;
	if (len < 1) u8val = 0xff;
	else
	{
		for (i = 0; i < len; i++)
		{
	//		val += datBuffer[i];
			u8val += src[i+1];
		}
		u8val = ~u8val;
		u8val = (u8val + 1);
	}
	return u8val;
}

uint16_t Checksum16(unsigned char *src,uint32_t len)
{
	uint32_t i = 0;
	uint16_t u16val=0;
	if (len <= 5) u16val = 0xffff;
	else
	{
		for (i = 0; i < (len - 5); i++)
		{
	//		val += datBuffer[i];
			u16val += src[i];
		}
		u16val = ~u16val;
		u16val = (u16val + 0x7f);
	}
	return u16val;
}
// Ham chuyen ky tu hex sang ma ascii tuong ung
void hex2char(unsigned char *src, uint16_t hexnum, uint8_t len)
{
	uint8_t n=0,i=0;
	unsigned char hexchar[4];
	unsigned char *hexchar_ptr;
	hexchar_ptr = &hexchar[0];

	for (i = 0; i < len; i++)
	{
		n = 4*(len - 1 - i);
		hexchar[i] = (hexnum & (uint16_t)(0x000f << n)) >> n;
	}
	for (i = 0; i < len; i++)
	{
		if (hexchar[i] <= 0x09) hexchar[i] += 0x30;
		else if ((hexchar[i] >= 0x0a) && (hexchar[i] <= 0x0f)) hexchar[i] += 0x37;
	}

	i = 0;
	while (i < len)
	{
		i++;
		*src++ = *hexchar_ptr++;
	}
}
// chuyen ky tu sang so hex
int8_t char2hex(unsigned char c)
{
	int8_t hexNum;
	if ((c >= '0') && (c <= '9')) hexNum = c - 0x30;
	else if ((c >='A') && (c <= 'F')) hexNum = c - 0x37;
        else if (c == 0x20) hexNum = 0x20;
//	else hexNum = -1;

	return hexNum;
}
// chuyen tu dang memory sang so float
float hex2float(unsigned char* src,uint32_t startbyte)
{
	uint32_t hex32Num;

	unsigned char datBuffer[8];
	unsigned char* datBuffer_ptr = (unsigned char*)&datBuffer[0];
	int8_t dst[8];
	uint8_t i = 0;
	float Val;

	*(datBuffer_ptr    ) = *(src + startbyte + 6);
	*(datBuffer_ptr + 1) = *(src + startbyte + 7);
	*(datBuffer_ptr + 2) = *(src + startbyte + 4);
	*(datBuffer_ptr + 3) = *(src + startbyte + 5);
	*(datBuffer_ptr + 4) = *(src + startbyte + 2);
	*(datBuffer_ptr + 5) = *(src + startbyte + 3);
	*(datBuffer_ptr + 6) = *(src + startbyte);
	*(datBuffer_ptr + 7) = *(src + startbyte + 1);

	for (i = 0; i < 8; i++)
	{
		dst[i] = char2hex(datBuffer[i]);
	}
	hex32Num = 	(dst[0] << 28) |
				(dst[1] << 24) |
				(dst[2] << 20) |
				(dst[3] << 16) |
				(dst[4] << 12) |
				(dst[5] << 8) |
				(dst[6] << 4) |
				(dst[7]) ;

	memcpy(&Val,&hex32Num,4);

	return (Val);
}
uint8_t hex2byte(unsigned char* src,uint32_t startbyte)
{
	int8_t dst[2]={0,0};
	uint8_t i = 0;
	uint8_t Val;

	for (i = 0; i < 2; i++)
	{
		dst[i] = char2hex(src[i+startbyte]);
	}
        if (dst[0] != 0x20)
        {
          Val = 	(dst[0] << 4) |
			(dst[1]) ;
        }
        else
        {
          Val = 0x20;
        } 
	return (Val);
}
//--------------
int16_t iiihex2byte2(unsigned char* src,uint32_t startbyte)
{
//	unsigned char datBuffer[4];
//	unsigned char* datBuffer_ptr = (unsigned char*)&datBuffer[0];

	int8_t dst[4];
	uint8_t i = 0;
	int16_t Val = 0;

//	while (i < 4)
//	{
//		i++;
//		*datBuffer_ptr++ = *src + startbyte++;
//	}

	for (i = 0; i < 4; i++)
	{
		dst[i] = char2hex(src[i+startbyte]);
//		dst[i] = char2hex(datBuffer[i]);
	}
	Val = 	(dst[0] << 12) |
			(dst[1] << 8)  |
			(dst[2] << 4)  |
			(dst[3]) ;
        
        if((Val&0x8000)== 0x8000){
          Val =(~Val)+1; 
          Val = Val*(-1);
        }

	return (Val);
}
//--------------
int16_t iihex2byte2(unsigned char* src,uint32_t startbyte)
{
//	unsigned char datBuffer[4];
//	unsigned char* datBuffer_ptr = (unsigned char*)&datBuffer[0];

	int8_t dst[4];
	uint8_t i = 0;
	int16_t Val = 0;

//	while (i < 4)
//	{
//		i++;
//		*datBuffer_ptr++ = *src + startbyte++;
//	}

	for (i = 0; i < 4; i++)
	{
		dst[i] = char2hex(src[i+startbyte]);
//		dst[i] = char2hex(datBuffer[i]);
	}
	Val = 	(dst[0] << 12) |
			(dst[1] << 8)  |
			(dst[2] << 4)  |
			(dst[3]) ;
        
        if((Val&0x8000)== 0x8000){
          Val =(~Val)+1;
          
          if(Val>=150*100){
            Val = 256*100 - Val;
          }else{
            Val = Val*(-1);
          }
          
        }

	return (Val);
}
//---------------------------------------------------------------
int16_t ihex2byte2(unsigned char* src,uint32_t startbyte)
{
//	unsigned char datBuffer[4];
//	unsigned char* datBuffer_ptr = (unsigned char*)&datBuffer[0];

	int8_t dst[4];
	uint8_t i = 0;
	int16_t Val = 0;

//	while (i < 4)
//	{
//		i++;
//		*datBuffer_ptr++ = *src + startbyte++;
//	}

	for (i = 0; i < 4; i++)
	{
		dst[i] = char2hex(src[i+startbyte]);
//		dst[i] = char2hex(datBuffer[i]);
	}
	Val = 	(dst[0] << 12) |
			(dst[1] << 8)  |
			(dst[2] << 4)  |
			(dst[3]) ;
        if(Val>=211*100){
          Val = (256*100-Val);
          Val = Val*(-1);
        }

	return (Val);
}

//----------------------------------------------------------------
uint16_t hex2byte2(unsigned char* src,uint32_t startbyte)
{
//	unsigned char datBuffer[4];
//	unsigned char* datBuffer_ptr = (unsigned char*)&datBuffer[0];

	int8_t dst[4];
	uint8_t i = 0;
	uint16_t Val = 0;

//	while (i < 4)
//	{
//		i++;
//		*datBuffer_ptr++ = *src + startbyte++;
//	}

	for (i = 0; i < 4; i++)
	{
		dst[i] = char2hex(src[i+startbyte]);
//		dst[i] = char2hex(datBuffer[i]);
	}
	Val = 	(dst[0] << 12) |
			(dst[1] << 8)  |
			(dst[2] << 4)  |
			(dst[3]) ;

	return (Val);
}

uint32_t hex2byte4(unsigned char* src,uint32_t startbyte)
{
	int8_t dst[8];
	uint8_t i = 0;
	uint32_t u32Val = 0;

	for (i = 0; i < 8; i++)
	{
		dst[i] = char2hex(src[i+startbyte]);
	}
//	Val = 	(dst[0] << 12) |
//			(dst[1] << 8)  |
//			(dst[2] << 4)  |
//			(dst[3]) ;

	u32Val = 	(dst[0] << 28) |
				(dst[1] << 24) |
				(dst[2] << 20) |
				(dst[3] << 16) |
				(dst[4] << 12) |
				(dst[5] << 8)  |
				(dst[6] << 4)  |
				(dst[7]) ;

	return (u32Val);
}


void u82char(unsigned char *src, uint8_t value, uint8_t len)
{
	uint8_t n=0,
			i=0;

	unsigned char temp[2];
	unsigned char *temp_ptr = &temp[0];

	for (i = 0; i < len; i++)
	{
		n = 4*(len - 1 - i);
		temp[i] = (value & (uint8_t)(0x0f << n)) >> n;
	}
	for (i = 0; i < len; i++)
	{
		if (temp[i] <= 0x09) temp[i] += 0x30;
		else if ((temp[i] >= 0x0a) && (temp[i] <= 0x0f)) temp[i] += 0x37;
	}

	i = 0;
	while (i < len)
	{
		i++;
		*src++ = *temp_ptr++;
	}
}

void u162char(unsigned char *src, uint16_t value, uint8_t len)
{
	uint8_t n=0,
			i=0;

	unsigned char temp[4];
	unsigned char *temp_ptr = &temp[0];

	for (i = 0; i < len; i++)
	{
		n = 4*(len - 1 - i);
		temp[i] = (value & (uint16_t)(0x000f << n)) >> n;
	}
	for (i = 0; i < len; i++)
	{
		if (temp[i] <= 0x09) temp[i] += 0x30;
		else if ((temp[i] >= 0x0a) && (temp[i] <= 0x0f)) temp[i] += 0x37;
	}

	i = 0;
	while (i < len)
	{
		i++;
		*src++ = *temp_ptr++;
	}
}

void u322char(unsigned char *src, uint32_t value, uint8_t len)
{
	uint8_t n=0,
			i=0;

	unsigned char temp[8];
	unsigned char *temp_ptr = &temp[0];

	for (i = 0; i < len; i++)
	{
		n = 4*(len - 1 - i);
		temp[i] = (value & (uint32_t)(0x0000000f << n)) >> n;
	}
	for (i = 0; i < len; i++)
	{
		if (temp[i] <= 0x09) temp[i] += 0x30;
		else if ((temp[i] >= 0x0a) && (temp[i] <= 0x0f)) temp[i] += 0x37;
	}

	i = 0;
	while (i < len)
	{
		i++;
		*src++ = *temp_ptr++;
	}
}

bool htmlIsValidHexDigit(const char cDigit)
{
    if(((cDigit >= '0') && (cDigit <= '9')) ||
       ((cDigit >= 'a') && (cDigit <= 'f')) ||
       ((cDigit >= 'A') && (cDigit <= 'F')))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

unsigned char htmlHexDigit(const char cDigit)
{
    if((cDigit >= '0') && (cDigit <= '9'))
    {
        return(cDigit - '0');
    }
    else
    {
        if((cDigit >= 'a') && (cDigit <= 'f'))
        {
            return((cDigit - 'a') + 10);
        }
        else
        {
            if((cDigit >= 'A') && (cDigit <= 'F'))
            {
                return((cDigit - 'A') + 10);
            }
        }
    }

    //
    // If we get here, we were passed an invalid hex digit so return 0xFF.
    //
    return(0xFF);
}

bool htmlDecodeHexEscape(const char *pcEncoded, char *pcDecoded)
{
    if((pcEncoded[0] != '%') || !htmlIsValidHexDigit(pcEncoded[1]) ||
       !htmlIsValidHexDigit(pcEncoded[2]))
    {
        return(false);
    }
    else
    {
        *pcDecoded = ((htmlHexDigit(pcEncoded[1]) * 16) +
        		htmlHexDigit(pcEncoded[2]));
        return(true);
    }
}

uint32_t htmlDecodeFormString(const  char *pcEncoded, char *pcDecoded,
                       uint32_t ulLen)
{
    uint32_t ulLoop;
    uint32_t ulCount;
    bool bValid;

    ulCount = 0;
    ulLoop = 0;

    //
    // Keep going until we run out of input or fill the output buffer.
    //
    while(pcEncoded[ulLoop] && (ulCount < (ulLen - 1)))
    {
        switch(pcEncoded[ulLoop])
        {
            //
            // '+' in the encoded data is decoded as a space.
            //
            case '+':
            {
                pcDecoded[ulCount++] = ' ';
                ulLoop++;
                break;
            }

            //
            // '%' in the encoded data indicates that the following 2
            // characters indicate the hex ASCII code of the decoded character.
            //
            case '%':
            {
                if(pcEncoded[ulLoop + 1] && pcEncoded[ulLoop + 2])
                {
                    //
                    // Decode the escape sequence.
                    //
                    bValid = htmlDecodeHexEscape(&pcEncoded[ulLoop],
                                                   &pcDecoded[ulCount]);

                    //
                    // If the escape sequence was valid, move to the next
                    // output character.
                    //
                    if(bValid)
                    {
                        ulCount++;
                    }

                    //
                    // Skip past the escape sequence in the encoded string.
                    //
                    ulLoop += 3;
                }
                else
                {
                    //
                    // We reached the end of the string partway through an
                    // escape sequence so just ignore it and return the number
                    // of decoded characters found so far.
                    //
                    pcDecoded[ulCount] = '\0';
                    return(ulCount);
                }
                break;
            }
            case '&':
			{
//				bFlag = 1;
				pcDecoded[ulCount] = '\0';
				return(ulCount);
//				break;
			}
            //
            // For all other characters, just copy the input to the output.
            //
            default:
            {
                pcDecoded[ulCount++] = pcEncoded[ulLoop++];
                break;
            }
        }
    }

    //
    // Terminate the string and return the number of characters we decoded.
    //
    pcDecoded[ulCount] = '\0';
    return(ulCount);
}


// tach lay gia tri nhan ve qua http tu pcExtString--> pcDst
// return: so chu so co trong gia tri nhan ve
uint8_t htmlTextExtract(const char* pcExtString, uint32_t* pcDst, uint8_t keywordLen)
{
  uint8_t len=0;
	uint32_t i = 0;
        uint32_t temp=0;
	const char *data;
	char cTextBuff[16]={0};
	char* cTextBuff_ptr = &cTextBuff[0];

	data = pcExtString;
	data += (keywordLen + 7);
	do
	{
		switch(data[i])
		{
				case 0:
				case '&':
					cTextBuff[i] = 0;
						break;
				case '+':
					cTextBuff[i] = ' ';
						break;
				default:
					cTextBuff[i] = data[i];
						len++;
						break;
		}

		if(cTextBuff[i] == 0)
		{
			break;
		}
		i++;
	}while(i < sizeof(cTextBuff));

	switch(len)
	{
		case 1:
		{
			temp = *cTextBuff_ptr - 0x30;
		} 	break;
		case 2:
		{
			temp = (*cTextBuff_ptr - 0x30) * 10 + (*(cTextBuff_ptr+1) - 0x30) ;
		} 	break;

		case 3:
		{
			temp = (*cTextBuff_ptr - 0x30) * 100 + (*(cTextBuff_ptr+1) - 0x30) * 10 + (*(cTextBuff_ptr+2) - 0x30)  ;
		} 	break;
		case 4:
		{
			temp = (*cTextBuff_ptr - 0x30) * 1000 + (*(cTextBuff_ptr+1) - 0x30) * 100 + (*(cTextBuff_ptr+2) - 0x30) * 10 + (*(cTextBuff_ptr+3) - 0x30)  ;
		} 	break;
		default:
			break;
	}
	*pcDst = temp;
//	strncpy(pcSrc,cTextBuff,16);
        return len;
}


uint8_t htmlTextExtract2(const char* pcExtString, int32_t* pcDst, uint8_t keywordLen)
{
  uint8_t len=0;
  uint8_t sign = 0;
	uint32_t i = 0;
        uint32_t temp=0;
	const char *data;
	char cTextBuff[16]={0};
	char* cTextBuff_ptr = &cTextBuff[0];

	data = pcExtString;
	data += (keywordLen + 7);
	do
	{
		switch(data[i])
		{
				case 0:
				case '&':
					cTextBuff[i] = 0;
						break;
				case '+':
					cTextBuff[i] = ' ';
						break;
				default:
					cTextBuff[i] = data[i];
						len++;
						break;
		}

		if(cTextBuff[i] == 0)
		{
			break;
		}
		i++;
	}while(i < sizeof(cTextBuff));
        
        if(cTextBuff[0] == '-')
        {
          sign = 1;
          switch(len)
          {
                  case 2:
                  {
                          temp = *(cTextBuff_ptr+1) - 0x30;
                  } 	break;
                  case 3:
                  {
                          temp = (*(cTextBuff_ptr+1) - 0x30) * 10 + (*(cTextBuff_ptr+2) - 0x30) ;
                  } 	break;

                  case 4:
                  {
                          temp = (*(cTextBuff_ptr+1) - 0x30) * 100 + (*(cTextBuff_ptr+2) - 0x30) * 10 + (*(cTextBuff_ptr+3) - 0x30)  ;
                  } 	break;
                  case 5:
                  {
                          temp = (*(cTextBuff_ptr+1) - 0x30) * 1000 + (*(cTextBuff_ptr+2) - 0x30) * 100 + (*(cTextBuff_ptr+3) - 0x30) * 10 + (*(cTextBuff_ptr+4) - 0x30)  ;
                  } 	break;
                  default:
                          break;
          }
          *pcDst = -temp;
          
        }
        else
        {
          sign = 0;
          switch(len)
          {
                  case 1:
                  {
                          temp = *cTextBuff_ptr - 0x30;
                  } 	break;
                  case 2:
                  {
                          temp = (*cTextBuff_ptr - 0x30) * 10 + (*(cTextBuff_ptr+1) - 0x30) ;
                  } 	break;

                  case 3:
                  {
                          temp = (*cTextBuff_ptr - 0x30) * 100 + (*(cTextBuff_ptr+1) - 0x30) * 10 + (*(cTextBuff_ptr+2) - 0x30)  ;
                  } 	break;
                  case 4:
                  {
                          temp = (*cTextBuff_ptr - 0x30) * 1000 + (*(cTextBuff_ptr+1) - 0x30) * 100 + (*(cTextBuff_ptr+2) - 0x30) * 10 + (*(cTextBuff_ptr+3) - 0x30)  ;
                  } 	break;  
                  case 5:
                  {
                          temp = (*cTextBuff_ptr - 0x30) * 10000 + (*(cTextBuff_ptr+1) - 0x30) * 1000 + (*(cTextBuff_ptr+2) - 0x30) * 100 + (*(cTextBuff_ptr+3) - 0x30)*10 + (*(cTextBuff_ptr+4) - 0x30)  ;
                  } 	break;
                  default:
                          break;
          }
          *pcDst = temp;
        }
//	strncpy(pcSrc,cTextBuff,16);
        return len;
}

uint32_t revertIP(uint32_t ipaddr)
{
	uint32_t val[4];
	uint32_t newip = 0;
	val[0]=(ipaddr & 0xff000000) >> 24;
    val[1]=	(ipaddr & 0x00ff0000) >> 16;
    val[2]=	(ipaddr & 0x0000ff00) >> 8;
    val[3]=	(ipaddr & 0x000000ff);

    newip = val[0] | (val[1] << 8) | (val[2] << 16) | (val [3] << 24);
	return newip;
}

void RectID_Calculation(char* dst, unsigned char* src)
{
	uint8_t i;
	int8_t rectID1[4];
	char buffer[32];
	char* buffer_ptr = (char*)&buffer[0];
	uint16_t rectID2;

	for (i = 0; i < 4; i++)
	{
		rectID1[i] = char2hex(src[i]);
	}

	for (i = 0; i < 4; i++)
	{
		if (rectID1[i] <= 0x09) rectID1[i] += 0x30;
		else if ((rectID1[i] >= 0x0a) && (rectID1[i] <= 0x0f)) rectID1[i] += 0x37;
	}

	rectID2 = hex2byte2(src,4);

	if (rectID2 < 10)
	{
		snprintf(buffer_ptr,18,"RectID:2%c%c%c%c%d%d%d%d%d",rectID1[0],rectID1[1],rectID1[2],rectID1[3],0,0,0,0,0);
		snprintf(&buffer[17],2,"%d",rectID2);
		buffer[18] = '\0';

	} else if ((rectID2 > 9) && (rectID2 < 100))
	{
		snprintf(buffer_ptr,18,"RectID:2%c%c%c%c%d%d%d%d%d",rectID1[0],rectID1[1],rectID1[2],rectID1[3],0,0,0,0,0);
		snprintf(&buffer[16],3,"%d",rectID2);
		buffer[18] = '\0';
	} else if ((rectID2 > 99) && (rectID2 < 1000))
	{
		snprintf(buffer_ptr,18,"RectID:2%c%c%c%c%d%d%d%d%d",rectID1[0],rectID1[1],rectID1[2],rectID1[3],0,0,0,0,0);
		snprintf(&buffer[15],4,"%d",rectID2);
		buffer[18] = '\0';
	} else if ((rectID2 > 999) && (rectID2 < 10000))
	{
		snprintf(buffer_ptr,18,"RectID:2%c%c%c%c%d%d%d%d%d",rectID1[0],rectID1[1],rectID1[2],rectID1[3],0,0,0,0,0);
		snprintf(&buffer[14],5,"%d",rectID2);
		buffer[18] = '\0';
	} else if ((rectID2 > 9999) && (rectID2 < 100000))
	{
		snprintf(buffer_ptr,18,"RectID:2%c%c%c%c%d%d%d%d%d",rectID1[0],rectID1[1],rectID1[2],rectID1[3],0,0,0,0,0);
		snprintf(&buffer[13],6,"%d",rectID2);
		buffer[18] = '\0';
	}


	buffer_ptr = &buffer[0];
	i = 0;
	while (i < 19)
	{
		i++;
		*dst++ = *buffer_ptr++;
	}
}