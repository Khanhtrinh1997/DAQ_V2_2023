/**
 * @file private_mib_module.h
 * @brief Private MIB module
 *
 * @section License
 *
 * ^^(^____^)^^
 **/

#ifndef _PRIVATE_MIB_MODULE_H
#define _PRIVATE_MIB_MODULE_H

//Dependencies
#include "mibs/mib_common.h"
#include "Header.h"
#include "variables.h"
//Size of testString object
#define PRIVATE_MIB_TEST_STRING_SIZE 32
//Number of LEDs
#define PRIVATE_MIB_LED_COUNT 3
//Size of ledColor object
#define PRIVATE_MIB_LED_COLOR_SIZE 8


/**
 * @brief LED table entry
 **/

typedef struct
{
   char_t ledColor[PRIVATE_MIB_LED_COLOR_SIZE];
   size_t ledColorLen;
   int32_t ledState;
} PrivateMibLedEntry;

/**
 * @brief SiteInfo group
 **/

typedef struct
{
	char_t siteBTSCode[40];
	size_t siteBTSCodeLen;
	char_t siteDCsystemVendor[MIB2_IF_DESCR_SIZE];
	size_t siteDCsystemVendorLen;
	uint32_t siteControllerModel[2];                //????????
	uint32_t siteBatteryType;
	uint32_t siteAmbientTemp;
	uint32_t siteTrapCounter;
	uint32_t siteSystemReset;
	char_t siteMACInfo[17];
	size_t siteMACInfoLen;
	char_t siteSerialNumber[20];
	size_t siteSerialNumberLen;
        uint32_t siteFirmwareCRC;
	char_t siteCRCStr[20];
	size_t siteCRCStrLen;        
	uint32_t siteFirmwareUpdate;
        uint32_t siteFWType;
        char_t siteDevModel[17];
	size_t siteDevModelLen;
        uint32_t siteTrapEnable;
} PrivateMibSiteInfoGroup;

/**
 * @brief rect table entry
 **/

typedef struct
{
	int32_t rectIndex;
	uint32_t rectStatus;
	uint32_t rectOutputCurrent;
	uint32_t rectOutputVoltage;
	uint32_t rectTemp;
	char rectType[16];
        size_t rectTypeLen;
	uint32_t rectAlarmStatus;
        uint32_t rectFault;
        uint32_t rectNoResp;
	char rectSerialNo[16];
        size_t rectSerialNoLen;
	uint32_t rectRevisionLevel;
	
} PrivateMibRectInfoEntry;

/**
 * @brief RectInfo group
 **/

typedef struct
{
	uint32_t rectInstalledRect;
	uint32_t rectActiveRect;
	uint32_t rectTotalCurrent;
	uint32_t rectUtilization;
	PrivateMibRectInfoEntry rectTable[24];
} PrivateMibRectInfoGroup;

/**
 * @brief acPhase table entry
 **/

typedef struct
{
	int32_t acPhaseIndex;
	uint32_t acPhaseVolt;
	
} PrivateMibAcPhaseEntry;

/**
 * @brief AcInfo group
 **/

typedef struct
{
   int32_t acPhaseNumber;
   PrivateMibAcPhaseEntry acPhaseTable[3];
} PrivateMibAcPhaseGroup;

/**
 * @brief BatteryInfo group
 **/

typedef struct
{
	uint32_t battVolt;
	uint32_t battCurr;
	uint32_t battTemp;
	uint32_t battBanksNumofBanks;
	uint32_t battCapLeft1;
	uint32_t battCapLeft2;
        uint32_t battCapLeft3;
        uint32_t battCapLeft4;
	uint32_t battBrkStatus[4];

} PrivateMibBatteryGroup;
/**
 * @brief LoadInfo group
 **/

typedef struct
{
	uint32_t loadCurrent;
	uint8_t loadStatus[10];
              
} PrivateMibLoadGroup;

/**
 * @brief cnt group
 **/
typedef struct
{
  uint32_t EMERcnt;
  uint32_t EMERV21cnt;
  uint32_t ZTEcnt;
  uint32_t HWcnt;
  
  uint32_t ModbusRTUcnt;
  
  uint32_t EMERState;
  uint32_t EMERV21State;
  uint32_t ZTEState;
  uint32_t HWState;
  uint32_t HWRs485Cmd;
  
  uint32_t RS232ReInitEnable;
} PrivateMibCntGroup;


#if (USERDEF_MONITOR_BM == ENABLED)
//================================= BM Group =================================//

/**
 * @brief Batt Monitor table entry
 **/

typedef struct
{	 
  uint32_t bmID;
  uint32_t bmStatus;
  uint32_t bmBattVolt;
  uint32_t bmPackVolt;
  uint32_t bmPackCurr;
  uint32_t bmCellVolt1;
  uint32_t bmCellVolt2;
  uint32_t bmCellVolt3;
  uint32_t bmCellVolt4;
  uint32_t bmCellTemp1;
  uint32_t bmCellTemp2;
  uint32_t bmCellTemp3;
  uint32_t bmCellTemp4;
  uint32_t bmCellTemp5;
  uint32_t bmCellTemp6;
  union
  {
          struct
          {
                  uint32_t  protect_PackOverVolt:1;   		//0x00000001:Pack over voltage alarm
                  uint32_t  protect_BattOverVolt:1;  			//0x00000002:Pack over voltage alarm
                  uint32_t  protect_CellOverVolt:1; 			//0x00000004:Pack over voltage alarm
                  uint32_t  protect_HalfVoltProtect:1;			//0x00000008:Pack over voltage alarm
                  uint32_t  protect_BattUnderVolt:1;			//0x00000010:Pack over voltage alarm
                  uint32_t  protect_CellUnderVolt:1;   		//0x00000020:Pack over voltage alarm
                  uint32_t  protect_CharOverCurrent:1;  		//0x00000040:Pack over voltage alarm
                  uint32_t  protect_DisCharOverCurrent:1; 	//0x00000080:Pack over voltage alarm
                  uint32_t  protect_CharHighTemp:1;			//0x00000100:Pack over voltage alarm
                  uint32_t  protect_MosfetError:1;   		//0x00000200:Pack over voltage alarm
                  uint32_t  protect_CharLowTemp:1;  			//0x00000400:Pack over voltage alarm
                  uint32_t  protect_Unbalancing:1; 		//0x00000800:Pack over voltage alarm
                  uint32_t  protect_LowCapacity:1;		//0x00001000:Pack over voltage alarm                  
                  uint32_t  protect_ShortCircuit:1;  			//0x00004000:Pack over voltage alarm
                  uint32_t  reserved2:2;		//0x00002000:Pack over voltage alarm
                  
                  uint32_t  alarm_PackOverVolt:1;   		//0x00000001:Pack over voltage alarm (V= 59V)
                  uint32_t  alarm_BattOverVolt:1;  			//0x00000002:Battery over voltage alarmV (V= 58V)
                  uint32_t  alarm_CellOverVolt:1; 			//0x00000004:Cell over voltage alarm (V = 3,6V)
                  uint32_t  alarm_SocUnderVolt:1;					//0x00000008: SocUnderVolt
                  uint32_t  alarm_BattUnderVolt:1;			//0x00000010:Battery under voltage alarm(V = 43.2V)
                  uint32_t  alarm_CellUnderVolt:1;   		//0x00000020:Cell under voltage alarm(V = 2.8V)
                  uint32_t  alarm_CharOverCurrent:1;  		//0x00000040:Charge over current alarm(A=  45A)
                  uint32_t  alarm_DisCharOverCurrent:1; 	//0x00000080:Discharge over current alarm(A=  50A)
                  uint32_t  alarm_CharHighTemp:1;			//0x00000100:Charge high temperature alarm(T=  65?)
                  uint32_t  alarm_DisCharHighTemp:1;   		//0x00000200:Discharge high temperature alarm(T=  65?)
                  uint32_t  alarm_CharLowTemp:1;  			//0x00000400:Charge low temperature alarm(T=  5?)
                  uint32_t  alarm_DisCharLowTemp:1; 		//0x00000800:Discharge low temperature alarm(T=-10?)
                  uint32_t  alarm_LowCapacity:1;		//0x00001000:Low capacity alarm (SOC=10%)
                  uint32_t  reserved1:3;
                  
                  
                  
          }bit;
          uint32_t all;
  }bmAlarmStatus;
  
//  uint32_t bmAlarmStatus;
  uint32_t bmBalanceStatus;
  uint32_t bmBattStatus;
  uint32_t bmSOC;
  uint32_t bmSOH;
  uint32_t bmDischargeTime;
  uint32_t bmTotalRunTime;
  uint32_t bmVoltDiff;
  uint32_t bmMaxTemp;
  uint32_t bmVoltThres;
  uint32_t bmCurrThres;
  uint32_t bmTimeThres;
  uint32_t bmSOCThres;  
  uint32_t bmMinTemp;
  uint32_t bmLowCapTime;
  uint32_t bmTotalDisAH;
  uint32_t bmHalfVoltAlarm;
  uint32_t bmRechargeVolt;
  uint32_t bmCurrBalanceThres;
  uint32_t bmType;
  uint32_t bmModbusID;
	
} PrivateMibBMInfoEntry;

/**
 * @brief Batt Monitor group
 **/

typedef struct
{
	uint32_t bmInstalledBM;
	uint32_t bmActiveBM;
	PrivateMibBMInfoEntry bmTable[16];
} PrivateMibBMGroup;
//================================= BM Group =================================//
#endif

/**
 * @brief LIBatt table entry
 **/

typedef struct
{
	int32_t liBattIndex;
	uint32_t liBattStatus;
	int32_t liBattPackVolt;
	int32_t liBattPackCurr;
	uint32_t liBattRemainCap;
	int32_t liBattAvrCellTemp;
	int32_t liBattAmbTemp;
	uint32_t liBattWarningFlag;
	uint32_t liBattProtectFlag;
	uint32_t liBattFaultStat;
	uint32_t liBattSOC;
        uint32_t liBattType;
        uint16_t liBattModbusID;
        uint32_t liBattCellVolt[16];
        int32_t liBattCellTemp[16];
	uint32_t liBattSOH;
        uint32_t liBattPeriod;
        uint32_t liBattStatusCCL;
        uint32_t liBattMode;
	char liBattModel[20];
        size_t liBattModelLen;
	
	char liBattSWVer[20];
        size_t liBattSWVerLen;
	
	char liBattHWVer[20];
        size_t liBattHWVerLen;
	
	char liBattSerialNo[31];
        size_t liBattSerialNoLen;
        union
        {
                struct
                {
                        uint32_t  alarm_CellOverVolt:1;   		//0x00000001:
                        uint32_t  alarm_CellLowVolt:1;  	        //0x00000002:
                        uint32_t  alarm_PackOverVolt:1;			//0x00000004:
                        uint32_t  alarm_PackLowVolt:1;			//0x00000008:
                        uint32_t  alarm_CharOverCurrent:1;   		//0x00000010:
                        uint32_t  alarm_DisCharOverCurrent:1;  		//0x00000020:
                        uint32_t  alarm_BattHighTemp:1; 	        //0x00000040:(Charging high temperature OR Discharge high temperature alarm)-thanhcm3 fix
                        uint32_t  alarm_BattLowTemp:1;			//0x00000080:(Charging low temperature OR Discharge low temperature alarm)  -thanhcm3 fix
                        uint32_t  alarm_EnvHighTemp:1;   		//0x00000100:
                        uint32_t  alarm_EnvLowTemp:1;  			//0x00000200:
                        uint32_t  alarm_PCBHighTemp:1; 		        //0x00000400:
                        uint32_t  alarm_LowCapacity:1;		        //0x00000800:
                        uint32_t  alarm_VoltDiff:1;		        //0x00001000:
                        uint32_t  reserved1:3;
                        
                        uint32_t  protect_CellOverVolt:1;   		//0x00000001:
                        uint32_t  protect_CellLowVolt:1;  		//0x00000002:
                        uint32_t  protect_PackOverVolt:1; 		//0x00000004:
                        uint32_t  protect_PackLowVolt:1;		//0x00000008:
                        uint32_t  protect_ShortCircuit:1;		//0x00000010:
                        uint32_t  protect_OverCurr:1;   		//0x00000020:
                        uint32_t  protect_char_or_dischar_high_temp:1;  //0x00000040:(Charging high temperature OR Discharge high temperature protection) -thanhcm3 fix 
                        uint32_t  protect_char_or_dischar_low_temp:1; 	//0x00000080:(Charging low temperature OR Discharge low temperature protection)   -thanhcm3 fix 
                        uint32_t  protect_env_high_temp:1;		//0x00000100: protect_EnvHighTemp-thanhcm3 fix 
                        uint32_t  protect_env_low_temp:1;   		//0x00000200: protect_EnvLowTemp -thanhcm3 fix 
                        uint32_t  protect_pcb_high_temp:1;      	//0x00000400: protect_PCBHighTemp-thanhcm3 fix
                        uint32_t  protect_reverse:1;			
                        uint32_t  break_cell:1;                         //Battery cell fault        -thanhcm3 fix
                        uint32_t  break_sensor:1;                       //Temperature sensor fault  -thanhcm3 fix
                        uint32_t  break_charging_mosfet:1;              //Charging MOSFET fault     -thanhcn3 fix
                        uint32_t  break_discharging_mosfet:1;           //Discharging MOSFET fault  -thanhcm3 fix  
                }bit;
                uint32_t all;
        }liBattAlarmStatus;
        
//----------------------M1---------------------------------//
        uint32_t u32ConverterState;
        uint32_t u32ErrCode;
        uint32_t u32BalanceStatus;
        uint32_t u32MosfetMode;
        uint32_t u32Mcu2McuErr;
        uint32_t u32CHGCapacity;
        uint32_t u32DSGCapacity;
        uint32_t u32Efficiency;
        uint32_t u32NumberOfCan;
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
        uint32_t u32SystemByte;
        uint32_t u32KeyTime;
        uint32_t u32IniMaxim;
        uint32_t u32EnableKCS;
        uint32_t u32EnableLock;
        //===Write
        //===Alarm
        int32_t i32PackHighVolt_AWrite;
        int32_t i32BattHighVolt_AWrite;
        int32_t i32CellHighVolt_AWrite;
        int32_t i32BattLowVolt_AWrite;
        int32_t i32CellLowVolt_AWrite;
        int32_t i32CharHighCurr_AWrite;
        int32_t i32DisCharHighCurr_AWrite;
        int32_t i32CharHighTemp_AWrite;
        int32_t i32DisCharHighTemp_AWrite;
        int32_t i32CharLowTemp_AWrite;
        int32_t i32DisCharLowtemp_AWrite;
        int32_t i32LowCap_AWrite;
        int32_t i32BMSHighTemp_AWrite;
        int32_t i32BMSLowTemp_AWrite;
        //===Protect
        int32_t i32PackOverVolt_PWrite;
        int32_t i32BattOverVolt_PWrite;
        int32_t i32CellOverVolt_PWrite;
        int32_t i32BattUnderVolt_PWrite;
        int32_t i32CellUnderVolt_PWrite;
        int32_t i32CharOverCurr_PWrite;
        int32_t i32DisCharOverCurr_PWrite;
        int32_t i32CharOverTemp_PWrite;
        int32_t i32DisCharOverTemp_PWrite;
        int32_t i32CharUnderTemp_PWrite;
        int32_t i32DisCharUnderTemp_PWrite;
        int32_t i32UnderCap_PWrite;
        int32_t i32BMSOverTemp_PWrite;
        int32_t i32BMSUnderTemp_PWrite;
        int32_t i32DifferentVolt_PWrite;
        //===
        uint32_t u32VoltBalanceWrite;
        uint32_t u32DeltaVoltBalanceWrite;
        uint32_t u32DisCharCurrLimitWrite;
        uint32_t u32CharCurrLimitWrite;
        uint32_t u32VoltDisCharRefWrite;
        uint32_t u32VoltDisCharCMDWrite;
        uint32_t u32SystemByteWrite;
        uint32_t u32KeyTimeWrite;
        uint32_t u32IniMaximWrite;
        uint32_t u32EnableKCSWrite;
        uint32_t u32EnableLockWrite;
        uint16_t u16_liionCapInitWrite;
          
        uint32_t u32ADCPackVolt;
        uint32_t u32ADCBattVolt;
        int32_t i32ADCPackCurr;
        int32_t i32ADCBattCurr;
        
        uint32_t u32SOCMaxim;
        uint32_t u32SOHMaxim;
        uint32_t u32FullCapRepMaxim;
        uint32_t u32VoltMaxim;
        uint32_t u32CurrMaxim;
        
        uint32_t u32IKalamn;
        uint32_t u32SOCKalamn;
        uint32_t u32VpriKalamn;
        uint32_t u32VmesKalamn;
        uint32_t u32CapKalamn;
        uint32_t u32InternalR0Kalamn;
        uint16_t u16_liionCapInit;
//----------------------M1---------------------------------//
} PrivateMibLIBattInfoEntry;

/**
 * @brief LIBattInfo group
 **/

typedef struct
{
	uint32_t liBattInstalledPack;
	uint32_t liBattActivePack;
	uint32_t liBattTotalCurrent;
	uint32_t liBattUtilization;
	PrivateMibLIBattInfoEntry liBattTable[16];
} PrivateMibLIBattInfoGroup;

//================================= SMCB Group =================================//
/**
 * @brief SMCB table entry
 **/
typedef struct // smcb
{
  int32_t SmcbStatusID;
  uint32_t SmcbStatus; // connect, 0:no 1:yes
  uint32_t SmcbState;  // 0:off 1:on
  uint32_t SmcbStateWrite;
  uint32_t SmcbType;    
  uint16_t SmcbModbusID;
} PrivateMibSMCBInfoEntry;

/**
 * @brief SMCBInfo group
 **/
typedef struct
{
	uint32_t SmcbInstalledSMCB;
	uint32_t SmcbActiveSMCB;
        uint32_t flag; 
	PrivateMibSMCBInfoEntry SmcbTable[5];
} PrivateMibSMCBInfoGroup;

//================================= SMCB Group =================================//

//================================= FUEL Group =================================//
/**
 * @brief FUEL table entry
 **/
typedef struct 
{
  int32_t FuelStatusID;
  uint32_t FuelStatus; // connect, 0:no 1:yes
  uint32_t FuelType;    
  uint16_t FuelModbusID;
  uint32_t FuelLevel;
} PrivateMibFUELInfoEntry;

/**
 * @brief FUELInfo group
 **/
typedef struct
{
	uint32_t FuelInstalledFUEL;
	uint32_t FuelActiveFUEL; 
	PrivateMibFUELInfoEntry FuelTable[2];
} PrivateMibFUELInfoGroup;

//================================= FUEL Group =================================//
/**
 * @brief GENInfo table entry
 **/
typedef struct
{
  	uint32_t genStatusID;
	uint32_t genStatusStatus;
	uint32_t genStatusLNVolt1;
	uint32_t genStatusLNVolt2;
	uint32_t genStatusLNVolt3;
	uint32_t genStatusLNCurr1;
	uint32_t genStatusLNCurr2;
	uint32_t genStatusLNCurr3;
	uint32_t genStatusLNPower1;
	uint32_t genStatusLNPower2;
	uint32_t genStatusLNPower3;
	int32_t genStatusLNSPower1;
	int32_t genStatusLNSPower2;
	int32_t genStatusLNSPower3;
	uint32_t genStatusFrequency;
	uint32_t genStatusBattVolt;
	uint32_t genStatusCoolantTemp;
	uint32_t genStatusOilPressure;
	uint32_t genStatusFuelLevel;
	uint32_t genStatusRunTime;
	uint32_t genStatusOilTemp;
	uint32_t genStatusMaintenanceTime;
	uint32_t genStatusEngineSpeed;
        uint32_t genStatusWorkingHour;
        uint32_t genStatusWorkingMin;
        uint32_t genStatusActiveEnergyLow;
        uint32_t genStatusActiveEnergyHigh;   
        
        union
        {
          struct
          {
                  uint32_t  err_LowFuel:1;   		        //0x00000001:   uint32_t  err_ChargeAlterFail:1; 
                  uint32_t  err_UnderFreq:1;  			//0x00000002:*
                  uint32_t  err_OverFreq:1; 			//0x00000004:*
                  uint32_t  err_HiCoolantTemp:1;		//0x00000008:* 
                  uint32_t  err_UnderVolt:1;			//0x00000010:*
                  uint32_t  err_OverVolt:1;   		        //0x00000020:*
                  uint32_t  err_LowOilPressure:1;  		//0x00000040:*
                  uint32_t  err_HiBattVol:1; 	                //0x00000080:   uint32_t  err_FailToStart:1; 	
                  uint32_t  err_LowBattVol:1;			//0x00000100:   uint32_t  err_AI2high:1;	
                  uint32_t  err_GenPhaseSeqFail:1;   		//0x00000200:
                  uint32_t  err_UnderPower:1;  			//0x00000400:
                  uint32_t  err_OverPower:1; 		        //0x00000800:
                  uint32_t  err_OilPressureSensor:1;		//0x00001000:
                  uint32_t  err_Tempsensor:1;                   //0x00002000:
                  uint32_t  err_AI1sensor:1;                    //0x00004000:
                  uint32_t  err_AI2sensor:1;                    //0x00008000:
                  
                  uint32_t  err_EmergencyStop:1;   		//0x00000001:
                  uint32_t  err_GenStop:1;  			//0x00000002:
                  uint32_t  err_ReversePower:1; 		//0x00000004:
                  uint32_t  err_CabinTempLow:1;			//0x00000008:
                  uint32_t  err_CabinTempHigh:1;		//0x00000010:
                  uint32_t  err_PickupSignalLoss:1;   		//0x00000020:
                  uint32_t  err_Al1low:1;  		        //0x00000040:
                  uint32_t  err_OverSpeed:1; 	                //0x00000080:
                  uint32_t  err_UnderSpeed:1;			//0x00000100:
                  uint32_t  err_AI2low:1;   		        //0x00000200:
                  uint32_t  err_AI1high:1;  			//0x00000400:
                  uint32_t  err_OverCurr:1; 		        //0x00000800:*
                  uint32_t  err_ShortCircuit:1;		        //0x00001000:
                  uint32_t  err_EarthFault:1;		        //0x00002000:
                  uint32_t  err_UnderCurrent:1;  		//0x00004000:
                  uint32_t  err_Maintenance:1;                  //0x00008000:
          }bit;
          uint32_t all;
        }genStatusAlarmStatus;
//	uint32_t genStatusAlarmStatus;
        
        char_t genSerial[20];
        size_t genSerialLen;
        uint16_t genStatusModbusID;
        uint8_t genStatusType;
        uint8_t genStopMode;
        uint8_t genManualMode;
        uint8_t genAutoMode;
        uint8_t genStartMode;

} PrivateMibGenInfoEntry;

/**
 * @brief GenInfo group
 **/

typedef struct
{
	uint32_t genInstalledGen;
	uint32_t genActiveGen;
        uint32_t genflag;
	PrivateMibGenInfoEntry genTable[2];
} PrivateMibGenInfoGroup;

//================================= PM Group =================================//

/**
 * @brief LIBatt table entry
 **/

typedef struct
{
  uint32_t pmID;	
  uint32_t pmStatus;	
  uint32_t pmImportActiveEnergy;	
  uint32_t pmExportActiveEnergy;
  uint32_t pmImportReactiveEnergy;	
  uint32_t pmExportReactiveEnergy;	
  uint32_t pmTotalActiveEnergy;	
  uint32_t pmTotalReactiveEnergy;	
  uint32_t pmActivePower;	
  int32_t  pmReactivePower;	
  uint32_t pmPowerFactor;
  uint32_t pmFrequency;	
  uint32_t pmTotalCurrent;	
  
  uint32_t pmPhase1Current;	
  uint32_t pmPhase1Voltage;	
  uint32_t pmPhase1RealPower;
  uint32_t pmPhase1ApparentPower;	
  int32_t pmPhase1ReactivePower;	
  uint32_t pmPhase1PowerFactor;
  uint32_t pmPhase2Current;	
  uint32_t pmPhase2Voltage;	
  uint32_t pmPhase2RealPower;
  uint32_t pmPhase2ApparentPower;	
  uint32_t pmPhase2ReactivePower;	
  uint32_t pmPhase2PowerFactor;
  uint32_t pmPhase3Current;	
  uint32_t pmPhase3Voltage;	
  uint32_t pmPhase3RealPower;
  uint32_t pmPhase3ApparentPower;	
  uint32_t pmPhase3ReactivePower;
  uint32_t pmPhase3PowerFactor;  
  
  uint32_t pmOutOfRangeFreq;  
  uint32_t pmDeltaFreqDisConnect;  
  uint32_t pmDeltaFreqReConnect;  
  uint32_t pmEnableFreqTrap;
  uint32_t pm_delta_freq_v;
  uint8_t  pm_cnt_timeout_freq_out;
  uint8_t  pm_cnt_timeout_freq_in;
  
  uint8_t  pm_remaining_gen;
  uint16_t pm_set_cnt_remaining_gen;
  
  
  char_t pmSerial[20];
  char_t pmModel[20];
  size_t pmSerialLen;
  size_t pmModelLen;
  uint32_t pmType;
  uint32_t pmModbusID;
	
} PrivateMibPMInfoEntry;

/**
 * @brief LIBattInfo group
 **/

typedef struct
{
	uint32_t pmInstalledPM;
	uint32_t pmActivePM;
	PrivateMibPMInfoEntry pmTable[2];
} PrivateMibPMGroup;
//================================= PM Group =================================//
//================================= ISENSE Group =================================//

/**
 * @brief ISense table entry
 **/

typedef struct
{
  uint32_t isenseID;	
  uint32_t isenseStatus;	
  uint32_t isenseImportActiveEnergy;	
  uint32_t isenseExportActiveEnergy;
  uint32_t isenseImportReactiveEnergy;	
  uint32_t isenseExportReactiveEnergy;	
  uint32_t isenseTotalActiveEnergy;	
  uint32_t isenseTotalReactiveEnergy;	
  uint32_t isenseActivePower;	
  int32_t  isenseReactivePower;	
  int32_t  isenseApparentPower; 
  uint32_t isensePowerFactor;
  uint32_t isenseFrequency;	
  uint32_t isenseCurrent;	
  uint32_t isenseVoltage;
  
  uint32_t isenseOutOfRangeFreq;  
  uint32_t isenseDeltaFreqDisConnect;  
  uint32_t isenseDeltaFreqReConnect;
  uint32_t isense_delta_freq_v;  
  uint32_t isenseEnableFreqTrap; 
  uint8_t  isense_cnt_timeout_freq_out;
  uint8_t  isense_cnt_timeout_freq_in;
  
  char_t isenseSerial[20];
  char_t isenseModel[20];
  size_t isenseSerialLen;
  size_t isenseModelLen;
  uint32_t isenseType;
  uint32_t isenseModbusID;
	
} PrivateMibISENSEInfoEntry;
/**
 * @brief ISenseInfo group
 **/
typedef struct
{
	uint32_t isenseInstalledISENSE;
	uint32_t isenseActiveISENSE;
	PrivateMibISENSEInfoEntry isenseTable;
} PrivateMibISENSEGroup;
//================================= ISENSE Group =================================//
//PM_DC Group=======================================================================
//thanhcm3 add for dpc--------------------------------------------------------------
//privte mib pm dc info entry-------------------------------------------------------
typedef struct{
  uint32_t pm_dc_id;
  uint32_t pm_dc_status;
  uint32_t pm_dc_active_power;
  uint32_t pm_dc_active_energy;
  uint32_t pm_dc_current;
  uint32_t pm_dc_voltage;
  char_t   pm_dc_model_[20];
  size_t   pm_dc_model_len;
  uint32_t pm_dc_type;
  uint32_t pm_dc_modbus_ID; 
}PrivateMibPMDCInfoEntry;
//pribate mib pm dc froup----------------------------------------------------------
typedef struct{
  uint32_t                 pm_dc_installed_PM_DC;
  uint32_t                 pm_dc_active_PM_DC;
  PrivateMibPMDCInfoEntry  pm_dc_table;
}PrivateMibPMDCGroup;
//PM_DC Group======================================================================
//FAN DPC Group=====================================================================
typedef struct {
  FAN_INFO_DPC_T mib;
  
}PrivateMibFANDPCInfo;

//FAN DPC Group=====================================================================
//================================= VAC Group =================================//

/**
 * @brief VAC table entry
 **/

typedef struct
{
  uint32_t vacID;
  uint32_t vacStatus;
  int32_t vacInTemp;
  int32_t vacExTemp;
  int32_t vacFrost1Temp;
  int32_t vacFrost2Temp;
  uint32_t vacHumid;
  uint32_t vacFan1Duty;
  uint32_t vacFan1RPM;
  uint32_t vacFan2Duty;
  uint32_t vacFan2RPM;  
  union
  {
          struct
          {            
            uint32_t  alarm_Frost2Under:1;		//0x00010000: Frost2Under
            uint32_t  alarm_AirConOver:1;		//0x00020000: AirConOver
            uint32_t  alarm_AirConUnder:1;		//0x00040000: AirConUnder
            uint32_t  alarm_HiHumid:1;		//0x00080000: HiHumid
            uint32_t  alarm_FilterStuck:1;		//0x00100000: FilterStuck
            uint32_t  :11;    
            
            
            uint32_t  alarm_AirCon1Status:1;   		//0x00000001:AirCon1Status ON=1/OFF=0 
            uint32_t  alarm_AirCon2Status:1;  			//0x00000002:AirCon2Status ON=1/OFF=0  
            uint32_t  alarm_AirCon1Fail:1; 			//0x00000004: AirCon1Fail
            uint32_t  alarm_AirCon2Fail:1;			//0x00000008: AirCon2Fail
            uint32_t  alarm_FanFail:1;			//0x00000010: FanFail
            uint32_t  alarm_DI1:1;   		//0x00000020: DI1 Alarm
            uint32_t  alarm_DI2:1;  		//0x00000040: DI2 Alarm
            uint32_t  alarm_DO1:1; 	//0x00000080: DO1 Status CLOSE=0/OPEN=1
            uint32_t  alarm_DO2:1;			//0x00000100: DO1 Status CLOSE=0/OPEN=1
            uint32_t  alarm_InTempOver:1;   		//0x00000200: InTempOver
            uint32_t  alarm_InTempUnder:1;  			//0x00000400: InTempUnder
            uint32_t  alarm_ExTempOver:1; 		//0x00000800: ExTempOver
            uint32_t  alarm_ExTempUnder:1;		//0x00001000: ExTempUnder
            uint32_t  alarm_Frost1Over:1;		//0x00002000: Frost1Over
            uint32_t  alarm_Frost1Under:1;		//0x00004000: Frost1Under
            uint32_t  alarm_Frost2Over:1;		//0x00008000: Frost2Over
          }bit;
          uint32_t all;
  }vacAlarmStatus;
  
  uint32_t vacSysMode;
  uint32_t vacFanSpeedMode;
  int32_t vacFanStartTemp;
  int32_t vacSetTemp;
  int32_t vacPidOffsetTemp;
  uint32_t vacFanMinSpeed;
  uint32_t vacFanMaxSpeed;
  int32_t vacFilterStuckTemp;
  uint32_t vacNightModeEn;
  uint32_t vacNightModeStart;
  uint32_t vacNightModeEnd;
  uint32_t vacNightMaxSpeed;
  uint32_t vacManualMode;
  uint32_t vacManualMaxSpeed;
  int32_t vacInMaxTemp;
  int32_t vacExMaxTemp;
  int32_t vacFrostMaxTemp;
  int32_t vacInMinTemp;
  int32_t vacExMinTemp;
  int32_t vacFrostMinTemp;
  int32_t vacMinOutTemp;
  int32_t vacDeltaTemp;
  int32_t vacPanicTemp;
  int32_t vacACU1OnTemp;
  int32_t vacACU2OnTemp;
  uint32_t vacACU2En;
  uint32_t vacAirCon1Model;
  uint32_t vacAirCon1Type;
  uint32_t vacAirCon2Model;
  uint32_t vacAirCon2Type;
  uint32_t vacAirConOnOff;
  uint32_t vacAirConMode;
  int32_t vacAirConTemp;
  uint32_t vacAirConSpeed;
  uint32_t vacAircondDir;
  uint32_t vacAirConVolt;
  uint32_t vacAirConCurrent;
  uint32_t vacAirConPower;
  uint32_t vacAirConFrequency;
  uint32_t vacAirConPF;
  uint32_t vacRealTimeSync;
  uint32_t vacType;
  char_t vacSerial[20];
  size_t vacSerialLen;
  uint32_t vacModbusID;
  uint32_t vacUpdateFlag;
  uint32_t vacFirmVersion;
  char_t cSyncTime[20];
  size_t cSyncTimeLen;
  uint32_t vacWEnable;
  uint32_t vacWSerial1;
  uint32_t vacWSerial2;
  uint32_t vacEnableReset;
  // write
  uint32_t vacSysModeWrite;
  uint32_t vacFanSpeedModeWrite;
  int32_t vacFanStartTempWrite;
  int32_t vacSetTempWrite;
  int32_t vacPidOffsetTempWrite;
  uint32_t vacFanMinSpeedWrite;
  uint32_t vacFanMaxSpeedWrite;
  int32_t vacFilterStuckTempWrite;
  uint32_t vacNightModeEnWrite;
  uint32_t vacNightModeStartWrite;
  uint32_t vacNightModeEndWrite;
  uint32_t vacNightMaxSpeedWrite;
  uint32_t vacManualModeWrite;
  uint32_t vacManualMaxSpeedWrite;
  int32_t vacInMaxTempWrite;
  int32_t vacExMaxTempWrite;
  int32_t vacFrostMaxTempWrite;
  int32_t vacInMinTempWrite;
  int32_t vacExMinTempWrite;
  int32_t vacFrostMinTempWrite;
  int32_t vacMinOutTempWrite;
  int32_t vacDeltaTempWrite;
  int32_t vacPanicTempWrite;
//  int32_t vacACU1OnTemp;
//  int32_t vacACU2OnTemp;
//  uint32_t vacACU2En;
  uint32_t vacAirCon1ModelWrite;
  uint32_t vacAirCon1TypeWrite;
  uint32_t vacAirCon2ModelWrite;
  uint32_t vacAirCon2TypeWrite;
  uint32_t vacAirConOnOffWrite;
  uint32_t vacAirConModeWrite;
  int32_t vacAirConTempWrite;
  uint32_t vacAirConSpeedWrite;
  uint32_t vacAircondDirWrite;  
  uint32_t vacWEnableWrite;
  uint32_t vacWSerial1Write;
  uint32_t vacWSerial2Write;
  // write
  uint32_t vacActiveFan;
  uint32_t vacInstalledFan;
  uint32_t vacInstalledAirCon;
  uint32_t vacUpdateStep;
  
} PrivateMibVACInfoEntry;

/**
 * @brief VAC group
 **/

typedef struct
{
	uint32_t vacInstalledVAC;
	uint32_t vacActiveVAC;
	PrivateMibVACInfoEntry vacTable[1];
} PrivateMibVACGroup;
//================================= VAC Group =================================//
/**
 * @brief configAcc table entry
 **/

typedef struct
{
	int32_t cfgBTPlanTestIndex;
	uint8_t cfgBTPlanTestMonth;
	uint8_t cfgBTPlanTestDate;
	uint8_t cfgBTPlanTestHour;
        char cfgBTPlanTestString[16];
        size_t cfgBTPlanTestStringLen;
} PrivateMibCfgBTPlanTestEntry;

/**
 * @brief cfgBTSCU table entry
 **/

typedef struct
{
	int32_t cfgBTSCUIndex;
	uint16_t cfgBTSCUYear;
	uint8_t cfgBTSCUMonth;
	uint8_t cfgBTSCUDate;
	uint8_t cfgBTSCUHour;
	uint8_t cfgBTSCUMinute;
	uint8_t cfgBTSCUSecond;        
        char cfgBTSCUString[21];
        size_t cfgBTSCUStringLen;
} PrivateMibCfgBTSCUEntry;

/**
 * @brief configAcc group
 **/

typedef struct
{
	uint32_t cfgBTEndVolt;
	uint32_t cfgBTEndCap;
	uint32_t cfgBTEndTime;
	uint8_t cfgBTPlanTestNumber;
	uint8_t cfgBTPlanTestEn;
	uint8_t cfgBTSCUNumber;
        uint8_t cfgBTCellNo;
        uint32_t cfgTestVoltCfg;
        uint32_t cfgAutoTestDay;
        uint32_t cfgTestStartTime;
        uint32_t cfgAgissonBTRespond;
	PrivateMibCfgBTPlanTestEntry cfgBTPlanTestTable[4];
	PrivateMibCfgBTSCUEntry cfgBTSCUTable[1];
} PrivateMibCfgBTGroup;

/**
 * @brief ConfigInfo group
 **/

typedef struct
{
	uint32_t cfgWalkInTimeEn;
	uint32_t cfgWalkInTimeDuration;
	uint32_t cfgCurrentLimit;
        uint32_t cfgCurrentlimitA; //thanhcm3 add for dpc
	uint32_t cfgFloatVolt;
	uint32_t cfgBoostVolt;
	uint32_t cfgLLVDVolt;
	uint32_t cfgDCLowVolt;
	uint32_t cfgStartManualTest;
	uint32_t cfgTempCompValue;
	uint32_t cfgBLVDVolt;
	uint32_t cfgBattCapacityTotal;
	int32_t cfgHighMajorTempLevel;
        int32_t cfgLowTempLevel;
	uint32_t cfgAcLowLevel;
	uint32_t cfgBattCapacityTotal2;
        uint32_t cfgBattCapacityTotal3; //thanhcm3 add for dpc
        uint32_t cfgBattCapacityTotal4; //thanhcm3 add for dpc
        //uint16_t fan_para_dc_starting_point; //thanhcm3 add for dpc
        //uint16_t fan_para_dc_sensivive_point;//thanhcm3 add for dpc
        //uint16_t fan_para_alarm_high_temp; //thanhcm3 add for dpc
        //uint16_t fan_para_alarm_low_temp;//thanhcm3 add for dpc
        
        uint32_t cfgDCOverVolt;
        uint32_t cfgDCUnderVolt;
        uint32_t cfgAcUnderLevel;
        uint32_t cfgAcHighLevel;
        uint32_t cfgBattTestVolt;
        uint32_t cfgOverMajorTempLevel;
        uint32_t cfgLLVDEn;
        uint32_t cfgBLVDEn;
//        uint32_t cfgTestVoltCfg;
//        uint32_t cfgAutoTestDay;
//        uint32_t cfgTestStartTime;
} PrivateMibConfigGroup;

/**
 * @brief MainAlarm group
 **/

typedef struct
{
	uint32_t alarmBattHighTemp;
	uint32_t alarmLLVD;
	uint32_t alarmBLVD;
	uint32_t alarmDCLow;
	uint32_t alarmACmains;
	uint32_t alarmBattBreakerOpen;
	uint32_t alarmLoadBreakerOpen;
	uint32_t alarmRectACFault;
	uint32_t alarmRectNoResp;
	uint32_t alarmManualMode;
	uint32_t alarmACLow;        
        uint32_t alarmSmoke;
        uint32_t alarmEnvDoor;
        uint32_t alarmLIB;
        uint32_t alarmGen;
        uint32_t alarmPM;
        uint32_t alarmVAC;
        uint32_t alarmBM;
        uint32_t alarmSMCB;
        uint32_t alarmOutOfRangeFreq;
        uint32_t alarmFUEL;
        uint32_t alarmISENSE;
        uint32_t alarm_pm_dc;
        //uint32_t alarm_dpc_fan;
        uint32_t alarmOutOfRangeFreq2;
        
        uint32_t alarmLIBPack[16];        
        uint32_t alarmGENPack[2];
        uint32_t alarmBMPack[16];
        uint32_t alarmVACPack[1];
        uint32_t alarmSMCBPack[5];
        uint32_t alarmOutOfRangeFreqPack[2];
        uint32_t alarmFUELPack[2];
        uint32_t alarmOutOfRangeFreqPack2[1];
        uint32_t alarmISENSEPack[1];
        uint32_t alarm_pm_dc_pack[1];
        uint32_t alarm_dpc_fan_pack[1];
        
	uint32_t alarmBattHighTemp_old;
        uint32_t alarmLLVD_old;
	uint32_t alarmBLVD_old;
	uint32_t alarmDCLow_old;
	uint32_t alarmACmains_old;
	uint32_t alarmBattBreakerOpen_old;
	uint32_t alarmLoadBreakerOpen_old;
	uint32_t alarmRectACFault_old;
	uint32_t alarmRectNoResp_old;
	uint32_t alarmManualMode_old;
	uint32_t alarmACLow_old; 
        uint32_t alarmSmoke_old;
        uint32_t alarmEnvDoor_old;        
        uint32_t alarmLIB_old;
        uint32_t alarmGen_old;     
        uint32_t alarmPM_old;
        uint32_t alarmVAC_old;
        uint32_t alarmBM_old;
        uint32_t alarmSMCB_old;
        uint32_t alarmOutOfRangeFreq_Old;
        uint32_t alarmFUEL_old;
        uint32_t alarmISENSE_old;
        uint32_t alarmOutOfRangeFreq2_old;
        
        uint32_t alarmLIBPack_old[16];
        uint32_t alarmGENPack_old[2];
        uint32_t alarmBMPack_old[16];
        uint32_t alarmVACPack_old[1];
        uint32_t alarmSMCBPack_old[5];
        uint32_t alarmOutOfRangeFreqPack_Old[2];
        uint32_t alarmFUELPack_old[2];
        uint32_t alarmOutOfRangeFreqPack2_old[1];
        
	uint32_t alarmBattHighTemp_old2;
        uint32_t alarmLLVD_old2;
	uint32_t alarmBLVD_old2;
	uint32_t alarmDCLow_old2;
	uint32_t alarmACmains_old2;
	uint32_t alarmBattBreakerOpen_old2;
	uint32_t alarmLoadBreakerOpen_old2;
	uint32_t alarmRectACFault_old2;
	uint32_t alarmRectNoResp_old2;
	uint32_t alarmManualMode_old2;
	uint32_t alarmACLow_old2; 
        uint32_t alarmSmoke_old2;
        uint32_t alarmEnvDoor_old2;        
        uint32_t alarmLIB_old2;
        uint32_t alarmGen_old2;
        uint32_t alarmPM_old2;
        uint32_t alarmVAC_old2;
        uint32_t alarmBM_old2;
        uint32_t alarmOutOfRangeFreq_Old2;
        uint32_t alarmFUEL_old2;
        uint32_t alarmISENSE_old2;
        uint32_t alarmOutOfRangeFreq2_old2;
        
        uint32_t alarmLIBPack_old2[16];        
        uint32_t alarmGENPack_old2[2];
        uint32_t alarmBMPack_old2[16];
        uint32_t alarmVACPack_old2[1];
        uint32_t alarmSMCBPack_old2[5];
        uint32_t alarmOutOfRangeFreqPack_Old2[2];
        uint32_t alarmFUELPack_old2[2];
        uint32_t alarmOutOfRangeFreqPack2_Old2[1];
        uint32_t alarmISENSEPack_old2[1];
        uint32_t alarm_pm_dc_pack_old2[1];
        
        uint32_t alarmACSPD;
        uint32_t alarmDCSPD;
        
        uint32_t alarmACSPD_old;
        uint32_t alarmDCSPD_old;
} PrivateMibMainAlarmGroup;

/**
 * @brief DIAlarm group
 **/

typedef struct
{
	uint32_t alarmDigitalInput[8];
        uint32_t alarmDigitalInput_old[8];
        uint32_t alarmDigitalInput_old2[8];
} PrivateMibDIAlarmGroup;
/**
 * @brief ConnAlarm group
 **/

typedef struct
{
    uint32_t alarmLIBConnect;
    uint32_t alarmPMUConnect;
    uint32_t alarmGenConnect;  
    uint32_t alarmBMConnect; 
    uint32_t alarmPMConnect;
    uint32_t alarmVACConnect;    
    uint32_t alarmSMCBConnect;   
    uint32_t alarmFUELConnect; 
    uint32_t alarmISENSEConnect;
    uint32_t alarm_pm_dc_connect;
    uint32_t alarm_dpc_fan_connect;
    
    uint32_t alarmLIBConnect_old;
    uint32_t alarmPMUConnect_old;
    uint32_t alarmGenConnect_old; 
    uint32_t alarmBMConnect_old; 
    uint32_t alarmPMConnect_old;
    uint32_t alarmVACConnect_old;     
    uint32_t alarmSMCBConnect_old;    
    uint32_t alarmFUELConnect_old;    
    uint32_t alarmISENSEConnect_old;  
    
    uint32_t alarmLIBConnect_old2;
    uint32_t alarmPMUConnect_old2;
    uint32_t alarmGenConnect_old2;
    uint32_t alarmBMConnect_old2; 
    uint32_t alarmPMConnect_old2;
    uint32_t alarmVACConnect_old2;  
    uint32_t alarmSMCBConnect_old2;  
    uint32_t alarmFUELConnect_old2;  
    uint32_t alarmISENSEConnect_old2;
    uint32_t alarm_dpc_fan_connect_old2; 
    
    uint32_t alarmPMUConnect_old3; //fix loi ko bao trap SNMP cu tu nguon Agisson.
} PrivateMibConnAlarmGroup;
/**
 * @brief SNMPConfig group
 **/

typedef struct
{
        uint8_t u8TrapMode;
        uint32_t u32TrapInterval;
        uint32_t u32TrapTick;
        char_t cFTPServer[40];
        size_t cFTPServerLen;
	char_t cSyncTime[20];
	size_t cSyncTimeLen;
        tEthernet_Setting_Struct siteNetworkInfo;	
        uint8_t u8NewFWFlag;
} PrivateMibCfgNetworkGroup;

/**
 * @brief Private MIB base
 **/

typedef struct
{
   char_t testString[PRIVATE_MIB_TEST_STRING_SIZE];
   size_t testStringLen;
   int32_t testInteger;
   uint32_t currentTime;
   int32_t ledCount;
//   PrivateMibLedEntry ledTable[PRIVATE_MIB_LED_COUNT];
   
   PrivateMibSiteInfoGroup siteGroup;
   PrivateMibRectInfoGroup rectGroup;
   PrivateMibAcPhaseGroup acPhaseGroup;   
   PrivateMibBatteryGroup batteryGroup;
   PrivateMibLoadGroup loadGroup;
   PrivateMibCntGroup  cntGroup;
#if (USERDEF_MONITOR_BM == ENABLED)
   PrivateMibBMGroup bmGroup;
#endif   
#if (USERDEF_MONITOR_PM == ENABLED)   
   PrivateMibPMGroup pmGroup;
#endif
#if (USERDEF_MONITOR_ISENSE == ENABLED)   
   PrivateMibISENSEGroup isenseGroup;
#endif
#if (USERDEF_MONITOR_PM_DC == ENABLED)
   PrivateMibPMDCGroup   pm_dc_group;
#endif
#if (USERDEF_MONITOR_VAC == ENABLED)
   PrivateMibVACGroup vacGroup;
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) // smcb
   PrivateMibSMCBInfoGroup smcbGroup; 
#endif
#if (USERDEF_MONITOR_FUEL == ENABLED) 
   PrivateMibFUELInfoGroup fuelGroup; 
#endif
#if (USERDEF_RS485_DKD51_BDP == ENABLED)
   PrivateMibFANDPCInfo    fan_dpc_info;
#endif 
   PrivateMibLIBattInfoGroup liBattGroup;
   PrivateMibGenInfoGroup genGroup;
   PrivateMibCfgBTGroup cfgBTGroup;
   PrivateMibConfigGroup configGroup;
   PrivateMibMainAlarmGroup mainAlarmGroup;
   PrivateMibDIAlarmGroup diAlarmGroup;
   PrivateMibConnAlarmGroup connAlarmGroup;
   PrivateMibCfgNetworkGroup cfgNetworkGroup;
} PrivateMibBase;


//Private MIB related constants
extern PrivateMibBase privateMibBase;
extern const MibObject privateMibObjects[];
extern const MibModule privateMibModule;

#endif
