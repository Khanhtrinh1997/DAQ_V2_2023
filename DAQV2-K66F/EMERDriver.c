/*
 * EMER_driver.c
 *
 *  Created on: Nov 23, 2016
 *      Author: thaoptb2
 */

#include "Header.h"

//unsigned char g_setInfo[32];
//unsigned char g_getInfo[24];
tMESGState MESGState;


void setCommand(unsigned char *arrCmd,unsigned char *templateCmd, float setNum, bool type)
{
	uint8_t	i=0;
	unsigned char cmd[32];
	unsigned char* cmd_ptr = (unsigned char*)&cmd[0];
	unsigned char* header_setCode_ptr = (unsigned char*)&header_setCode[0];
	unsigned char hexNum[4];
	unsigned char hexArray[8] = {0,0,0,0,0,0,0,0};
	uint16_t pChecksum=0;

	while (i < 13)
	{
		i++;
		*cmd_ptr++ = *header_setCode_ptr++;
	}

	while (i < 19)
	{
		i++;
		*cmd_ptr++ = *templateCmd++;
	}

	if (type == 0)
	{
		memcpy(&hexNum,&setNum,4);
		for (i = 0; i < 4; i++)
		{
			hex2char(&hexArray[i*2],hexNum[i],2);
		}
		for (i = 0; i < 8; i++)
		{
			cmd[19+i] = hexArray[i];
		}
	} else if (type == 1)
	{
		hex2char(&hexArray[0],(uint16_t)setNum,2);
		for (i = 0; i < 8; i++)
		{
			cmd[19+i] = hexArray[i];
		}
	}

	pChecksum = Checksum16(&cmd[0],32);
	hex2char(&cmd[27],pChecksum,4);

	cmd[31] = 0x0d;
	cmd_ptr = &cmd[0];
	i = 0;
	while (i < 32)
	{
		i++;
		*arrCmd++ = *cmd_ptr++;
	}
}
void getCommand(unsigned char *arrCmd,unsigned char *templateCmd)
{
	uint8_t	i=0;
	unsigned char cmd[24];
	unsigned char* cmd_ptr = (unsigned char*)&cmd[0];
	unsigned char* header_getCode_ptr = (unsigned char*)&header_getCode[0];
//	unsigned char getCode[6];
	uint16_t pChecksum=0;

	while (i < 13)
	{
		i++;
		*cmd_ptr++ = *header_getCode_ptr++;
	}

	while (i < 19)
	{
		i++;
		*cmd_ptr++ = *templateCmd++;
	}

	pChecksum = Checksum16(&cmd[0],24);
	hex2char(&cmd[19],pChecksum,4);

	cmd[23] = 0x0d;
	cmd_ptr = &cmd[0];
	i = 0;
	while (i < 24)
	{
		i++;
		*arrCmd++ = *cmd_ptr++;
	}
}
