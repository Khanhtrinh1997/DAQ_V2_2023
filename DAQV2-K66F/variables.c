/*
 * variables.c
 *
 *  Created on: 14-12-2016
 *      Author: Administrator
 */
#include "variables.h"
//Dependencies
#include <stdint.h>
#include <stdbool.h>
#include <string.h> 
#include "error.h"

uint32_t  trapStatus_TimePeriod;
uint8_t   DigitalInput[10];
uint8_t   AccessIdTemp[8];
uint8_t trap_flag[200]={0};
uint8_t g_u8ReadBackBuffer[256];
   
uint8_t setCmd_flag = 0;
uint8_t setCmd_Ready = 0;
uint32_t setCmd_mask = 0;
uint32_t settingCommand = 0;
uint32_t settingCommandmask= 0;

uint8_t getHistoryAlrm = 0;
uint16_t u16_mesgNo = 0;



FAN_INFO_DPC_T fan_info_dpc_t;
tAcInfo sAcInfo;
tRectifierInfo sRectInfo;
tDcInfo sDcInfo;
tBattInfo sBattInfo;
tLoadInfo sLoadInfo;
tALARMEnvInfo sAlarmEnvInfo;
tAlarmGroupInfo sAlarmInfo;
tSiteInfo sSiteInfo;
//tSNMPConfigInfo	sSNMPConfigInfo;
tBattTestInfo sBattTestInfo;
tFirmwarePackage sFirmwarePackage;
tHistoryInfo sHistoryInfo;
tHistoryStored sHistoryWrited;
tHistoryStored *pHistoryWrited;
tAlarmValue sAlarmValue[128];
error_t ftpTransferErr;
error_t ftpTransferErr2;

tModbusManager sModbusManager;

tInterruptMesg_Struct sInterruptMesg;
tInterruptMesgState InterruptMesgState = CHECKBYTE1;
ttestloop sTestRS485;