/*
 * variables.h
 *
 *  Created on: 14-12-2016
 *      Author: Administrator
 */

#ifndef SOURCE_VARIABLES_H_
#define SOURCE_VARIABLES_H_

//Dependencies
#include <stdint.h>
#include "eeprom_rtc.h"
//#include "menu.h"
#define ACTIVE_ALARM_NUMBER 18
   #define EEPROM_MACADDR1 0x02
#define EEPROM_MACADDR2 0x04
#define EEPROM_MACADDR3 0x06
#define EEPROM_MACADDR4 0x08
#define EEPROM_MACADDR5 0x0a
#define EEPROM_MACADDR6 0x0c
#define MAC_ALREADY_WRITTEN 14
   #define EEPROM_SERIALNO 16

#define DEFAULT_EEPROM_MACADDR1 0x00
#define DEFAULT_EEPROM_MACADDR2 0x1A
#define DEFAULT_EEPROM_MACADDR3 0xB6
#define DEFAULT_EEPROM_MACADDR4 0x29
#define DEFAULT_EEPROM_MACADDR5 0x02
#define DEFAULT_EEPROM_MACADDR6 0x90
   
#define UCNAMESIZE 19

//*****************************************************************************
//
//! The address of the first block of flash to be used for storing parameters.
//
//*****************************************************************************
#define FLASH_CFG_START_SN          0x000000//0x40FF00//0x000F0400//0x00FF00// External Flash Block 0 - Sector 0 - Line 0
#define FLASH_CFG_START_FW          0x001000//0x40FF00//0x000F0400//0x00FF00// External Flash Block 0 - Sector 1 - Line 0
#define FLASH_CFG_START_PARAM1          0x002000//0x40FF00//0x000F0400//0x00FF00// External Flash Block 0 - Sector 2 - Line 0  
#define FLASH_PB_LOG_START 0x003000// External Flash Block 0 - Sector 3 - Line 0
#define FLASH_PB1_LOG_START 0x004000// External Flash Block 0 - Sector 4 - Line 0
#define FLASH_PB2_LOG_START 0x005000// External Flash Block 0 - Sector 5 - Line 0
#define FLASH_PB3_LOG_START 0x006000// External Flash Block 0 - Sector 6 - Line 0 
#define FLASH_PB4_LOG_START 0x007000// External Flash Block 0 - Sector 7 - Line 0 
#define FLASH_REBOOT 0x008000// External Flash Block 0 - Sector 8 - Line 0 
 
extern uint8_t  writeFlash;
#define FLASH_FW_START 0x010000// External Flash Block 1 - Sector 0 - Line 0 

static unsigned char calibMesg_1[74] = {0x7E,0x7B,0x7C,
									0x30,0x31,
									0x20,0x20,0x20,0x20,0x20,0x20,
									0x20,0x20, // Confirm Message
									0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30, // Current IP
									0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30, // Current SN
									0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30, // Current GW
									0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30, // Current SIP
									0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30, // Current MAC                                                                        
									0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30, // FirmwareCRC
									0x20,0x20, // Test RS485 Response
									0x30,0x30,0x30,0x30,0x0D};
extern unsigned char calibMesg_1[74];
static unsigned char scuTimeSet[32] = {0x7E,0x32,0x30,0x30,0x31,0x45,0x31,0x45,0x32,0x32,0x30,0x30,0x45,
								0x30,0x30,0x30,0x30,// Year
								0x30,0x30,// Month
								0x30,0x30,// Date
								0x30,0x30,// Hour
								0x30,0x30,// Minute
								0x30,0x30,// Second
									0x30,0x30,0x30,0x30,0x0D};
extern unsigned char scuTimeSet[32];

typedef struct TimeFormat
{
	unsigned char  hour;
	unsigned char  min;
	unsigned char  sec;
	unsigned char  day;
	unsigned char  date;
	unsigned char  month;
	unsigned char  year;
}TimeFormat;

typedef struct
{
  uint32_t u32NumOfRow;
  uint8_t u8RowBuffer[45];
  uint8_t u8StartCode,
          u8ByteCount,
          u8RecordType,
          u8Checksum,
          u8ChecksumCal;
  uint8_t u8Ascii2Hex[24];
  uint8_t u8Data[16];  
  uint16_t u16FlashAddress; 
  uint32_t u32RowIdx;
  bool bIsValidRow;
  bool bIsValidStartCode;
  uint8_t u8SectorIdx;
  uint32_t u32OffsetAddr;
  uint32_t u32ReadingAddr;
  
}tFirmwarePackage;

extern tFirmwarePackage sFirmwarePackage;

typedef struct
{
   char cTimeFrame[19];
}
tAlarmTimeParameters;
//*****************************************************************************//
typedef struct
{
//   unsigned char ucContent[11];
   char ucName[21];
   char cRectID[20];
   uint8_t u8SequenceNumber;
   uint8_t u8ID;
}
tAlarmContentParameters;
//*****************************************************************************//
typedef struct
{
    tAlarmTimeParameters sTimeFrame[2];

    tAlarmContentParameters sAlarmContent;
}
tHistoryParameters;

typedef struct
{
  unsigned char ucSequenceNum;
  unsigned char ucSequenceNum_old;

  tHistoryParameters sHistoryParam[200];
}
tHistoryInfo;

extern tHistoryInfo sHistoryInfo;
typedef struct
{
  uint32_t HisNum;
  char HisName[18][21];
  char HisTime[18][5];
  char Reserve[40];
}
tHistoryStored;
extern  tHistoryStored sHistoryWrited;

typedef struct
{
  uint8_t   step;
  uint16_t  lowerVal;
  uint16_t  upperVal;
  uint16_t  addrEEPROM;
  int16_t   realVal;
  int16_t   defaultVal;
  uint8_t   point;
}sSetting_Values_Struct;


typedef struct
{
  uint8_t   status;
  uint8_t   checked;
  uint16_t  index;
  char      alarmName[21];
}sActive_Alarm_Struct;

typedef struct
{
  uint8_t   u8AlarmValue;
  uint8_t   u8NormalValue;
}tAlarmValue;
extern tAlarmValue sAlarmValue[128];

typedef struct
{
	uint32_t	u32IP;
	uint32_t	u32SN;
	uint32_t	u32GW;
	uint32_t	u32SIP;     
    	uint8_t		ucIP[16];
    	uint8_t		ucSN[16];
    	uint8_t		ucGW[16];
    	uint8_t		ucSIP[16];
//        uint8_t u8TrapMode;
//        uint32_t u32TrapInterval;
}tEthernet_Setting_Struct;

extern   uint8_t   AccessIdTemp[8];
extern uint8_t trap_flag[200];
extern uint32_t setCount_test;
extern uint32_t trapStatus_TimePeriod;
extern uint8_t DigitalInput[10];

typedef struct
{
	int32_t	s32acVolt[3];
	float	facVolt[3];
        float   f_hz_in;             //thanhcm3 add for dpc
        float   f_ac_curr[3];        //thanhcm3 add for dpc
	uint8_t u8Thres[3];          
	uint8_t u8_I_Thres[3];
        uint8_t u8_hz;               //thanhcm3 add for dpc
	uint8_t u8MainFail;
	uint8_t u8PowerOff;
        uint8_t u8ACSPD;
	float fAcLowThres;
	float fAcHighThres;
	float fAcInputCurrLimit;
	float fAcUnderThres;
        float f_ac_high_hz;         //thanhcm3 add for dpc
        float f_ac_low_hz;          //thanhcm3 add for dpc
        
        uint8_t acphase;            // Anvd33 add for Vertiv    
        
        uint16_t acAlarm[9];        // Anvd33 add for Vertiv
}
tAcInfo;
extern tAcInfo sAcInfo;
//for fan dpc-------------------------------------------------------------------
typedef struct{
  uint16_t env_temp;
  uint16_t airflow_temp;
  uint16_t voltage;
  uint16_t working_current;
  //state--------------------------
  uint8_t state_fan;
  uint8_t state_device;
  //alarm--------------------------
  uint8_t alarm_fail_refrigeration;
  uint8_t alarm_high_temp;
  uint8_t alarm_low_temp;
  uint8_t alarm_fail_sensor_temp;
  uint8_t alarm_low_voltage;
  uint8_t alarm_high_voltage;
  //parameter----------------------
  uint16_t para_dc_starting_point;
  uint16_t para_dc_sensivive_point;
  int16_t  para_dc_heating_start_point;
  int16_t para_dc_heating_sensivive;
  uint16_t para_alarm_high_temp;
  int16_t  para_alarm_low_temp;
  //parameter set-----------------
  uint32_t temp_set;
  int32_t  i_tem_set;
  //cnt response------------------
  uint8_t  no_response;
}FAN_INFO_DPC_T;
extern FAN_INFO_DPC_T fan_info_dpc_t;
//*****************************************************************************//
typedef struct
{            
	float fRect_DcCurr;
	float fRect_Temp;
	float fRect_LimCurr;
	float fRect_DcVolt;
	float fRect_AcVolt;
	uint32_t u32Rect_SN;

	uint8_t u8Rect_ACFault;
	uint8_t u8Rect_NoResp;
	uint8_t u8Rect_Fail;

	uint8_t u8Rect_Out;
	uint8_t u8Rect_Sts;
	uint8_t u8Rect_OnPosition;
        uint8_t rect_sts_current_limit;  //thanhcm3 add for dpc
        uint8_t rect_sts_fl_eq_testing;  //thanhcm3 add for dpc 
        uint8_t rect_dc_over_volt;
        uint8_t rect_ac_over_volt;
        uint8_t rect_fan_fault;
        uint8_t rect_over_temp;
        uint8_t rect_dc_over_curr; 
        
        uint16_t Alarm[3];
}
tRAWRectifierParameters;
//*****************************************************************************//
//typedef struct
//{
//	uint32_t u32Rect_DcCurr;
//	uint32_t u32Rect_Temp;
//	uint32_t u32Rect_LimCurr;
//	uint32_t u32Rect_DcVolt;
//	uint32_t u32Rect_AcVolt;
//
//	uint32_t u32Rect_Fail;
//	uint32_t u32Rect_Out;
//	uint32_t u32Rect_ACFault;
//	uint32_t u32Rect_NoResp;
//	uint32_t u32Rect_State;
//
//}
//tSNMPRectifierParameters;
//*****************************************************************************//
typedef struct
{
	uint32_t Hour;
	uint32_t Minute;
	uint32_t Second;
	uint32_t Date;
	uint32_t Month;
	uint32_t Year;
}
tTimeInfo;
extern tTimeInfo  EMERV21Time;
typedef struct
{
	float fBattTestVolt;
	float fBattTestDur;
	float fTestEndCap;

	float fStdTestEndVolt;
	uint8_t u8StdCellNo;
	float fStdTestTime;
	float fShortTestEndVolt;
	uint8_t u8ShortTestTime;

	uint32_t u32BattTestVolt;
	uint32_t u32BattTestDur;
	uint32_t u32TestEndCap;
	uint8_t u8TimeTestEn;
	tTimeInfo sPlanTest[4];
	tTimeInfo sDCTime;

	char cPlanTest1[15];
	char cPlanTest2[15];
	char cPlanTest3[15];
	char cPlanTest4[15];

	char cDCTime[21];

	uint8_t u8PlanTest1[3];
	uint8_t u8PlanTest2[3];
	uint8_t u8PlanTest3[3];
	uint8_t u8PlanTest4[3];
	uint8_t u8SetPlan;
	uint8_t u8SetSCU;

	uint8_t u8CheckSentAll;
	uint8_t u8CheckValidAll;

	uint32_t	u32STNoIdx;
	uint32_t	*pui32STNoIdx;
	uint32_t	u32STTableIdx;

	uint32_t	u32PLNoIdx;
	uint32_t	*pui32PLNoIdx;
	uint32_t	u32PLTableIdx;

	uint32_t *pui32BattTestVolt,
		 *pui32BattTestDur,
		 *pui32TestEndCap;
	uint8_t	 *pui8TimeTestEn;
}
tBattTestInfo;
extern tBattTestInfo sBattTestInfo;
//*****************************************************************************//
//typedef struct
//{
//	char cRect_SN[16];
//
//	char cRectType[16];
//	char cRectRev[16];
//
//	char cRect_DcCurr[11];
//	char cRect_Temp[11];
//	char cRect_LimCurr[11];
//	char cRect_DcVolt[11];
//	char cRect_AcVolt[11];
//
//} tREPORTRectifierParameters;

typedef struct
{
	uint16_t u8Rect_Num;
	float fAllRectDcVolt;
        uint8_t u8Rect_InfoNum;

	uint32_t	u32rectNoIdx;
	uint32_t*	pui32rectNoIdx;

	uint32_t 	u32rectInstRect,
			u32rectActRect,
			u32rectTotCurr,
			u32rectUtlz;
	int8_t  i8NumRectChanged;
	uint8_t u8NumRectlist;


	uint8_t u8rectInfolen;

	uint8_t* pui8RectType_len;
	uint8_t* pui8RectSN_len;
	uint8_t* pui8RectRev_len;

	uint8_t u8WITE;
	uint8_t u8WITI;
	tRAWRectifierParameters sRAWRectParam[24];
        
        float fTotal_Curr;

//	tSNMPRectifierParameters sSNMPRectParam[6];
//	tREPORTRectifierParameters sREPORTRectParam[6];
}
tRectifierInfo;
extern tRectifierInfo sRectInfo;

typedef struct
{
	uint8_t u8DC;		        // DC Alarm
	uint8_t u8BatVol[2];            // Batt low voltage
	uint8_t u8DC_NoFuse;            // Number of Load Fuse
	uint8_t u8Batt1_OC;             // Batt1 Over Current Alarm
	uint8_t u8Batt2_OC;             // Batt2 Over Current Alarm
	uint8_t u8BLVD;                 // BLVD Alarm
	uint8_t u8LLVD;                 // LLVD Alarm
	uint8_t u8LLVD2;                // LLVD Alarm
	uint8_t u8Sen1_BattTemp;        // Battery-Configured TempSensor Alarm
	uint8_t u8Sen1_BattInvalid;     // Battery TempSensor Invalid Alarm
	uint8_t u8Sen2_BattTemp;        // Battery-Configured TempSensor Alarm
	uint8_t u8Sen2_BattInvalid;     // Battery TempSensor Invalid Alarm
	uint8_t u8Sen3_BattTemp;
	uint8_t u8Sen3_BattInvalid;     // Battery TempSensor Invalid Alarm
	uint8_t u8Sen1_AmbTemp;         // Ambient-Configured TempSensor Alarm
	uint8_t u8Sen2_AmbTemp;         // Ambient-Configured TempSensor Alarm
	uint8_t u8Batt_Discharge;       // Battery discharge Alarm
	uint8_t u8OutVoltFault;         // Output Voltage Fault Alarm
	uint8_t u8LoadFuse[100],
			u8BattFuse[4],
			u8DIFuse[8];
        uint8_t u8NumOfUsrDefInfo;
        uint8_t u8DCSPD;		// DC Alarm
        uint8_t batt_low_volt_alarm;
        uint16_t u16SysAlarm[7]; //Anvd33 add for vertiv
}
tALARMDcInfo;
typedef struct
{
	float fVoltage;
	float fCurrent;
	uint8_t u8BatNo;
	float fBatt1Curr;
	float fBatt2Curr;
	float fBatt3Curr;
        float fBatt4Curr;     //thanhcm3 add for dpc
	float fBatt1Volt;
	float fBatt2Volt;
	float fBatt3Volt;
        float fBatt4Volt;     //thanhcm3 add for dpc
	float fBatt1RmnCap;
	float fBatt2RmnCap;
        float fBatt3RmnCap;   //thanhcm3 add for dpc
        float fBatt4RmnCap;   //thanhcm3 add for dpc
	float fSen1BattTemp;
	float fSen2BattTemp;
	float fSen3BattTemp;
        float fSen4BattTemp;  //thanhcm3 add for dpc
	float fSen1AmbTemp;
	float fSen2AmbTemp;
        float fHumidity;     //thanhcm3 add for dpc
        float fcustomized;   //thanhcm3 add for dpc       
}
tINFODcInfo;
typedef struct
{
	tALARMDcInfo sALARMDcInfo;
	tINFODcInfo sINFODcInfo;
        uint32_t u32DCNoResponse;
}
tDcInfo;
extern tDcInfo sDcInfo;
//typedef struct
//{
//	int32_t	s32Current;
//	uint32_t 	u32Voltage,
//			u32Temp,
//			u32CBStat[4],
//			u32StringNo,
//			u32CapLeft[2],
//			u32CapUsed;
//}
//tSNMPBattInfo;
typedef struct
{
	float fFltVoltCfg; //Float Voltage
	float fBotVoltCfg; //Boost Voltage
	float fTestVoltCfg; //Test Voltage
	float fTempCompVal; //TempComp Value
	float fLoMjAlrmVoltCfg; //LLVD
	float fLVDDV; //BLVD
	float fLoMnAlrmVoltCfg; //DC Low
        float fDCUnderCfg; //DC Under
        float fDCOverCfg; //DC Over

	uint8_t u8BankNo; //Battery String
	float fCapTotal; //Battery Standard Capacity
	float fCapTotal2; //Battery Standard Capacity
        float fCapTotal3; // thanhcm3 add for dpc
        float fCapTotal4; // thanhcm3 add for dpc
        float fCapTotal_all;//thanhcm3 add for dpc
	float fCCLVal;
        float fCapCCLVal;
	float fHiMjTempAlrmLevel;
	float fOvMjTempAlrmLevel;
        float fAutoTestDay;
        float fTestStartTime;
        uint8_t u8LLVDE;
        uint8_t u8BLVDE;
        float  fTotalCurr;              //Anvd33 add for vertiv
        float  fDcVolt;                 //Anvd33 add for vertiv
        float  fTemp;                   //Anvd33 add for vertiv
        uint16_t u16BattAlarm[5];       //Anvd33 add for vertiv
        
}
tRAWBattInfo;
typedef struct
{
	tRAWBattInfo	sRAWBattInfo;
//	tSNMPBattInfo	sSNMPBattInfo;
}
tBattInfo;
extern tBattInfo sBattInfo;
typedef struct
{
	int32_t i32Current;
	uint32_t 	u32Current,
		u32CBStat[10];
}
tLoadInfo;
extern tLoadInfo sLoadInfo;
typedef struct
{
	uint8_t u8EnvTemp;			// Nhiet do moi truong
	uint8_t u8EnvHumidity; 		// Do am moi truong
	uint8_t u8EnvSmoke; 		// Cam bien khoi
	uint8_t u8EnvWater; 		// Cam bien nuoc
	uint8_t u8EnvInfra;
	uint8_t u8EnvDoor; 			// Cam bien mo cua
	uint8_t u8EnvGlass;
	uint8_t u8HeatExch;
}
tALARMEnvInfo;
extern tALARMEnvInfo sAlarmEnvInfo;
typedef struct
{
	uint32_t 	u32MjHiBattVolt,
			u32MnHiBattVolt,
			u32MjLoBattVolt,
			u32MnLoBattVolt,
			u32MjBattHiTemp,
			u32MnBattHiTemp,
			u32BattDiscOpen,
			u32LLVDopen,
			u32BLVDopen,
			u32DCopen,
			u32ACmains,
			u32BattBrkOpen,
			u32DistBrkOpen,
			u32RectACFault,
			u32RectNoResp,
			u32RectFail,
			u32MjRect,
			u32MnRect,
			u32ManMode,
			u32LoadFuse,
			u32BattFuse,
			u32DIFuse[8],
			u32DIRelay,
			u32DigitalInput,
			u32ACThres,
			u32AmbTemp,
			u32Smoke,
			u32HeatExch,
			u32EnvDoor;
}
tAlarmGroupInfo;
extern tAlarmGroupInfo sAlarmInfo;

typedef struct
{
	uint8_t	cBtsNameDB[40];
	uint8_t	cBtsName[40];
	uint8_t	cBtsDCVendor[16];
	uint8_t	cBtsControllerModel[36];
	uint8_t	cBtsBattType[16];

	uint8_t	cBtsName_len;
	uint8_t	cBtsDCVendor_len;
	uint8_t	cBtsControllerModel_len;
	uint8_t	cBtsBattType_len;

	uint8_t*	pcBtsName_len;
	uint8_t*	pcBtsDCVendor_len;
	uint8_t*	pcBtsControllerModel_len;
	uint8_t*	pcBtsBattType_len;

	uint8_t*	pcBtsName;
	uint8_t*	pcBtsDCVendor;
	uint8_t*	pcBtsControllerMode;
	uint8_t*	pcBtsBattType;

	unsigned char ucMACAddress[6];

	uint32_t	u32BtsControllerModel[2];
	uint32_t	u32BtsAmbientTemp;
	uint32_t	u32RemoteReboot;
	uint32_t*	pui32RemoteReboot;
	uint32_t   u32SysTrapCnt;

}
tSiteInfo;
extern tSiteInfo sSiteInfo;

//typedef struct
//{
//	uint32_t	sysTrapRepRate,
//			sysTrapMode,
//			sysInfoWalkInEn,
//			sysInfoWalkInDur,
//			battCCLVal,
//			battFltVolt,
//			battBotVolt,
//			battHiMjAlrmVolt,
//			battHiMnAlrmVolt,
//			battLoMjAlrmVolt,
//			battLoMnAlrmVolt,
//			battTestVolt,
//			battStaManTest,
//			battTempCompVal,
//			battLVDDV,
//			battCapTotal,
//			battCapTotal2,
//			battHiMjTempAlrmLevel,
//			battHiOvTempAlrmLevel,
//			acLowLevel,
//			acHiLevel;
//
//	uint32_t        *pui32SysTrapRepRate,
//			*pui32SysTrapMode,
//			*pui32SysInfoWalkInEn,
//			*pui32SysInfoWalkInDur,
//			*pui32BattCCLVal,
//			*pui32BattFltVolt,
//			*pui32BattBotVolt,
//			*pui32BattHiMjAlrmVolt,
//			*pui32BattHiMnAlrmVolt,
//			*pui32BattLoMjAlrmVolt,
//			*pui32BattLoMnAlrmVolt,
//			*pui32BattStaManTest,
//			*pui32BattTempCompVal,
//			*pui32BattLVDDV,
//			*pui32BattCapTotal,
//			*pui32BattCapTotal2,
//			*pui32BattHiMjTempAlrmLevel,
//			*pui32BattHiMnTempAlrmLevel,
//			*pui32AcLowLevel;
//}
//tSNMPConfigInfo;
//extern tSNMPConfigInfo	sSNMPConfigInfo;
typedef struct InterruptMesg_Struct{

	uint8_t u8RecvByte[3];
	uint8_t u8Checkbit;

} tInterruptMesg_Struct;
extern tInterruptMesg_Struct sInterruptMesg;
typedef enum
{
CHECKBYTE1,
CHECKBYTE2,
CHECKBYTE3
}
tInterruptMesgState;
extern tInterruptMesgState InterruptMesgState;

typedef enum
{
LIB_COSLIGHT_INFO_1,
LIB_COSLIGHT_INFO_2,
LIB_COSLIGHT_INFO_3,
LIB_COSLIGHT_OLD_INFO_1,
LIB_COSLIGHT_OLD_INFO_2,
LIB_COSLIGHT_OLD_INFO_3,
LIB_SAFT_INFO_1,
LIB_SHOTO_INFO_1,
LIB_SHOTO_INFO_2,
LIB_SHOTO_INFO_3,
LIB_SHOTO_INFO_4,
LIB_HUAWEI_INFO_1,
LIB_HUAWEI_INFO_2,
LIB_HUAWEI_INFO_3,
LIB_HUAWEI_INFO_4,
LIB_HUAWEI_A1_INFO_1,
LIB_HUAWEI_A1_INFO_2,
LIB_HUAWEI_A1_INFO_3,
LIB_HUAWEI_A1_INFO_4,
LIB_VIETTEL_INFO_1,
LIB_VIETTEL_INFO_2,
LIB_VIETTEL_INFO_3,
LIB_VIETTEL_INFO_4,
LIB_VIETTEL_INFO_5,
LIB_VIETTEL_INFO_6,
LIB_NARADA50_INFO_1,
LIB_ZTT_INFO_1,
LIB_ZTT_INFO_2,
LIB_BYD_INFO_1,
LIB_VISION_INFO_1,
LIB_NARADA75_INFO_1,
LIB_NARADA75_INFO_2,
LIB_EVE_INFO_1,
LIB_EVE_INFO_2,
LIB_EVE_INFO_3,
LIB_ZTT_2020_INFO_1,
LIB_ZTT_2020_INFO_2,
LIB_ZTT_2020_INFO_3,

GEN_KUBOTA_INFO_1,
GEN_KUBOTA_INFO_2,
GEN_BE142_INFO_1,
GEN_BE142_INFO_2,
GEN_BE142_INFO_3,
GEN_DEEPSEA_INFO_1,
GEN_DEEPSEA_INFO_2,
GEN_DEEPSEA_INFO_3,
GEN_DEEPSEA_INFO_4,
GEN_LR2057_INFO_1,
GEN_LR2057_INFO_2,
GEN_LR2057_INFO_3,
GEN_HIMOINSA_INFO_1,
GEN_HIMOINSA_INFO_2,
GEN_QC315_INFO_1,
GEN_QC315_INFO_2,
GEN_QC315_INFO_3,
GEN_CUMMIN_INFO_1,

PM_FINECO_INFO_1,
PM_FINECO_INFO_2,
PM_ASCENT_INFO_1,
PM_ASCENT_INFO_2,
PM_ASCENT_INFO_3,
PM_ASCENT_INFO_4,
PM_ASCENT_INFO_5,
PM_ASCENT_INFO_6,
PM_EASTRON_INFO_1,
PM_EASTRON_INFO_2,
PM_EASTRON_INFO_3,
PM_EASTRON_INFO_4,
PM_CET1_INFO_1,
PM_CET1_INFO_2,
PM_CET1_INFO_3,
PM_PILOT_INFO_1,
PM_PILOT_INFO_2,
PM_PILOT_3PHASE_INFO_1,
PM_PILOT_3PHASE_INFO_2,
PM_PILOT_3PHASE_INFO_3,
ISENSE_FORLONG_INFO_1,
 
PM_SCHNEDER_3PHASE_INFO_1,
PM_SCHNEDER_3PHASE_INFO_2,
PM_SCHNEDER_3PHASE_INFO_3,
PM_SCHNEDER_3PHASE_INFO_4,
PM_SCHNEDER_3PHASE_INFO_5,
PM_SCHNEDER_3PHASE_INFO_6,
PM_SCHNEDER_3PHASE_INFO_7,
PM_SCHNEDER_3PHASE_INFO_8,
PM_SCHNEDER_3PHASE_INFO_9,

VAC_VIETTEL_INFO_1,
VAC_VIETTEL_INFO_2,
VAC_VIETTEL_INFO_3,
BM_VIETTEL_INFO_1,
BM_VIETTEL_INFO_2,

SMCB_OPEN_INFO_1,
SMCB_MATIS_INFO_1,
SMCB_GOL_INFO_1,

FUEL_HPT621_INFO_1,

PM_YADA_3PHASE_DPC_INFO_1,
PM_YADA_3PHASE_DPC_INFO_2,
PM_YADA_3PHASE_DPC_INFO_3,
PM_YADA_3PHASE_DPC_INFO_4,
PM_YADA_3PHASE_DPC_INFO_5,
PM_YADA_3PHASE_DPC_INFO_6,
PM_YADA_3PHASE_DPC_INFO_7,
PM_YADA_3PHASE_DPC_INFO_8,
PM_DC_YADA_INFO_1,
PM_DC_YADA_INFO_2,
PM_DC_YADA_INFO_3,
PM_DC_YADA_INFO_4,

PM_EASTRON_SDM72D_INFO_1,
PM_EASTRON_SDM72D_INFO_2,
PM_EASTRON_SDM72D_INFO_3,
PM_EASTRON_SDM72D_INFO_4,
PM_EASTRON_SDM72D_INFO_5,
PM_EASTRON_SDM72D_INFO_6,

ISENSE_IVY_INFO_1,
ISENSE_IVY_INFO_2,
ISENSE_IVY_INFO_3,
ISENSE_IVY_INFO_4,
ISENSE_IVY_INFO_5,
ISENSE_IVY_INFO_6,
ISENSE_IVY_INFO_7,
ISENSE_IVY_INFO_8,
ISENSE_IVY_INFO_9,

}
tRTUREQState;

typedef enum
{
  SET_DAQ_NOOP,
  SET_DAQ_WALKINTIMEEN,
  SET_DAQ_WALKINTIMEDURATION,
  SET_DAQ_CURRENTLIMIT,
  SET_DAQ_FLOATVOLT,
  SET_DAQ_BOOSTVOLT,
  SET_DAQ_LLVDVOLT,
  SET_DAQ_DCLOWVOLT,
  SET_DAQ_TEMPCOMPVALUE,
  SET_DAQ_BLVDVOLT,
  SET_DAQ_BATTCAPACITYTOTAL,
  SET_DAQ_HIGHMAJORTEMPLEVEL,
  SET_DAQ_LOWTEMPLEVEL,
  SET_DAQ_ACLOWLEVEL,
  SET_DAQ_BATTCAPACITYTOTAL2,
  SET_DAQ_DCOVERVOLT,
  SET_DAQ_DCUNDERVOLT,
  SET_DAQ_ACUNDERLEVEL,
  SET_DAQ_ACHIGHLEVEL,
  SET_DAQ_BATTTESTVOLT,
  SET_DAQ_OVERMAJORTEMPLEVEL,
  SET_DAQ_TESTVOLTCFG,
  SET_DAQ_AUTOTESTDAY,
  SET_DAQ_TESTSTARTTIME,
  SET_DAQ_BTENDVOLT,
  SET_DAQ_BTENDCAP,
  SET_DAQ_BTENDTIME,
  SET_ACM_SYSMODE,
  SET_ACM_PHASENUM,
  SET_ACM_MININPUTVOLT,
  SET_ACM_AUTORESETMODE,
  SET_ACM_AUTORESETTIME,
  SET_ACM_WARMUPTIME,
  SET_ACM_COOLDOWNTIME,
  SET_ACM_DCLOWDETECTMODE,
  SET_ACM_DCLOWVOLT,
  SET_ACM_NIGHTMODEEN,
  SET_ACM_NIGHTMODESTART,
  SET_ACM_NIGHTMODEEND,
  SET_ACM_MAXRUNTIME,
  SET_ACM_GRIDSTABLETIME,
  SET_ACM_BLOCKNUM,
  SET_ACM_STARTBLOCK1,
  SET_ACM_GENRUNTIME,
  SET_ACM_GENEXTENDTIME,
  SET_ACM_DEFAULTSETTING,
  SET_ACM_STSGRIDRUNHOUR,
  SET_ACM_STSGRIDRUNTIME,
  SET_ACM_STSGENRUNHOUR,
  SET_ACM_STSGENRUNTIME,
  SET_ACM_DATE,
  SET_ACM_MONTH,
  SET_ACM_YEAR,
  SET_ACM_HOUR,
  SET_ACM_MINUTE,
  SET_ACM_SECOND,
  SET_ACM_CFGACLOWLEVEL,
  SET_ACM_CFGACHILEVEL,
  SET_ACM_CFGFREQLOWLEVEL,
  SET_ACM_CFGFREQHILEVEL,
  SET_ACM_CFGGENPMAX,
  SET_ACM_CFGABNORMALPOWER,
  SET_ACM_CFGFUELLOWLEVEL,
  SET_VAC_SYSMODE,
  SET_VAC_FANSPEEDMODE,
  SET_VAC_FANSTARTTEMP,
  SET_VAC_SETTEMP,
  SET_VAC_PIDOFFSETTEMP,
  SET_VAC_FANMINSPEED,
  SET_VAC_FANMAXSPEED,
  SET_VAC_FILTERSTUCKTEMP,
  SET_VAC_NIGHTMODEEN,
  SET_VAC_NIGHTMODESTART,
  SET_VAC_NIGHTMODEEND,
  SET_VAC_NIGHTMAXSPEED,
  SET_VAC_MANUALMODE,
  SET_VAC_MANUALMAXSPEED,
  SET_VAC_INMAXTEMP,
  SET_VAC_EXMAXTEMP,
  SET_VAC_FROSTMAXTEMP,
  SET_VAC_INMINTEMP,
  SET_VAC_EXMINTEMP,
  SET_VAC_FROSTMINTEMP,
  SET_VAC_MINOUTTEMP,
  SET_VAC_DELTATEMP,
  SET_VAC_PANICTEMP,
  SET_VAC_ACU1ONTEMP,
  SET_VAC_ACU2ONTEMP,
  SET_VAC_ACU2EN,
  SET_VAC_SYNCTIME,
  SET_VAC_AIRCON1MODEL,
  SET_VAC_AIRCON1TYPE,
  SET_VAC_AIRCON2MODEL,
  SET_VAC_AIRCON2TYPE,
  SET_VAC_AIRCONONOFF,
  SET_VAC_AIRCONMODE,
  SET_VAC_AIRCONTEMP,
  SET_VAC_AIRCONSPEED,
  SET_VAC_AIRCONDDIR,
  SET_VAC_W_ENABLE,
  SET_VAC_W_SERIAL1,
  SET_VAC_W_SERIAL2,
  SET_VAC_RESET,
  SET_BM_VOLTDIFF,
  SET_BM_MAXTEMP,
  SET_BM_VOLTTHRES,
  SET_BM_CURRTHRES,
  SET_BM_TIMETHRES,
  SET_BM_SOCTHRES,
  SET_BM_MINTEMP,
  SET_BM_LOWCAPTIME,
  SET_BM_HALFVOLTALARM,
  SET_BM_RECHARGEVOLT,
  SET_BM_CURRBALANCETHRES,
  SET_SMU_CFGACLOWLEVEL,
  SET_SMU_CFGACHILEVEL,
  SET_SMU_CFGFREQLOWLEVEL,
  SET_SMU_CFGFREQHILEVEL,
  SET_SMU_CFGOVERPOWERDELTA,
  SET_SMU_CFGPOWERDIFF,
  SET_SMU_CFGFUELLOWLEVEL,
  SET_SMCB_STATE,
  SET_GEN_STOPMODE,
  SET_GEN_STARTMODE,
  SET_GEN_MANUALMODE,
  SET_GEN_AUTOMODE,
  SET_LIB_PACKHIGHVOLT,
  SET_LIB_BATTHIGHVOLT,
  SET_LIB_CELLHIGHVOLT,
  SET_LIB_BATTLOWVOLT,
  SET_LIB_CELLLOWVOLT,
  SET_LIB_CHARHIGHCURR,
  SET_LIB_DISCHARHIGHCURR,
  SET_LIB_CHARHIGHTEMP,
  SET_LIB_DISCHARHIGHTEMP,
  SET_LIB_CHARLOWTEMP,
  SET_LIB_DISCHARLOWTEMP,
  SET_LIB_LOWCAP,
  SET_LIB_BMSHIGHTEMP,
  SET_LIB_BMSLOWTEMP,
  SET_LIB_PACKOVERVOLT,
  SET_LIB_BATTOVERVOLT,
  SET_LIB_CELLOVERVOLT,
  SET_LIB_BATTUNDERVOLT,
  SET_LIB_CELLUNDERVOLT,
  SET_LIB_CHAROVERCURR,
  SET_LIB_DISCHAROVERCURR,
  SET_LIB_CHAROVERTEMP,
  SET_LIB_DISCHAROVERTEMP,
  SET_LIB_CHARUNDERTEMP,
  SET_LIB_DISCHARUNDERTEMP,
  SET_LIB_UNDERCAP,
  SET_LIB_BMSOVERTEMP,
  SET_LIB_BMSUNDERTEMP,
  SET_LIB_DIFFERENTVOLT,
  SET_LIB_VOLTBALANCE,
  SET_LIB_DELTAVOLTBALANCE,
  SET_LIB_DISCHARCURRLIMIT,
  SET_LIB_CHARCURRLIMIT,
  SET_LIB_VOLTDISCHARREF,
  SET_LIB_VOLTDISCHARCMD,
  SET_LIB_SYSTEMBYTE,
  SET_LIB_KEYTIME,
  SET_LIB_INIMAXIM,
  SET_LIB_ENABLEKCS,
  SET_LIB_ENABLELOCK,
  SET_LIB_CAPINIT,
  SET_FAN_DPC_STARTING_POINT,
  SET_FAN_DPC_SENSITIVE_POINT,
  SET_FAN_DPC_H_TEMP_W_POINT,
  SET_FAN_DPC_L_TEMP_W_POINT,
  SET_FAN_DPC_HEATING_START_POINT,
  SET_FAN_DPC_HEATING_SENSITIVE_POINT
}
tModbusSetCmd;

#if (USERDEF_MONITOR_LIB == ENABLED)
//======================== Lithium Battery Info structure ========================//
typedef struct
{ 
  uint32_t u32PackVolt;
  uint32_t u32BattVolt;
  uint16_t u16CellVolt[20];
  int32_t u32PackCurr;
  int16_t u16PackCurr;
  uint16_t u16CellTemp[20];
  
  uint16_t u16MinorAlarm;
  uint16_t u16MajorAlarm;
  
  uint16_t u16Protect1;
  uint16_t u16Protect2;
  
  uint16_t u16ModuleMode;
  uint16_t u16ModuleAlarm;
  uint16_t u16SoftwareVersion;
  uint16_t u16HardwareVersion;
  
  uint32_t u32AlarmStatus;
  uint32_t u32ProtectStatus;
  uint32_t u32SOC;
  uint32_t u32SOH;
  uint32_t u32DischargeTime; 
  
  uint32_t u32CapRemain;
  uint16_t u16AverTempCell;
  uint16_t u16EnvTemp;
  uint32_t u32FaultStatus;
  uint32_t u32Serial;
  
  uint8_t u8hwversion[40];
  uint8_t u8swversion[40];
  uint8_t u8mode[61];
  uint16_t sw_ver;    //thanhcm3 add fix bms Vietel 2 version 1.10, 1.06
  uint32_t reg_152;    //thanhcm3 add fix bms Vietel 2 version 1.10, 1.06  
  
  float fPackVolt;
  float fPackCurr;
  float fCapRemain;
  float fAverTempCell;
  float fEnvTemp;
  float fSOC;
  uint8_t u8NoResponseCnt;
  uint8_t u8ErrorFlag;
  uint16_t u16BattType;  
          
//----------------------M1---------------------------------//
  uint16_t u16ConverterState;
  uint16_t u16ErrCode;
  uint16_t u16BalanceStatus;
  uint16_t u16MosfetMode;
  uint32_t u32Mcu2McuErr;
  uint32_t u32CHGCapacity;
  uint32_t u32DSGCapacity;
  uint32_t u32Efficiency;
  uint16_t u16NumberOfCan;
  //===Alarm
  int32_t i32PackHighVolt_A;
  int32_t i32BattHighVolt_A;
  int32_t i32CellHighVolt_A;
  int32_t i32BattLowVolt_A;
  int32_t i32CellLowVolt_A;
  int32_t i32CharHighCurr_A;
  int32_t i32DisCharHighCurr_A;
  int32_t i32CharHighTemp_A;
  int32_t i32DisCharHighTemp_A;
  int32_t i32CharLowTemp_A;
  int32_t i32DisCharLowtemp_A;
  int32_t i32LowCap_A;
  int32_t i32BMSHighTemp_A;
  int32_t i32BMSLowTemp_A;
  //===Protect
  int32_t i32PackOverVolt_P;
  int32_t i32BattOverVolt_P;
  int32_t i32CellOverVolt_P;
  int32_t i32BattUnderVolt_P;
  int32_t i32CellUnderVolt_P;
  int32_t i32CharOverCurr_P;
  int32_t i32DisCharOverCurr_P;
  int32_t i32CharOverTemp_P;
  int32_t i32DisCharOverTemp_P;
  int32_t i32CharUnderTemp_P;
  int32_t i32DisCharUnderTemp_P;
  int32_t i32UnderCap_P;
  int32_t i32BMSOverTemp_P;
  int32_t i32BMSUnderTemp_P;
  int32_t i32DifferentVolt_P;
  //===
  uint32_t u32VoltBalance;
  uint32_t u32DeltaVoltBalance;
  uint32_t u32DisCharCurrLimit;
  uint32_t u32CharCurrLimit;
  uint32_t u32VoltDisCharRef;
  uint32_t u32VoltDisCharCMD;
  uint16_t u16SystemByte;
  uint16_t u16KeyTime;
  uint16_t u16IniMaxim;
  uint16_t u16EnableKCS;
  uint16_t u16EnableLock;
              
  uint32_t u32ADCPackVolt;
  uint32_t u32ADCBattVolt;
  int32_t i32ADCPackCurr;
  int32_t i32ADCBattCurr;
  
  uint16_t u16SOCMaxim;
  uint16_t u16SOHMaxim;
  uint16_t u16FullCapRepMaxim;
  uint16_t u16VoltMaxim;
  uint16_t u16CurrMaxim;
  
  uint32_t u32IKalamn;
  uint32_t u32SOCKalamn;
  uint32_t u32VpriKalamn;
  uint32_t u32VmesKalamn;
  uint32_t u32CapKalamn;
  uint32_t u32InternalR0Kalamn;
  uint16_t u16_liionCapInit;
//----------------------M1---------------------------------//
}
tLiionBattInfo;
extern tLiionBattInfo sLiionBattInfo[16];
//======================== Lithium Battery Info structure ========================//
#endif
#if (USERDEF_MONITOR_GEN == ENABLED)
//======================== Generator Info structure ========================//
typedef struct
{    
  uint8_t u8NoResponseCnt;
  uint8_t u8ErrorFlag;
  
  uint32_t u32ID;
  uint32_t u32Status;
  uint32_t u32LNVolt1;
  uint32_t u32LNVolt2;
  uint32_t u32LNVolt3;
  uint32_t u32LNCurr1;
  uint32_t u32LNCurr2;
  uint32_t u32LNCurr3;
  uint32_t u32LNPower1; // KW
  uint32_t u32LNPower2;
  uint32_t u32LNPower3;
  int32_t u32LNSPower1; //KVA
  int32_t u32LNSPower2;
  int32_t u32LNSPower3;
  uint32_t u32MainFrequency;
  uint32_t u32GenFrequency;
  uint32_t u32BattVolt;
  uint32_t u32CoolantTemp;
  uint32_t u32OilPressure;
  uint32_t u32FuelLevel;
  uint32_t u32RunTime;
  uint32_t u32MaintenanceTime;
  uint32_t u32EngineSpeed;
  uint32_t u32EngineWorkingHour;
  uint32_t u32EngineWorkingMin;
  uint32_t u32ActiveEnergyLow;
  uint32_t u32ActiveEnergyHigh;  
  float fPF1;
  float fPF2;
  float fPF3;  
  uint32_t u32AlarmStatus;
  uint16_t u16Error1Status;
  uint16_t u16Error2Status;
  uint16_t u16ErrorBattVolt;
  uint16_t u16StatusBits1;
  uint16_t u16StatusBits2;

}
tGenInfo;
extern tGenInfo sGenInfo[2];
//======================== Generator Info structure ========================//
#endif
#if (USERDEF_MONITOR_PM == ENABLED)
//======================== Power Meter Info structure ========================//
typedef struct
{
  float fVoltage;
  float fCurrent;
  float fActivePower;
  float fReactivePower;
  float fApparentPower;
  float fPowerFactor; 
  float fFrequency; 
}
tPhaseInfo;

typedef struct
{
  uint8_t u8NoResponseCnt;
  uint8_t u8ErrorFlag;
  
  float fRealEnergy;
  float fApparentEnergy;
  float fReactiveEnergy;
  float fRealPower;
  float fApparentPower;
  float fReactivePower;
  float fPowerFactor;
  float fFrequency;
  
  float fNeutralCurrent;
  float fTotalCurrent;
  
  int32_t u32ImportActiveE;
  int32_t u32ExportActiveE;
  int32_t u32NetActiveE;
  int32_t u32TotalActiveE;
  
  int32_t u32ImportReActiveE;
  int32_t u32ExportReActiveE;
  int32_t u32NetReActiveE;
  int32_t u32TotalReActiveE;
  
  uint8_t u8Model[20];
  uint32_t u32SerialNumber;
  tPhaseInfo sPhaseInfo[3];
  int16_t id_SerialNumber;
}
tPMInfo;
extern tPMInfo sPMInfo[2];
//======================== Power Meter Info structure ========================//
#endif
#if (USERDEF_MONITOR_ISENSE == ENABLED)
//======================== i sense grid Info structure ========================//

typedef struct
{
  uint8_t u8NoResponseCnt;
  uint8_t u8ErrorFlag;
  
  float fVoltage;
  float fCurrent;
  float fRealEnergy;
  float fApparentEnergy;
  float fReactiveEnergy;
  float fApparentPower;
  float fReactivePower;
  float fActivePower;
  float fPowerFactor;
  float fFrequency;
  
  int32_t u32TotalActiveE;
  int32_t u32TotalReActiveE;
  
  uint8_t u8Model[20];
  uint32_t u32SerialNumber;
  uint16_t serial[3];
  uint16_t serial_IVY[6];
}
tISENSEInfo;
extern tISENSEInfo sISENSEInfo[1];
//======================== i sense grid structure ========================//
#endif
//pm dc structure==========================================================
#if (USERDEF_MONITOR_PM_DC == ENABLED)
typedef struct{
  uint8_t   u8_no_response_cnt;
  uint8_t   u8_error_flag;
  float     f_voltage;
  float     f_current;
  float     f_active_power;
  float     f_active_energy;
  
  uint8_t   u8_model[20];
  
}T_PM_DC_INFO;
extern T_PM_DC_INFO s_pm_dc_info[1];
#endif
//pm dc structure==========================================================
#if (USERDEF_MONITOR_VAC == ENABLED)
//======================== VAC Info structure ========================//
typedef struct
{    
  uint8_t u8NoResponseCnt;
  uint8_t u8ErrorFlag;
  /* Status Variables */
  int16_t i16InTemp;
  int16_t i16ExTemp;
  int16_t i16Frost1Temp;
  int16_t i16Frost2Temp;
  uint32_t u32Humid;
  uint32_t u32Fan1Duty;
  uint32_t u32Fan1RPM;
  uint32_t u32Fan2Duty;
  uint32_t u32Fan2RPM;
  uint32_t u32AlarmStatus;
  uint16_t u16FirmVersion;

  uint16_t u16SN_Lo;
  uint16_t u16SN_Hi;
  /* Setting Variables */
  uint32_t u32SysMode;
  uint32_t u32FanSpeedMode;
  int16_t i16FanStartTemp;
  int16_t i16SetTemp;
  int16_t i16PidOffsetTemp;
  uint32_t u32FanMinSpeed;
  uint32_t u32FanMaxSpeed;
  int16_t i16FilterStuckTemp;
  uint32_t u32NightModeEn;
  uint32_t u32NightModeStart;
  uint32_t u32NightModeEnd;
  uint32_t u32NightMaxSpeed;
  uint32_t u32ManualMode;
  uint32_t u32ManualMaxSpeed;
  int16_t i16InMaxTemp;
  int16_t i16ExMaxTemp;
  int16_t i16FrostMaxTemp;
  int16_t i16InMinTemp;
  int16_t i16ExMinTemp;
  int16_t i16FrostMinTemp;
  int16_t i16MinOutTemp;
  int16_t i16DeltaTemp;
  int16_t i16PanicTemp;
  int16_t i16ACU1OnTemp;
  int16_t i16ACU2OnTemp;
  uint32_t u32ACU2En;
  uint32_t u32AirCon1Model;
  uint32_t u32AirCon1Type;
  uint32_t u32AirCon2Model;
  uint32_t u32AirCon2Type;
  uint32_t u32AirConOnOff;
  uint32_t u32AirConMode;
  int16_t i16AirConTemp;
  uint32_t u32AirConSpeed;
  uint32_t u32AircondDir;
  uint32_t u32AirConVolt;
  uint32_t u32AirConCurrent;
  uint32_t u32AirConPower;
  uint32_t u32AirConFrequency;
  uint32_t u32AirConPF;
  uint32_t u32RealTimeSync;
  uint32_t u32Type;
  uint32_t u32Serial;
  uint32_t u32ModbusID;
  
  uint16_t u16ActiveFan;
  uint16_t u16InstalledFan;
  uint16_t u16InstalledAirCon;
  
  uint32_t u32WEnable;
  uint32_t u32WSerial1;
  uint32_t u32WSerial2;
  
  uint8_t u8VACUpdateStep;

}
tVACInfo;
extern tVACInfo sVACInfo[1];
//======================== VAC Info structure ========================//
#endif
#if (USERDEF_MONITOR_BM == ENABLED)
//======================== BM Info structure ========================//
typedef struct
{    
  uint8_t u8NoResponseCnt;
  uint8_t u8ErrorFlag;  
  
  uint32_t u32BattVolt;
  uint32_t u32PackVolt;
  uint16_t u16CellVolt[4];
  int32_t i32PackCurr;
  uint16_t u16CellTemp[6];
  
  uint32_t u32AlarmStatus;
  uint32_t u32BalanceStatus;
  uint16_t u16BattStatus;
  
  uint16_t u16SOC;
  uint32_t u32SOH;
  uint32_t u32DischargeTime; 
  uint32_t u32TotalRunTime;
  
  uint32_t u32VoltDiff;
  uint32_t u32MaxTemp;
  uint32_t u32VoltThres;
  uint32_t u32CurrThres;
  uint32_t u32TimeThres;
  uint32_t u32SOCThres; 
  uint32_t u32MinTemp;
  uint32_t u32LowCapTime;
  uint32_t u32TotalDisAH;
  uint32_t u32HalfVoltAlarm;
  uint32_t u32RechargeVolt;
  uint32_t u32CurrBalanceThres;
  
  uint8_t cSerialNo[8];
}
tBMInfo;
extern tBMInfo sBMInfo[16];
//======================== BM Info structure ========================//
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) //smcb
//======================== SMCB Info structure ========================//
typedef struct
{    
  uint8_t u8NoResponseCnt;
  uint8_t u8ErrorFlag;  
  
  uint32_t u32State;
}
tSMCBInfo;
extern tSMCBInfo sSMCBInfo[5];
#endif
//======================== SMCB Info structure ========================//
#if (USERDEF_MONITOR_FUEL == ENABLED) //fuel
//======================== FUEL Info structure ========================//
typedef struct
{    
  uint8_t u8NoResponseCnt;
  uint8_t u8ErrorFlag;  
  
  uint32_t u32FuelLevel;
}
tFUELInfo;
extern tFUELInfo sFUELInfo[2];
#endif
//======================== FUEL Info structure ========================//

//======================== Modbus device manager structure ========================//
#if (USERDEF_MONITOR_GEN == ENABLED)
typedef struct
{
	uint8_t u8GenArrayIndex;
	uint8_t u8GenNoResponse;
        uint32_t u32GenSlaveOffset;
        uint32_t u32GenAbsSlaveID;
	uint8_t u8GenType;
        tRTUREQState u8GenRunningStep;
        uint32_t u32GenBaseAddr[4];
        uint8_t u8GenNumOfReg[4];
}
tGenManager;
#endif
#if (USERDEF_MONITOR_PM == ENABLED)
typedef struct
{
	uint8_t u8PMArrayIndex;
	uint8_t u8PMNoResponse;
	uint32_t u32PMSlaveOffset;
	uint32_t u32PMAbsSlaveID;
	uint8_t u8PMType; // 1:FINECO 2:ASCENT 
	tRTUREQState u8PMRunningStep;
	uint32_t u32PMBaseAddr[10];
	uint8_t u8PMNumOfReg[10];
}
tPMManager;
#endif	
#if (USERDEF_MONITOR_SMCB == ENABLED) //smcb
typedef struct
{
	uint8_t u8SMCBArrayIndex;
	uint8_t u8SMCBNoResponse;
	uint32_t u32SMCBSlaveOffset;
	uint32_t u32SMCBAbsSlaveID;
	uint8_t u8SMCBType;
	tRTUREQState u8SMCBRunningStep;
	uint32_t u32SMCBBaseAddr[2];
	uint8_t u8SMCBNumOfReg[2];
}
tSMCBManager;
#endif	
#if (USERDEF_MONITOR_FUEL == ENABLED)
typedef struct
{
	uint8_t u8FUELArrayIndex;
	uint8_t u8FUELNoResponse;
	uint32_t u32FUELSlaveOffset;
	uint32_t u32FUELAbsSlaveID;
	uint8_t u8FUELType;
	tRTUREQState u8FUELRunningStep;
	uint32_t u32FUELBaseAddr[2];
	uint8_t u8FUELNumOfReg[2];
}
tFUELManager;
#endif	
#if (USERDEF_MONITOR_ISENSE == ENABLED)
typedef struct
{
	uint8_t u8ISENSEArrayIndex;
	uint8_t u8ISENSENoResponse;
	uint32_t u32ISENSESlaveOffset;
	uint32_t u32ISENSEAbsSlaveID;
	uint8_t u8ISENSEType;
	tRTUREQState u8ISENSERunningStep;
	uint32_t u32ISENSEBaseAddr[10];
	uint8_t u8ISENSENumOfReg[10];
}
tISENSEManager;
#endif	
#if (USERDEF_MONITOR_PM_DC == ENABLED)
typedef struct{
  uint8_t          u8_pm_dc_array_index;
  uint8_t          u8_pm_dc_no_response;
  uint32_t         u32_pm_dc_slave_offset;
  uint32_t         u32_pm_dc_abs_slaveID;
  uint8_t          u8_pm_dc_type;
  tRTUREQState     u8_pm_dc_running_step;
  uint32_t         u32_pm_dc_base_addr[6];
  uint8_t          u8_pm_dc_num_of_reg[6];
}tPMDCManager;
#endif
#if (USERDEF_MONITOR_VAC == ENABLED)
typedef struct
{
	uint8_t u8VACArrayIndex;
	uint8_t u8VACNoResponse;
	uint32_t u32VACSlaveOffset;
	uint32_t u32VACAbsSlaveID;
	uint8_t u8VACType;
	tRTUREQState u8VACRunningStep;
	uint32_t u32VACBaseAddr[4];
	uint8_t u8VACNumOfReg[4];
}
tVACManager;
#endif
#if (USERDEF_MONITOR_LIB == ENABLED)
typedef struct
{
	uint8_t u8LIBArrayIndex;
	uint8_t u8LIBNoResponse;
        uint32_t u32LIBSlaveOffset;
        uint32_t u32LIBAbsSlaveID;
	uint8_t u8LIBType;
        tRTUREQState u8LIBRunningStep;
        uint32_t u32LIBBaseAddr[4];
        uint8_t u8LIBNumOfReg[4];
}
tLIBManager;
#endif	
#if (USERDEF_MONITOR_BM == ENABLED)
typedef struct
{
	uint8_t u8BMArrayIndex;
	uint8_t u8BMNoResponse;
	uint32_t u32BMSlaveOffset;
	uint32_t u32BMAbsSlaveID;
	uint8_t u8BMType;
	tRTUREQState u8BMRunningStep;
	uint32_t u32BMBaseAddr[4];
	uint8_t u8BMNumOfReg[4];
}
tBMManager;
#endif
typedef struct
{
  uint32_t SettingCommand;              // chon thiet bi            
	#if (USERDEF_MONITOR_GEN == ENABLED)    // may phat dien
  uint8_t u8NumberOfGen;                // so thiet bi
  uint8_t u8GenCurrentIndex;            // offset dia chi cac thiet bi (vd: tram co 3 may phat offset: 0, 1, 2)
  tGenManager sGenManager[2];
	#endif	
	#if (USERDEF_MONITOR_LIB == ENABLED)    // ac quy
  uint8_t u8NumberOfLIB;
  uint8_t u8LIBCurrentIndex;
  tLIBManager sLIBManager[16];
	#endif	
	#if (USERDEF_MONITOR_BM == ENABLED)     // ac quy
  uint8_t u8NumberOfBM;
  uint8_t u8BMCurrentIndex;
  tBMManager sBMManager[16];
	#endif	
	#if (USERDEF_MONITOR_VAC == ENABLED)
  uint8_t u8NumberOfVAC;
  uint8_t u8VACCurrentIndex;
  tVACManager sVACManager[2];
	#endif	  
	#if (USERDEF_MONITOR_PM == ENABLED)
  uint8_t u8NumberOfPM;
  uint8_t u8PMCurrentIndex;
  tPMManager sPMManager[2];  
  uint8_t u8PMBootPassFlag;
	#endif	 
        #if (USERDEF_MONITOR_SMCB == ENABLED) // smcb
        #if (USERDEF_MONITOR_FUEL == ENABLED) 
  uint8_t u8NumberOfFUEL;
  uint8_t u8NumberOfSMCB;
  uint8_t u8SMCBCurrentIndex;
  tSMCBManager sSMCBManager[5];  
	#endif	 
  uint8_t u8FUELCurrentIndex;
  tFUELManager sFUELManager[2];  
	#endif	 
        #if (USERDEF_MONITOR_ISENSE == ENABLED) 
  uint8_t u8NumberOfISENSE;
  uint8_t u8ISENSECurrentIndex;
  tISENSEManager sISENSEManager[1];  
	#endif	
        #if (USERDEF_MONITOR_PM_DC == ENABLED)
  uint8_t u8_number_of_pm_dc;
  uint8_t u8_pm_dc_current_index;
  tPMDCManager s_pm_dc_manager[1];
        #endif
}
tModbusManager;

extern tModbusManager sModbusManager;
//======================== Modbus device manager structure ========================//
typedef struct
{
  uint8_t InitUart;
  uint8_t Cnt;
  uint8_t requesttestRS485;
  uint8_t Numberofbyterecvok;
  uint8_t RS485CabRecv;
  uint8_t RS485GenRecv;
  uint8_t CabRecvCnt;
  uint8_t GenRecvCnt; 
  uint8_t CabBuff[15];
  uint8_t GenBuff[15];
  uint8_t Test485LoopOk;
}
ttestloop;
extern ttestloop sTestRS485;

extern uint8_t setCmd_flag;
extern uint8_t setCmd_Ready;

extern uint32_t setCmd_mask;
extern uint32_t settingCommand;
extern uint32_t settingCommandmask;
extern uint8_t rw_flag;
extern uint8_t u8FwUpdateCompleteFlag;
extern uint8_t u8FwUpdateFlag;
extern uint8_t u8SaveConfigFlag;
extern uint8_t u8IsRebootFlag;
extern uint8_t u8IsRewriteSN;

extern uint8_t getHistoryAlrm;
extern uint16_t u16_mesgNo;
extern uint8_t masterTxData[256];
extern uint8_t g_u8ReadBackBuffer[256];

//Thanhcm3 TEST============================
extern uint32_t EMER_cnt;
extern uint32_t EMERV21_cnt;
extern uint32_t ZTE_cnt;
extern uint32_t HW_cnt;
extern uint32_t ModbusRTU_cnt;
extern uint32_t flag_ReInitRs232;

extern uint32_t EMER_cnt_Tick;
extern uint32_t Save_EMER_cnt_Tick;




#endif /* SOURCE_VARIABLES_H_ */
