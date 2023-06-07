#ifndef SOURCE_AGISSONDRIVER_H_
#define SOURCE_AGISSONDRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h> 
#include "variables.h"
//#define FLASH_PB_LOG_START 0xFF00
//#define FLASH_PB1_LOG_START 0x1FF00
//#define FLASH_PB2_LOG_START 0x2FF00
//#define FLASH_PB3_LOG_START 0x3FF00

#if (USERDEF_RS485_AGISSON == ENABLED)
//#define Mesg1 (RS485Timer == 105)|| (RS485Timer == 106)|| (RS485Timer == 107)|| (RS485Timer == 108)|| (RS485Timer == 109)|| (RS485Timer == 110)              \
//               || (RS485Timer == 111)|| (RS485Timer == 115)|| (RS485Timer == 120)|| (RS485Timer == 121)|| (RS485Timer == 122)|| (RS485Timer == 123)|| (RS485Timer == 124)    \
//               || (RS485Timer == 125)|| (RS485Timer == 126)|| (RS485Timer == 127)|| (RS485Timer == 105)|| (RS485Timer == 105)|| (RS485Timer == 105)|| (RS485Timer == 105)     \
//               || (RS485Timer == 105)|| (RS485Timer == 105)|| (RS485Timer == 105)|| (RS485Timer == 105)|| (RS485Timer == 105)|| (RS485Timer == 105)|| (RS485Timer == 105) 
void delay(void);
extern uint8_t Batt_Charge_Volt[15];
extern uint8_t Batt_Info[21];
extern uint8_t LVD[26];
extern uint8_t Batt_Temp[44];
extern uint8_t Volt_Limit[23];
extern uint8_t Env_Info[39];
extern uint8_t Batt_HTD[11];
extern uint8_t Batt_Manual_Test_En[8];
extern uint8_t Batt_Short_Test_En[8];
extern uint8_t Batt_Test_Stop[8];
extern uint8_t ih;
extern uint16_t HisCount_old[5];
extern uint8_t thao;
extern uint32_t Cab_Connection;     
extern uint8_t CPMU01_Ver_IP;

struct uartinfoget
{
	uint8_t AC_Status;
	uint8_t AC_Low;
	uint8_t DC_Status;
	uint8_t LLVD_Status;
	uint8_t BLVD_Status;
	uint8_t Batt_Temp_Status;
	uint8_t Load_Status;
	uint8_t DI_Alarm;

	uint8_t Rect1_Status;
	uint8_t Rect2_Status;
	uint8_t Rect3_Status;
	uint8_t Rect4_Status;
	uint8_t Rect5_Status;
	uint8_t Rect6_Status;

	uint8_t Batt_Temp1[4];
	uint8_t Batt_Temp2[4];
	uint8_t Env_Temp2[4];

	uint8_t Batt_Boost_Volt[4];
	uint8_t Batt_Float_Volt[4];

	uint8_t Batt_Cycled_BC;
	uint8_t Batt_I_Charge_coff[4];
	uint8_t Batt_1_Cap[4];
	uint8_t Batt_2_Cap[4];
	uint8_t LLVD_Allow;
	uint8_t BLVD_Allow;
	uint8_t LLVD_Volt[4];
	uint8_t BLVD_Volt[4];
        uint8_t StdBattTestEndVolt[4];
	uint8_t StdBattTestTime[4];
        uint8_t ShortBattTestEndVolt[4];

        
	uint8_t Batt_Temp_Meas_Upper_Limit[4];
	uint8_t Batt_Temp_Meas_Lower_Limit[4];
	uint8_t Batt_Temp_High_Alarm[4];
	uint8_t Batt_Temp_Low_Alarm[4];
	uint8_t Batt_Temp_Coff[4];

	uint8_t AC_Volt_Over[4];
	uint8_t AC_Volt_Under[4];
	uint8_t DC_Volt_Over[4];
	uint8_t DC_Volt_Under[4];
	uint8_t V_load[4];
	uint8_t I_load[4];
	uint8_t I_charge[4];
	uint8_t VAC[4];
	uint8_t Rect1;
	uint8_t Rect1_V[4];
	uint8_t Rect1_I[4];
	uint8_t Rect2;
	uint8_t Rect2_V[4];
	uint8_t Rect2_I[4];
	uint8_t Rect3;
	uint8_t Rect3_V[4];
	uint8_t Rect3_I[4];
	uint8_t Rect4;
	uint8_t Rect4_V[4];
	uint8_t Rect4_I[4];
	uint8_t Rect5;
	uint8_t Rect5_V[4];
	uint8_t Rect5_I[4];
	uint8_t Rect6;
	uint8_t Rect6_V[4];
	uint8_t Rect6_I[4];

};
extern struct uartinfoget Uart_array_data_get;
struct uartget
{
	float Batt_Temp1;
	float Batt_Temp2;
	float Env_Temp2;

	float Batt_Boost_Volt;
	float Batt_Float_Volt;

	float Batt_I_Charge_coff;
	float Batt_1_Cap;

	float LLVD_Volt;
	float BLVD_Volt;

	float Batt_Temp_Meas_Upper_Limit;
	float Batt_Temp_Meas_Lower_Limit;
	float Batt_Temp_High_Alarm;
	float Batt_Temp_Low_Alarm;
	float Batt_Temp_Coff;

	float AC_Volt_Over;
	float AC_Volt_Under;
	float DC_Volt_Over;
	float DC_Volt_Under;
	float V_load;
	float I_load;
	float I_charge;
	float VAC;

	float Rect1_V;
	float Rect1_I;
	float Rect2_V;
	float Rect2_I;
	float Rect3_V;
	float Rect3_I;
	float Rect4_V;
	float Rect4_I;
	float Rect5_V;
	float Rect5_I;
	float Rect6_V;
	float Rect6_I;
	uint8_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
};
extern struct uartget Uart_data_get;
struct uartinfo
{
	float Batt_Boost_Volt;
	float Batt_Float_Volt;
	float Batt_I_coff;
	float Batt_1_Cap;
	float Batt_2_Cap;
	uint8_t Batt_No;
	uint8_t Batt_Duration;
	uint8_t LLVD_Enable;
	uint8_t BLVD_Enable;
	float LLVD_Volt;
	float BLVD_Volt;
	float Batt_Temp_Meas_Upper_Limit;
	float Batt_Temp_Meas_Lower_Limit;
	float Batt_Temp_Upper_Limit;
	float Batt_Temp_Lower_Limit;
	float Batt_Temp_Comp_Coff;
	uint8_t Foundation_of_Temp_Comp;
	float AC_Volt_Upper_Limit;
	float AC_Volt_Lower_Limit;
	float DC_Volt_Upper_Limit;
	float DC_Volt_Lower_Limit;
	float Env_Temp_Meas_Upper_Limit;
	float Env_Temp_Meas_Lower_Limit;
	float Env_Temp_Upper_Limit;
	float Env_Temp_Lower_Limit;
	float Env_Hum_Meas_Upper_Limit;
	float Env_Hum_Meas_Lower_Limit;
	float Env_Hum_Upper_Limit;
	float Env_Hum_Lower_Limit;
	uint8_t HTD_Enable;
	uint8_t Batt_HTD_Temp;
};
extern struct uartinfo Uart_data;
typedef struct 
{
  uint8_t Hiscode;
  uint8_t Year;
  uint8_t Month;
  uint8_t Day;
  uint8_t Hour;
  uint8_t Minute; 
  }hisevent;

typedef struct
{
   hisevent HisFlashEvent[40];
   uint16_t Count;
   uint16_t Count_old;
   uint8_t NoSector;
   uint8_t reserve[9];
   uint8_t CODE[2];
 }tHisFlashLog;

extern tHisFlashLog sHisFlashLog[5];
//extern tHisFlashLog sHisFlashLog1;
//extern tHisFlashLog sHisFlashLog2;
//extern tHisFlashLog sHisFlashLog3;
extern tHisFlashLog *pHisFlashLog;
static unsigned char AcmdCodeTable[31][9];
uint8_t ChecksumH(uint8_t *Datachecksum,uint8_t size);
void Send_StartMesg0(uint32_t RS485Timer);
void Send_StartMesg1(uint32_t RS485Timer);
void Send_StartMesg23(uint32_t RS485Timer);
void Send_StartMesg4(uint32_t RS485Timer);
void Send_StartMesg56789(uint32_t RS485Timer);
void Send_StartMesg10(uint32_t RS485Timer);
void Send_RequestMesg(uint32_t RS485Timer,uint32_t RS485Command);
void Recv_Mesg(void);
void DataSetup(uint8_t Setupcode);
void SetFloatVolt(void);
void SetBoostVolt(void);
void SetCCL(void);
void SetBattCap1(void);
void SetBattCap2(void);
void SetLLVD_EN(void);
void SetBLVD_EN(void);
void SetLLVD(void);
void SetBLVD(void);
void SetTempComVal(void);
void SetOverTemp(void);
void SetDClow(void);
void SetACLow(void);
void SetBattTestEndVolt(void);
void SetBattStdCellNo(void);
void SetBattStdTestTime(void);
void SetBattShortTestEndVolt(void);
void SetBattShortTestTime(void);
void Set_Batt_Test_Stop(void);
void Start_Batt_Short_Test(void);
void Start_Batt_Manual_Test(void);
void GetHistoryLog(void);
#endif
void NextHis(void);
extern uint16_t HisCount_old[5];
#endif