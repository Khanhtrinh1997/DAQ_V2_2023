#include "board.h"
#include "fsl_gpio.h"
#include "fsl_uart.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "variables.h"

//#define RS4851_UART 						UART3
//#define RS4851_UART_CLKSRC 			SYS_CLK
//#define RS4851_UART_IRQn 				UART3_RX_TX_IRQn
//#define RS4851_UART_IRQHandler 		UART3_RX_TX_IRQHandler
//
//#define RS4852_UART 						UART4
//#define RS4852_UART_CLKSRC 			SYS_CLK
//#define RS4852_UART_IRQn 				UART4_RX_TX_IRQn
//#define RS4852_UART_IRQHandler 		UART4_RX_TX_IRQHandler

#define RS4851R_UART 	                UART3
#define RS4851R_UART_CLKSRC 		BUS_CLK
#define RS4851R_UART_IRQn 		UART3_RX_TX_IRQn
#define RS4851R_MODBUS_UART_IRQHandler  	UART3_RX_TX_IRQHandler

#define RS4851T_UART 			UART3
#define RS4851T_UART_CLKSRC 		BUS_CLK
#define RS4851T_UART_IRQn 		UART3_RX_TX_IRQn
//#define RS4851T_MODBUS_UART_IRQHandler 		UART3_RX_TX_IRQHandler

#define RS4851R_UART_BAUDRATE 14400//28800
#define RS4851T_UART_BAUDRATE 14400//28800
#define RS4851R_UART_BAUDRATE_2400  3600 //for yada_dpc
#define RS4851T_UART_BAUDRATE_9600  //An 2022

#define RS4851_UART_BAUDRATE 43200//28800
//#define RS4852_UART_BAUDRATE 43200//28800
//thanhcm3 vac update v2==================================================
#define CHECK_TRUE                              1 
#define CHECK_FALSE                             0

#define ATCMD_VAC_STATE_CHECK                   ":at+VACboot?\r\n"
#define ATCMD_VAC_STATE_RESPON                  ":at+VACboot=1\r\n"

#define ATCMD_VAC_RETURN_APP_REQ                ":at+VACreturnapp=1\r\n"
#define ATCMD_VAC_RETURN_APP_RES                ":at+VACreturnappok\r\n"

#define STATE_VAC_V2_APP                        1
#define STATE_VAC_V2_BOOT                       2
#define STATE_VAC_V2_DISCONNECT                 3
//==========================================================================
#define STATE_ON                                1
#define STATE_OFF                               0

#define ERR                                     1
#define NO_ERR                                  0

#define EMER                                    0
#define ZTE                                     1
#define HW                                      2
#define EMERV21                                 3
#define DKD51_BDP                               4
#define VERTIV_M830B                            5
void RS4851R_MODBUS_UART_IRQHandler(void);


//typedef enum
//{
//KUBOTA_INFO_1,
//KUBOTA_INFO_2
//}
//tGenState;

typedef enum
{
    _WAIT_MODE,
    _READ_BATT_STATUS,    
    _WAIT_BATT_RESPOND,
    _READ_LIB_STATUS,    
    _WAIT_LIB_RESPOND,
    _READ_GEN_STATUS,    
    _WAIT_GEN_RESPOND,
    _READ_PM_STATUS,    
    _WAIT_PM_RESPOND,
    _READ_VAC_STATUS,    
    _WAIT_VAC_RESPOND,
    _READ_SMCB_STATUS,    
    _WAIT_SMCB_RESPOND,
    _READ_FUEL_STATUS,    
    _WAIT_FUEL_RESPOND,
    _READ_ISENSE_STATUS,    
    _WAIT_ISENSE_RESPOND,
    _READ_PM_DC_STATUS,
    _WAIT_PM_DC_RESPOND,
    _READ_BM_STATUS,    
    _WAIT_BM_RESPOND,
    _READ_FAN_DPC_STATUS,
    _WAIT_FAN_DPC_RESPOND,
    _READ_TAZ_STATUS,    
    _WAIT_TAZ_RESPOND,
    _READ_ACM_STATUS,    
    _WAIT_ACM_RESPOND,
    _READ_DCU_STATUS,    
    _WAIT_DCU_RESPOND,
    _WRITE_SINGLE_REG,    
    _WAIT_WRITE_MULTI_REG,
    _TEST_MODE,
}
tRTUState;

enum
{
    _READ_COIL_STATUS 			= 1,
    _READ_INPUT_STATUS			= 2,
    _READ_HOLDING_REGS 			= 3,    // 4x
    _READ_INPUT_REG 			= 4,    // 3x
    _FORCE_SINGLE_COIL 			= 5,
    _PRESET_SINGLE_REG 			= 6,
    _READ_EXCEPTION_STATUS 		= 7,
    _FETCH_COMM_EVENT_COUNTER 	= 11,
    _FET_COMM_EVENT_LOG 		= 12,
    _FORCE_MULTIPLE_COILS  		= 15,
    _PRESET_MULTIPLE_REGS 		= 16,
    _REPORT_SLAVE_ID 			= 17,
    _READ_GENERAL_REFERENCE 	= 20,
    _WRITE_GENERAL_REFERENCE 	= 21,
    _MASK_WRITE_4X_REG 			= 22,
    _READ_WRITE_4X_REGS 		= 23,
    _READ_FIFO_QUEUE 			= 24,
    _SETUP_REGISTER				= 49,
    _USER_REGISTER 				= 50,    
    _EXCEPTION_READ_HOLDING = 0x83,
    _EXCEPTION_READ_INPUT = 0x84,
    _EXCEPTION_WRITE_MULTI_REG = 0x90,
};
typedef enum
{
_DKD51_FAN_SYS_REQ,
_DKD51_FAN_SYS_RES,
_DKD51_FAN_STATE_REQ,
_DKD51_FAN_STATE_RES,
_DKD51_FAN_WARNING_REQ,
_DKD51_FAN_WARNING_RES,
_DKD51_FAN_REMOTE_REQ,
_DKD51_FAN_REMOTE_RES,
_DKD51_FAN_PARAMETER_REQ,
_DKD51_FAN_PARAMETER_RES,
_DKD51_FAN_EQUIPMENT_REQ,
_DKD51_FAN_EQUIPMENT_RES,
}FAN_DPC_STATE_E;

typedef struct
{
    tRTUState runningStep;
    uint8_t battNorespond;
    uint8_t battError;

    uint8_t u8BuffWrite[256];
    uint8_t u8BuffRead[256];

    uint8_t u8CRCHigh;
    uint8_t u8CRCLow;

    uint8_t u8CRCHighRead;
    uint8_t u8CRCLowRead;

    uint8_t u8SlaveID;
    uint8_t u8FunctionCode;

    uint8_t u8StartHigh;
    uint8_t u8StartLow;
    uint8_t u8IDHigh;
    uint8_t u8IDLow;
    uint8_t u8NumberRegHigh;
    uint8_t u8NumberRegLow;

    uint8_t u8ReadWriteSetup;
    uint8_t u8SetupType;

    uint8_t u8ByteCount;

    uint8_t u8MosbusEn; // =1: reading, =2: finish reading.
    uint8_t u8DataPointer;
} sMODBUSRTU_struct;
// for fan dpc--------------------------------------------------------------------------
typedef struct{
  uint8_t    buff_485[500];
  uint16_t   cnt;
}RS_485_CN_T;


typedef struct 
{
  uint8_t            rx_enable;
  RS_485_CN_T        rx;
  RS_485_CN_T        uart_rx;
  uint16_t           check_sum_read;
  uint16_t           check_sum_calculator;
  FAN_DPC_STATE_E    fan_state_e;
}FAN_DPC_SWITCH_UART_T;

extern FAN_DPC_SWITCH_UART_T fan_dpc_switch_uart_t;

// for vac update v2 dpc-----------------------------------------------------------------
typedef struct{
  uint8_t buff[50];
  uint16_t rxcnt;
}VAC_V2UPDATE_RECEIVE_struct;
extern VAC_V2UPDATE_RECEIVE_struct vac_v2_update_receive;
extern sMODBUSRTU_struct Modbus;

extern uint16_t LoadCurr, ACFaultM1,PMUConnectM1;

void Init_RS485_MODBUS_UART (void);
void ReInit_RS485_MODBUS_UART (void);
void ReInit_RS485_MODBUS_UART_yada_dpc (void);
void ReInit2_RS485_MODBUS_UART (uint32_t baud);
void ModbusCRC(unsigned char *str, unsigned char len);
void ReInit_RS485_MODBUS_UART_2022 (uint32_t baud, uart_parity_mode_t parity);  

int8_t RS4851_Check_Respond_Setting_Data (void);

int8_t RS4851_Check_Respond_Data_LIB (void);

int8_t RS4851_Check_Respond_Data_GEN (void);
#if (USERDEF_MONITOR_PM == ENABLED)
int8_t RS4851_Check_Respond_Data_PM (void);
#endif
#if (USERDEF_MONITOR_ISENSE == ENABLED)
int8_t RS4851_Check_Respond_Data_ISENSE (void);
#endif
#if (USERDEF_MONITOR_VAC == ENABLED)
int8_t RS4851_Check_Respond_Data_VAC (void);
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) // smcb
int8_t RS4851_Check_Respond_Data_SMCB (void);
#endif
#if (USERDEF_MONITOR_FUEL == ENABLED) // fuel
int8_t RS4851_Check_Respond_Data_FUEL (void);
void Extract_Holding_Regs_Data_FUEL (void);
#endif
#if (USERDEF_MONITOR_BM == ENABLED)
int8_t RS4851_Check_Respond_Data_BM (void);
#endif
#if (USERDEF_MONITOR_PM_DC == ENABLED)
int8_t RS4851_Check_Respond_Data_PM_DC (void);
#endif
void Read_Coil_Status_Query (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);
void Read_Input_Status_Query (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);
void Read_Holding_Regs_Query (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);
void Read_Holding_Regs_Query_yada (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);
void Read_Input_Regs_Query (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);
void Read_Holding_Regs_Query_Deepsea (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);
void Read_Product_Query (uint8_t slaveAddr);
void Read_Input_Regs_Query_coslight (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);
void Read_Input_Status_Query_coslight (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);
void Read_Holding_Regs_Query_coslight (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);


void Write_Single_Reg (uint8_t slaveAddr, uint16_t regAddr, uint16_t writeVal);
void Write_Multi_Reg (uint8_t slaveAddr, uint16_t regAddr, uint8_t *writeBuff, uint8_t size);
void Read_Input_Status_Query_fast (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);
void Read_Input_Regs_Query_fast (uint8_t slaveAddr, uint16_t startingAddr, uint16_t noPoint);

//void SetVar_Time(uint8_t slaveAddr, uint16_t regAddr,TimeFormat setTime);
void SetVar_f32(uint8_t slaveAddr, uint16_t regAddr, float val);
void SetVar_i32(uint8_t slaveAddr, uint16_t regAddr, int32_t val);
void SetVar_i16(uint8_t slaveAddr, uint16_t regAddr, uint16_t val);
void SetVar_Time(uint8_t slaveAddr, uint16_t regAddr, TimeFormat setTime);
void Write_Time_Reg (uint8_t slaveAddr, uint16_t regAddr, uint16_t writeVal);
void delay2(void);
void delay_deepsea(void);
void delay_deepsea_coslight(void);

void delay2_yada(void);
void modbusParsebit(uint8_t *buf, uint8_t *varType1, uint32_t pos, uint8_t coil);
void modbusParseInfoSign(uint8_t *buf, int8_t *varType1,int16_t *varType2,int32_t *varType3,uint32_t pos, uint8_t type);
void modbusParseInfoSignInverse(uint8_t *buf, int8_t *varType1,int16_t *varType2,int32_t *varType3,uint32_t pos, uint8_t type);
void modbusParseInfo(uint8_t *buf, uint8_t *varType1,uint16_t *varType2,uint32_t *varType3,uint32_t pos, uint8_t type);
void modbusParseInfoInverse(uint8_t *buf, uint8_t *varType1,uint16_t *varType2,uint32_t *varType3,uint32_t pos, uint8_t type);
void modbusParseInfofInverse(uint8_t *buf, float *varType4, uint32_t pos);
void modbusParseInfofInverse2(uint8_t *buf, float *varType4, uint32_t pos);
void modbusParseInfof(uint8_t *buf, float *varType4, uint32_t pos);
void modbusParseInfoString2(uint8_t *buf, uint8_t *dstBuf, uint8_t numOfByte, uint32_t pos);

void Extract_Holding_Regs_Data_LIB (void);
void SetVar_i32LIB(uint8_t slaveAddr, uint16_t regAddr, int32_t val);
void SetVar_u16LIB(uint8_t slaveAddr, uint16_t regAddr, uint16_t val);
void Extract_Holding_Regs_Data_GEN (void);
void SetVar_i16GEN(uint8_t slaveAddr, uint16_t regAddr, int16_t val);
void SetVar_FC06GEN(uint8_t slaveAddr, uint16_t regAddr, uint16_t Val);
#if (USERDEF_MONITOR_PM == ENABLED)
void Extract_Holding_Regs_Data_PM (void);
#endif
#if (USERDEF_MONITOR_ISENSE == ENABLED)
void Extract_Holding_Regs_Data_ISENSE (void);
#endif
#if (USERDEF_MONITOR_PM_DC == ENABLED)
void Extract_Holding_Regs_Data_PM_DC(void);
#endif 
#if (USERDEF_MONITOR_VAC == ENABLED)
extern uint32_t VacResetOK;
void Extract_Holding_Regs_Data_VAC (void);
void SetVar_i16VAC(uint8_t slaveAddr, uint16_t regAddr, int16_t val);
void SetVar_i16VACUpdate(uint8_t slaveAddr, uint16_t regAddr, int16_t val);
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) // smcb
void Extract_Holding_Regs_Data_SMCB (void);
void SetVar_FC06SMCB(uint8_t slaveAddr, uint16_t regAddr, uint16_t Val);
#endif
#if (USERDEF_MONITOR_BM == ENABLED)
void Extract_Holding_Regs_Data_BM (void);
#endif
void modbusParseInfobcd(uint8_t *buf, uint8_t *varType1,uint16_t *varType2,uint32_t *varType3,uint32_t pos, uint8_t type);

uint8_t Check_app(uint8_t slaveAddr, uint16_t regAddr, int16_t val);
void vac_v2_send_block(uint8_t *data, size_t length);
uint8_t VAC_V2_check_boot();
uint8_t VAC_V2_check_state();
void SetVar_i16VACUpdate_v2(uint8_t slaveAddr, uint16_t regAddr, int16_t val);
uint8_t DKD51_check_sum_fan(FAN_DPC_SWITCH_UART_T* p);
uint8_t DKD51_check_RTN_fan(FAN_DPC_SWITCH_UART_T* p);
void DKD51_clean_buff_fan(FAN_DPC_SWITCH_UART_T* p);