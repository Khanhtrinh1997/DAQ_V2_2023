#include "Header.h"

// Tao mang chua lenh cai dat thong so xuong tu zte
void ZTE_SetCommand(unsigned char *arrCmd,uint16_t templateCmd, float setNum, unsigned char type)
{
	uint8_t	i=0;
	unsigned char cmd[28];
	unsigned char* cmd_ptr = (unsigned char*)&cmd[0];
	unsigned char* Header_Set_Code_ptr;
	unsigned char hexNum[4];
	unsigned char hexArray[8] = {0,0,0,0,0,0,0,0};
	uint16_t pChecksum=0;
	if(type==AC)
	{
		Header_Set_Code_ptr= (unsigned char*)&Header_AC_Setcode[0];
	}
	if(type==DC)
	{
		Header_Set_Code_ptr= (unsigned char*)&Header_DC_Setcode[0];
	}

	while (i < 13)
	{
		i++;
		*cmd_ptr++ = *Header_Set_Code_ptr++;
	}
	hex2char(&hexArray[0],templateCmd,2);
	while (i < 15)
	{
		i++;
		*cmd_ptr++ = hexArray[i-14];
	}

	memcpy(&hexNum,&setNum,4);
	for (i = 0; i < 4; i++)
	{
		hex2char(&hexArray[i*2],hexNum[i],2);
	}
	for (i = 0; i < 8; i++)
	{
		cmd[15+i] = hexArray[i];
	}

	pChecksum = Checksum16(&cmd[0],28);
	hex2char(&cmd[23],pChecksum,4);

	cmd[27] = 0x0d;
	cmd_ptr = &cmd[0];
	i = 0;
	while (i < 28)
	{
		i++;
		*arrCmd++ = *cmd_ptr++;
	}
}


void EMERV21_SetCommand(unsigned char *arrCmd,uint16_t templateCmd, float setNum, unsigned char type)
{
	uint8_t	i=0;
	unsigned char cmd[28];
	unsigned char* cmd_ptr = (unsigned char*)&cmd[0];
	unsigned char* Header_Set_Code_ptr;
	unsigned char hexNum[4];
	unsigned char hexArray[8] = {0,0,0,0,0,0,0,0};
	uint16_t pChecksum=0;
	if(type==AC)
	{
		Header_Set_Code_ptr= (unsigned char*)&Header_AC_Setcode[0];
	}
	if(type==DC)
	{
		Header_Set_Code_ptr= (unsigned char*)&Header_DC_Setcode[0];
	}

	while (i < 13)
	{
		i++;
		*cmd_ptr++ = *Header_Set_Code_ptr++;
	}
	hex2char(&hexArray[0],templateCmd,2);
	while (i < 15)
	{
		i++;
		*cmd_ptr++ = hexArray[i-14];
	}

	memcpy(&hexNum,&setNum,4);
	for (i = 0; i < 4; i++)
	{
		hex2char(&hexArray[i*2],hexNum[i],2);
	}
	for (i = 0; i < 8; i++)
	{
		cmd[15+i] = hexArray[i];
	}

	pChecksum = Checksum16(&cmd[0],28);
	hex2char(&cmd[23],pChecksum,4);

	cmd[27] = 0x0d;
	cmd_ptr = &cmd[0];
	i = 0;
	while (i < 28)
	{
		i++;
		*arrCmd++ = *cmd_ptr++;
	}
}
